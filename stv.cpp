#include "stv.h"
#include <QStringList>
#include <QMessageBox>
#include "countdialog.h"
#include <QSplashScreen>
#include "trackvote.h"
#include <QProgressDialog>
#include <QProgressBar>
#include <QPushButton>
#include "writetologfile.h"

STV::STV()
{

}

STV::STV(FileWork *f)
{
    //fileWork = f;
    f->Read();
    seats = f->get_Seats();
    votes = f->get_Votes();
    names = f->get_Candidate_Names();
    parties = f->get_Candidate_Parties();
    countDialog = new CountDialog();
    connect(countDialog->get_button(), &QPushButton::clicked, this, &STV::continue_count);
}

void STV::start(QProgressDialog *pd)
{
    dialog = pd;

    validate_votes();
    create_candidates();
    size = candidates.size();
    create_valid_votes();
    calculate_quota();
    start_count();
}

void STV::validate_votes()
{
    Validate *v = new Validate();
    valids = v->remove_invalids(votes, names);
    std::random_shuffle(valids.begin(), valids.end());
    invalids = v->get_invalids();
}

void STV::create_candidates()
{
    for (int i = 0; i < names.size(); i++)
    {
        candidates.append(new Candidate(i+1, names[i], parties[i]));
    }
}

void STV::create_valid_votes()
{
    for (int i = 0; i < valids.size(); i++)
    {
        //dialog->setValue(i);
        QString l = valids[i];
        QStringList prefs = l.split(",");
        QList<QPair<int, bool>> preferences;
        foreach (QString pref, prefs)
        {
            if (pref == "")
            {
                preferences.append(qMakePair(0, false));
            }
            else
                preferences.append(qMakePair(pref.toInt(), false));
        }
        validVotes.append(new Vote(i+1, "", preferences));
    }
    //dialog->setValue(valids.size());
}

void STV::calculate_quota()
{
    quota = (valids.size()/(seats+1))+1;//2680
    expenses_recoupment_point = quota/4;
}

void STV::start_count()
{
    distributionInfo = "";
    textForLogFile = "";
    countNumber = 1;
    distribute_first_preferences();
    check_for_elected();
    display_count_info();
}

void STV::distribute_first_preferences()
{
    textForLogFile += "Count " + QString::number(countNumber) + "\n";
    QVector<QList<Vote *>> lists (size);
    for (int i = 0; i < validVotes.size(); i++)
    {
        //dialog->setValue(valids.size()+i);
        Vote *v = validVotes[i];
        //QStringList l = v->get_preferences();
        QList<QPair<int, bool>> l = v->get_preferences();
        //int size = l.size();
        for (int j = 0; j < size; j++)
        {
            if (l[j].first == countNumber && !l[j].second && candidates[j]->get_status() == 0)
            {                
                //candidates[j]->increment_votes(countNumber-1, v);
                lists[j].append(v);
                v->set_route(TrackVote::add_assignment_route_string(candidates[j]));
                l[j].second = true;
                break;
            }
        }
        v->set_preferences(l);
    }
    for (int i = 0; i < size; i++)
    {
        candidates[i]->set_votes(lists[i]);
    }
    //dialog->setValue(valids.size()*2);
    //active = candidates;
}

void STV::check_for_elected()
{
    bool check = false;
    //if (check_if_should_continue())
    //{
        for (int i = 0; i < size && elected.size() < seats; i++)
        {
            if (candidates[i]->get_status() == 0)
            {
                int total = candidates[i]->get_total_votes().size();
                if (total >= quota)
                {
                    check = true;
                    elected.append(candidates[i]);
                    textForLogFile += candidates[i]->get_Name() + " elected\n";
                    //active.removeAt(i);
                    candidates[i]->set_status(1);
                    int surplus = total - quota;
                    candidates[i]->set_surplus(surplus);
                }
            }
        }
    //}
    //else
    //    count_complete();
    if (elected.size() == seats)
    {
        count_complete();
    }
}

void STV::display_count_info()
{
    Count *count = new Count(candidates, elected, eliminated, validVotes, nonTransferableVotesNotEffective, countNumber, distributionInfo);
    counts.append(count);
    countDialog->set_static_count_info(votes.size(), valids.size(), invalids.size(), quota, seats);
    countDialog->set_count_info(count);
    countDialog->set_list(/*candidates, valids.size()+validVotes.size(), dialog*/);
    countDialog->show();
    dialog->close();
    //clear_candidates_votes();
    //check_for_elected();
    //continue_count();
}

void STV::continue_count()
{
    //countDialog->display_progress();
    distributionInfo = "";
    QVector<int> next_preferences (size);
    transferableVotes.clear();
    countNumber++;
    textForLogFile += "\nCount " + QString::number(countNumber) + "\n";
    bool check = true;
    int count = 0;

    if (check_if_should_continue())
    {
        countDialog->reset_ui();
        if(elected.size() > 0)
        {
            for (int i = 0; i < elected.size(); i++)
            {
                if(elected[i]->get_surplus() > 0)
                {
                    count++;
                }
            }
            //if (countNumber == 3)
            //{
            //    check = false;
            //}
            int surplus_to_distribute = check_if_meets_criteria(count, next_preferences);
            if (surplus_to_distribute != 100)
            {
                textForLogFile += "Distribution of " + candidates[surplus_to_distribute]->get_Name() + "'s surplus\n";
                surplus_distribution(candidates[surplus_to_distribute], next_preferences);
            }
            else
            {
                defining_candidates_for_exclusion();
            }
        }
        else
            defining_candidates_for_exclusion();
    }
    else
        count_complete();
}

bool STV::check_if_should_continue()
{
    bool met = true;
    QList<Candidate *> continuing_cands;
    int surpluses = get_current_surplus_total();
    if (elected.size() < seats)
    {
        bool check = false;
        for (int i = 0; i < size; i ++)
        {
            if (candidates[i]->get_surplus() > 0)
            {
                check = true;
            }
            if (candidates[i]->get_status() == 0)
            {
                continuing_cands.append(candidates[i]);
            }
        }
        if (continuing_cands.size() == (seats - elected.size()))
        {
            for (int i = 0; i < continuing_cands.size(); i++)
            {
                textForLogFile += continuing_cands[i]->get_Name() + "elected\n";
            }
            elected.append(continuing_cands);
            met = false;
        }

        else if ((continuing_cands.size() == ((seats - elected.size())+1)))
        {
            int index = 0;
            QList<Candidate *> cands = continuing_cands;
            for (int i = 0; i < continuing_cands.size()-1; i++)
            {
                for (int j = i+1; j < continuing_cands.size(); j++)
                {
                    if (cands[j]->get_total_votes().size() < cands[i]->get_total_votes().size())
                    {
                        Candidate *temp = cands[i];
                        cands[i] = cands[j];
                        cands[j] = temp;
                    }
                }
            }
            if (cands[0]->get_total_votes().size()+surpluses < cands[1]->get_total_votes().size())
            {
                cands.removeFirst();
                for (int i = 0; i < cands.size(); i++)
                {
                    textForLogFile += cands[i]->get_Name() + " elected\n";
                }
                elected.append(cands);
                met = false;
            }
        }

        //else if ()

        else if (elected.size() == (seats-1))
        {
            int largest = 0;
            int surpluses = 0;
            int rest = 0;
            for (int i = 0; i < elected.size(); i++)
            {
                surpluses += elected[i]->get_surplus();
            }
            for (int i = 0; i < continuing_cands.size(); i++)
            {
                largest = continuing_cands[i]->get_total_votes().size();
                for (int j = 0; j < continuing_cands.size(); j++)
                {
                    if (j != i)
                    {
                        rest += continuing_cands[j]->get_total_votes().size();
                    }
                }
                if (largest > rest + surpluses)
                {
                    textForLogFile += continuing_cands[i]->get_Name() + " elected\n";
                    elected.append(continuing_cands[i]);
                    met = false;
                }
            }
        }
    }
    else
        met = false;

    if (!met)
    {
        countDialog->reset_ui();
        display_count_info();
    }
    return met;
}

int STV::check_if_meets_criteria(const int &count, QVector<int> &next_preferences)
{   
    //QVector<int> next_preferences (size);
    int distributable_candidate = 100;
    bool met = false;
    bool check = true;
    Candidate *c;
    QList<Candidate *> candidates_with_surpluses;
    //QList<Vote *> electeds_votes;
    if (count == 0)
    {
        distributable_candidate = 100;
    }
    else if (count == 1)
    {
        //one_surplus(electeds_votes, surplus, c);
        for (int i = 0; i < elected.size(); i++)
        {
            if(elected[i]->get_surplus() > 0)
                c = elected[i];
        }
        QList<Vote *> electeds_votes = c->get_votes_for_particular_count(c->index_of_count_candidate_was_elected_in());
        for (int i = 0; i < electeds_votes.size(); i++)
        {
            bool q = false;
            Vote *v = electeds_votes[i];
            separating_transferable_nonTransferable(1, v, q);
        }
        //QVector<int> type = check_surplus_type(c);
        int surplus = c->get_surplus();
        if (countNumber == 3)
        {
            check = false;
        }
        check_surplus_type(surplus, electeds_votes, next_preferences);
        distributable_candidate = check_criteria(c, next_preferences);
        //QList<Vote *> count_where_surplus_arose_votes = c->get_votes_for_particular_count(countNumber-2);
    }
    else
    {
        int ind = 0;
        ind  = more_than_one_surplus(candidates_with_surpluses, c, next_preferences);
        if (ind != 100)
            distributable_candidate = ind;
    }

    return distributable_candidate;
}

void STV::one_surplus(QList<Vote *> &electeds_votes, int &surplus, Candidate *c)
{
    /*for (int i = 0; i < elected.size(); i++)
    {
        if(elected[i]->get_surplus() > 0)
            c = elected[i];
    }
    electeds_votes = c->get_votes_for_particular_count(c->index_of_count_candidate_was_elected_in());
    for (int i = 0; i < electeds_votes.size(); i++)
    {
        Vote *v = electeds_votes[i];
        int j = countNumber;
        separating_transferable_nonTransferable(j, v);
    }
    //QVector<int> type = check_surplus_type(c);
    surplus = c->get_surplus();
    //QList<Vote *> count_where_surplus_arose_votes = c->get_votes_for_particular_count(countNumber-2);*/
}

int STV::more_than_one_surplus(QList<Candidate *> candidates_with_surpluses, Candidate *c, QVector<int> &next_preferences)
{
    int index = 0;
    int index_to_distribute = 100;
    for (int i = 0; i < elected.size(); i++)
    {
        if (elected[i]->get_surplus() > 0)
        {
            candidates_with_surpluses.append(elected[i]);
        }
    }

    Candidate *temp;
    int size1 = candidates_with_surpluses.size();
    for (int i = 0; i < size1 - 1; i++)
    {
        for (int j = i+1; j < size1; j++)
        {
            if (candidates_with_surpluses[j]->get_surplus() > candidates_with_surpluses[i]->get_surplus())
            {
                temp = candidates_with_surpluses[i];
                candidates_with_surpluses[i] = candidates_with_surpluses[j];
                candidates_with_surpluses[j] = temp;
            }
        }
    }
    int number_of_equals = 1;
    for (int j = 1; j < size1; j++)
    {
        if (candidates_with_surpluses[0]->get_surplus() == candidates_with_surpluses[j]->get_surplus())
        {
            number_of_equals++;
        }
        else
        {
            break;
        }
    }

    if (number_of_equals > 1)
    {
        index_to_distribute = equal_surpluses(candidates_with_surpluses);
    }
    else
    {
        c = candidates_with_surpluses[0];
        QList<Vote *> electeds_votes = c->get_votes_for_particular_count(c->index_of_count_candidate_was_elected_in());
        for (int i = 0; i < electeds_votes.size(); i++)
        {
            bool q = false;
            Vote *v = electeds_votes[i];
            separating_transferable_nonTransferable(1, v, q);
        }
        //QVector<int> type = check_surplus_type(c);
        int surplus = c->get_surplus();
        check_surplus_type(surplus, electeds_votes, next_preferences);
        index_to_distribute = check_criteria(c, next_preferences);
        if (index_to_distribute == 100)
        {
            transferableVotes.clear();
            electeds_votes.clear();
            surplus  = 0;
            for (int i = 0; i < size1; i++)
            {
                electeds_votes.append(candidates_with_surpluses[i]->get_votes_for_particular_count(c->index_of_count_candidate_was_elected_in()));
                surplus += candidates_with_surpluses[i]->get_surplus();
            }
            for (int i = 0; i < electeds_votes.size(); i++)
            {
                bool q = false;
                Vote *v = electeds_votes[i];
                separating_transferable_nonTransferable(1, v, q);
            }
            //QVector<int> type = check_surplus_type(c);
            check_surplus_type(surplus, electeds_votes, next_preferences);
            index_to_distribute = check_criteria(c, next_preferences);
        }
    }

    return index_to_distribute;
}

int STV::equal_surpluses(QList<Candidate *> &candidates_with_surpluses)
{
    int id_to_distribute = 100;

    QList<int> counts_surpluses_occured_in;
    QList<int> earliest_surpluses_indices;
    for (int i = 0; i < candidates_with_surpluses.size(); i++)
    {
        counts_surpluses_occured_in.append(candidates_with_surpluses[i]->index_of_count_candidate_was_elected_in());
    }

    int index = 0;
    earliest_surpluses_indices.append(index);
    for (int i = 1; i < counts_surpluses_occured_in.size(); i++)
    {
        if (counts_surpluses_occured_in[i] < counts_surpluses_occured_in[index])
        {
            earliest_surpluses_indices[index] = i;
        }
        else if (counts_surpluses_occured_in[i] == counts_surpluses_occured_in[index])
        {
            earliest_surpluses_indices.append(i);
        }
    }

    if (earliest_surpluses_indices.size() == 1)
    {
        id_to_distribute = earliest_surpluses_indices[0];
    }
    else
    {
        QList<int> indices;
        indices.append(earliest_surpluses_indices[0]);//->get_votes_for_particular_count(0).size());
        for (int i = 1; i < earliest_surpluses_indices.size(); i++)
        {
            if (candidates[earliest_surpluses_indices[i]]->get_votes_for_particular_count(0).size() > indices[0])
            {
                indices[0] = earliest_surpluses_indices[i];//->get_votes_for_particular_count(0).size();
            }
            else if (candidates[earliest_surpluses_indices[i]]->get_votes_for_particular_count(0).size() == indices[0])
            {
                indices.append(earliest_surpluses_indices[i]);//->get_votes_for_particular_count(0).size());
            }
        }
        if (indices.size() == 1)
        {
            id_to_distribute = indices[0];
        }
        else
        {

        }
    }

    return id_to_distribute;
}

int STV::check_criteria(Candidate *c, QVector<int> &next_preferences)
{
    int surplus_to_distribute_index = 100;
    int index = 0;
    for (int i = 0; i < size; i++)
    {
        if (candidates[i]->get_id() == c->get_id())
        {
            index = i;
            break;
        }
    }

    if (!elects_highest_continuing_candidate(next_preferences))
    {
        if (!brings_lowest_candidate_up(next_preferences))
        {
            if (!qualifies_for_expenses_recoupment(next_preferences))
            {
            }
            else
            {
                surplus_to_distribute_index = index;
            }
        }
        else
        {
            surplus_to_distribute_index = index;
        }
    }
    else
    {
        surplus_to_distribute_index = index;
    }

    return surplus_to_distribute_index;
}

void STV::check_surplus_type(/*Candidate *c, */const int &surplus, QList<Vote *> votes, QVector<int> &next_preferences)
{
    if (transferableVotes.size() > surplus)
    {
        transferables_greater_than_surplus(surplus, votes, next_preferences);
    }
    else
    {
        transferables_equal_to_or_less_than_surplus(surplus, votes, next_preferences);
    }
}

void STV::transferables_greater_than_surplus(/*Candidate *c, */const int &surplus, QList<Vote *> votes, QVector<int> &next_preferences)
{
    //QVector<int> type (size);
    float surplusf = surplus;
    float transferables = transferableVotes.size();
    float ratio = surplusf/transferables;
    QList<QList<Vote *>> amounts = finding_next_valid_preference(countNumber, votes);
    QVector<float> amounts_with_ratio (size);
    QVector<int> a1 (size);
    QVector<int> a (size);
    //int total = 0;
    for (int i = 0; i < size; i++)
    {
        amounts_with_ratio[i] = amounts[i].size()*ratio;
        a[i] = amounts[i].size();
        //total += qRound(amounts_with_ratio[i]);
        //type[i] = qRound(amounts_with_ratio[i]);
    }

    int total = 0;
    for (int i = 0; i < size; i++)
    {
        total += qRound(amounts_with_ratio[i]);
    }

    if (total == surplus)
    {
        for (int i = 0; i < size; i++)
        {
            next_preferences[i] = qRound(amounts_with_ratio[i]);
        }
    }
    else
    {
        sorting_out_fractions(amounts_with_ratio, a, a1, total, surplus);
        for (int i = 0; i < size; i++)
        {
            next_preferences[i] = qRound(amounts_with_ratio[i]);
        }
        /*int index = 0;
        for (int i = 1; i < size; i++)
        {
            if (amounts_with_ratio[i] != 0)
            {
                if (amounts_with_ratio[i] < amounts_with_ratio[index])
                {
                    index = i;
                }
            }
        }

        for (int i = 0; i < size; i++)
        {
            if (i == index)
            {
                next_preferences[i] = (int)(amounts_with_ratio[i]-0.5);
            }
            else
            {
                next_preferences[i] = qRound(amounts_with_ratio[i]);
            }
        }*/
    }
    //return type;
}

void STV::sorting_out_fractions(QVector<float> &amounts_with_ratio, const QVector<int> &a, QVector<int> &a1, const int &total, const int &surplus)
{
    //QVector<float> ratio_amounts = amounts_with_ratio;
    QList<int> indices;
    int current = 0;
    int highest = 0;
    int lowest = 0;
    //int amounts_size = ratio_amounts.size();
    int amounts_size = amounts_with_ratio.size();
    bool check = false;
    for (int i = 0; i < amounts_size && !check; i++)
    {
        if (amounts_with_ratio[i] != 0)
        {
            indices.clear();
            current = i;
            indices.append(current);
            for (int j = current+1; j < amounts_size; j++)
            {
                if (amounts_with_ratio[j] == amounts_with_ratio[current])
                {
                    indices.append(j);
                    check  = true;
                }
            }
        }
    }
    int difference = total - surplus;
    if (difference > 0)
    {
        QList<int> greatest;
        greatest.append(a[indices[0]]);
        for (int i = 1; i < indices.size(); i++)
        {
            if (a[indices[i]] < greatest[0])
            {
                greatest[0] = a[indices[i]];
            }
            else if (a[indices[i]] == greatest[0])
            {
                greatest.append(a[indices[i]]);
            }
        }
        if (greatest.size() == 1)
        {
            lowest = greatest[0];
        }
        else
        {
            QList<int> greatest_original_votes;
            greatest_original_votes.append(indices[0]);//->get_votes_for_particular_count(0).size());
            for (int i = 1; i < indices.size(); i++)
            {
                if (candidates[indices[i]]->get_votes_for_particular_count(0).size() < greatest_original_votes[0])
                {
                    greatest_original_votes[0] = indices[i];//->get_votes_for_particular_count(0).size();
                }
                else if (candidates[indices[i]]->get_votes_for_particular_count(0).size() == greatest_original_votes[0])
                {
                    greatest_original_votes.append(indices[i]);//->get_votes_for_particular_count(0).size());
                }
            }
            if (greatest_original_votes.size() == 1)
            {
                lowest = greatest_original_votes[0];
            }
            else
            {

            }
        }
        for (int i = 0; i < amounts_with_ratio.size(); i++)
        {
            if (i == lowest)
            {
                amounts_with_ratio[i] -= 0.5;
                break;
            }
        }
    }
    else
    {

    }

    /*if (indices.size() == 2)
    {
        int greatest = 0;
        int other = 0;
        if (a[indices[0]] > a[indices[1]])
        {
            greatest = indices[0];
            other = indices[1];
        }
        else if (a[indices[1]] > a[indices[0]])
        {
            greatest = indices[1];
            other = indices[0];
        }
        else
        {
            if (candidates[indices[0]]->get_votes_for_particular_count(0).size() > candidates[indices[1]]->get_votes_for_particular_count(0).size())
            {
                greatest = indices[0];
                other = indices[1];
            }
            else if (candidates[indices[1]]->get_votes_for_particular_count(0).size() > candidates[indices[0]]->get_votes_for_particular_count(0).size())
            {
                greatest = indices[1];
                other = indices[0];
            }
            else
            {
                int size1 = candidates[indices[0]]->get_VotesPerCount().size();
                bool check = false;
                for (int i = 1; i < size1 && !check; i++)
                {
                    if (candidates[indices[0]]->get_votes_for_particular_count(i).size() > candidates[indices[1]]->get_votes_for_particular_count(i).size())
                    {
                        greatest = indices[0];
                        other = indices[1];
                        check = true;
                        break;
                    }
                    else if (candidates[indices[1]]->get_votes_for_particular_count(i).size() > candidates[indices[0]]->get_votes_for_particular_count(i).size())
                    {
                        greatest = indices[1];
                        other = indices[0];
                        check = true;
                        break;
                    }
                }
                if (!check)
                {
                    drawing_lots();
                }
            }
        }

        for (int i = 0; i < amounts_with_ratio.size(); i++)
        {
            if (i == greatest)
            {
                amounts_with_ratio[i] += 0.5;
            }
            else if (i == other)
            {
                amounts_with_ratio[i] -= 0.5;
            }
        }
    }
    else
    {

    }*/

}

void STV::transferables_equal_to_or_less_than_surplus(const int &surplus, QList<Vote *> votes, QVector<int> &next_preferences)
{
    QList<QList<Vote *>> amounts = finding_next_valid_preference(countNumber, votes);
    for (int i = 0; i < size; i++)
    {
        next_preferences[i] = amounts[i].size();
    }
}

/*void STV::transferables_less_than_surplus(const int &surplus, QList<Vote *> votes, QVector<int> &next_preferences)
{
    QList<QList<Vote *>> amounts = finding_next_valid_preference(countNumber, votes);
    for (int i = 0; i < size; i++)
    {
        next_preferences[i] = amounts[i].size();
    }

    int met  = 0;
    for (int i = 0; i < votes.size() && met < total; i++)
    {
        if (votes[i]->get_transferable_to() == 0)
        {

        }
    }
}*/

bool STV::elects_highest_continuing_candidate(const QVector<int> &next_preferences)
{
    bool check = false;
    //Candidate *highest_continuing_candidate;
    QList<int> indices;
    int index = 0;// = candidates[0];
    for (int i = 0; i < size; i++)
    {
        if (candidates[i]->get_status() == 0)
        {
            index = i;
            break;
        }
    }
    indices.append(index);
    for (int i = 0; i < size; i++)
    {
        if (candidates[i]->get_status() == 0)
        {
            if (candidates[i]->get_total_votes().size() > candidates[index]->get_total_votes().size())
            {
                indices[0] = i;
            }
            else if (candidates[i]->get_total_votes().size() == candidates[index]->get_total_votes().size())
            {
                indices.append(i);
            }
        }

    }

    for (int i = 0; i < indices.size(); i++)
    {
        //highest_continuing_candidate = candidates[indices[i]];//counts[countNumber-2]->get_candidates().at(index);
        if ((candidates[indices[i]]->get_total_votes().size() + next_preferences[indices[i]]) > quota)
        {
            check = true;
            break;
        }
    }

    return check;
}

bool STV::brings_lowest_candidate_up(const QVector<int> &next_preferences)
{
    QList<Candidate *> num_of_votes;// = candidates;
    Candidate *temp;
    int index_of_lowest = 0;
    for (int i = 0; i < size; i++)
    {
        if (candidates[i]->get_status() == 0)
        {
            num_of_votes.append(candidates[i]);
        }
    }

    int size1 = num_of_votes.size();
    for (int i = 0; i < size1-1; i++)
    {
        for (int j = i+1; j < size1; j++)
        {
            if (num_of_votes[j]->get_total_votes().size() < num_of_votes[i]->get_total_votes().size())
            {
                temp = num_of_votes[i];
                num_of_votes[i] = num_of_votes[j];
                num_of_votes[j] = temp;
            }
        }
    }

    /*QList<Candidate *> lowest;
    lowest.append(num_of_votes[0]);
    for (int i = 0; i < size1-1; i++)
    {
        if (num_of_votes[0]->get_total_votes().size() == num_of_votes[i]->get_total_votes().size())
        {
            lowest.append(num_of_votes[i]);
        }
        else// if (num_of_votes[0]->get_total_votes().size() < num_of_votes[i]->get_total_votes().size())
        {
            break;
        }
    }

    if (lowest.size() > 1)
    {
        which_surplus_to_distribute(lowest);
    }*/
    index_of_lowest = num_of_votes[0]->get_id()-1;
    int second_lowest = num_of_votes[1]->get_id()-1;
    if (num_of_votes[0]->get_total_votes().size()+ next_preferences[index_of_lowest] >= num_of_votes[1]->get_total_votes().size() + next_preferences[second_lowest])
    {
        return true;
    }
    else
        return false;
}

bool STV::qualifies_for_expenses_recoupment(const QVector<int> &next_preferences)
{
    //QList<Candidate *> independents;
    bool check = false;
    //QString ind = "IND";
    for (int i = 0; i < size; i++)
    {
        //if (candidates[i]->get_Party().toLower() == ind.toLower())
        //{
            if (candidates[i]->get_total_votes().size() < expenses_recoupment_point)
            {
                if (candidates[i]->get_total_votes().size() + next_preferences[i] >= expenses_recoupment_point)
                {
                    check = true;
                    break;
                }
            }
        //}
    }
    return check;
}



void STV::surplus_distribution(Candidate *c, QVector<int> &next_preferences)
{
    distributionInfo += "Distribution of candidate " + c->get_Name() + "'s surplus";
    int surplus = c->get_surplus();
    QList<Vote *> votes = c->get_votes_for_particular_count(c->index_of_count_candidate_was_elected_in());
    int votes_size = votes.size();
    //QVector<QList<Vote *>> transfers (size);
    QList<Vote *> transfers;
    int check = 0;
    for (int i = 0; i < next_preferences.size(); i++)
    {
        check = 0;
        transfers.clear();
        if (candidates[i]->get_status() == 0)
        {
            for (int k = votes.size()-1; k >= 0 && check < next_preferences[i]; k--)
            {
                if (votes[k]->get_transferable_to() == candidates[i]->get_id())
                {
                    votes[k]->set_route(TrackVote::add_transferred_in_surplus_route_string(candidates[i]));
                    transfers.append(votes[k]);
                    votes.removeAt(k);
                    check++;
                }
                //else
                //    k--;
            }
            candidates[i]->set_votes(transfers);
        }
    }
    votes_size = votes.size();
    c->set_votes(votes, c->index_of_count_candidate_was_elected_in());
    if (c->get_total_votes().size() > quota)
    {
        int difference = c->get_total_votes().size() - quota;
        int met = 0;
        for (int i = 0; i < votes.size() && met < difference; i++)
        {
            if (votes[i]->get_transferable_to() == 0)
            {
                votes[i]->set_route(TrackVote::add_non_transferable_route_string(countNumber));
                nonTransferableVotesNotEffective.append(votes[i]);
                votes.removeAt(i);
            }
        }
    }
    //countDialog->set_votes_changes();
    c->set_surplusBeingDistributed(true);
    countDialog->set_votes_changes(candidates);
    check_for_elected();
    display_count_info();
    c->set_surplus(0);
}

void STV::defining_candidates_for_exclusion()
{
    QList<Candidate *> num_of_votes = candidates;
    QList<Candidate *> exclusions;

    for (int i = 0; i < num_of_votes.size();)
    {
        if (num_of_votes[i]->get_status() != 0)
        {
            num_of_votes.removeAt(i);
        }
        else
            i++;
    }
    int size1 = num_of_votes.size();
    for(int i = 0; i < size1-1; i++)
    {
        for (int j = i+1; j < size1; j++)
        {
            if (num_of_votes[j]->get_total_votes().size() < num_of_votes[i]->get_total_votes().size())
            {
                Candidate *temp = num_of_votes[i];
                num_of_votes[i] = num_of_votes[j];
                num_of_votes[j] = temp;
            }
        }
    }
    int amount = get_current_surplus_total();

    int total = amount;
    int nums = 0;
    bool check = false;
    Candidate *highest;
    for (int i = num_of_votes.size()-1; i >= 0  && !check; i--)
    {
        total = amount;
        highest = num_of_votes[i];
        int highest_total = highest->get_total_votes().size();
        for (nums = 0; nums < i; nums++)
        {
            total += num_of_votes[nums]->get_total_votes().size();
            if (total > highest_total)
                break;
        }
        if (total < highest_total)
        {
            check = true;
            for (int x = 0; x < nums; x++)
            {
                exclusions.append(num_of_votes[x]);
            }
        }
        else if (i == 1 && !check)
        {
            exclusions.append(num_of_votes[0]);
        }
    }
    /*amount += num_of_votes[0]->get_total_votes().size();
    bool check = false;
    exclusions.append(num_of_votes[0]);
    for (int i = 1; i < num_of_votes.size()-1; i++)
    {
        for (int j = i+1; j < num_of_votes.size(); j++)
        {
            if (amount+num_of_votes[i]->get_total_votes().size() < num_of_votes[j]->get_total_votes().size())
            {
                //if (exclusions.size() < ((seats - elected.size())-1))
                //{
                    exclusions.append(num_of_votes[i]);
                    amount += num_of_votes[i]->get_total_votes().size();
                //}
                //else
                //    check = true;
                break;
            }
            else
            {
                check = true;
                break;
            }
        }
        if (check)
            break;
    }*/

    /*int equal = 0;
    for (int j = 0; j < exclusions.size(); j++)
    {
        if (exclusions[0]->get_total_votes().size() == exclusions[j]->get_total_votes().size())
        {
            equal++;
        }
        else
            break;
    }

    if (equal > 0)
    {
        equal_lowest_candidates(equal, exclusions);
    }*/


    excluding_candidates(exclusions);
}

void STV::equal_lowest_candidates(const int &equal, QList<Candidate *> &exclusions)
{
    QList<Candidate *> excl = exclusions;
    for (int i = 0; i < excl.size()-1; i++)
    {
        for (int j = i+1; j < excl.size(); j++)
        {
            if (excl[j]->get_votes_for_particular_count(0).size() < excl[i]->get_votes_for_particular_count(0).size())
            {
                Candidate *temp = excl[i];
                excl[i] = excl[j];
                excl[j] = temp;
            }
        }
    }
    if (excl[0]->get_votes_for_particular_count(0).size() < excl[1]->get_votes_for_particular_count(0).size())
    {
        exclusions.clear();
        exclusions.append(excl[0]);
    }
    else
    {

    }

}

bool STV::should_candidate_be_excluded()
{
    return true;
}

void STV::excluding_candidates(QList<Candidate *> exclusions)
{
    for (int i = 0; i < exclusions.size(); i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (exclusions[i]->get_id() == candidates[j]->get_id())
            {
                candidates[j]->set_status(2);
            }
        }
    }
    QVector<QList<Vote *>> lists (size);
    distributionInfo += "Distribution of excluded candidates: ";
    for (int i = 0; i < exclusions.size(); i++)
    {
        distributionInfo += exclusions[i]->get_Name() + "'s, ";
        textForLogFile += exclusions[i]->get_Name() + ", ";
        eliminated.append(exclusions[i]);
        for (int j = 0; j < size; j++)
        {
            if (exclusions[i]->get_id() == candidates[j]->get_id())
            {
                if (elected.size() < seats)
                {
                    distribute_excluded_votes(j, lists);
                }
                break;
            }
        }
    }
    for(int i = 0; i < size; i++)
    {
        if (candidates[i]->get_status() == 0)
        {
            candidates[i]->set_votes(lists[i]);
        }
    }
    distributionInfo += " votes";
    textForLogFile += " excluded\n";
    countDialog->set_votes_changes(candidates);
    for (int i = 0; i < exclusions.size(); i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (exclusions[i]->get_id() == candidates[j]->get_id())
            {
                candidates[j]->clear_votes();
            }
        }
    }
   // }
    check_for_elected();
    display_count_info();
}

void STV::distribute_excluded_votes(const int &j, QVector<QList<Vote *>> &lists)
{
    bool check = false;
    //QVector<QList<Vote *>> lists (size);
    int votes_size = candidates[j]->get_total_votes().size();
    int index = 0;
    for (int i = 0; i < votes_size; i++)
    {
        bool q = false;
        Vote *v = candidates[j]->get_total_votes().at(i);
        separating_transferable_nonTransferable(1, v, q);
    }
    //clear_candidates_votes();
    for (int i = 0; i < votes_size; i++)
    {
        Vote *v = candidates[j]->get_total_votes().at(i);
        int transfer_to = v->get_transferable_to();
        if (transfer_to != 0 && candidates[transfer_to-1]->get_status() == 0)
        {
            //index = distributing_by_next_valid_preference(countNumber, v, 2);
            //if (index != 100)
                lists[transfer_to-1].append(v);
                 v->set_route(TrackVote::add_transferred_in_exclusion_route_string(candidates[transfer_to-1]));
            /*QList<QPair<int, bool>> l = v->get_preferences();
            int size1 = l.size();
            for (int y = 0; y < size1; y++)
            {
                if (l[y].first == x && !l[y].second && candidates[y]->get_status())
                {
                    candidates[y]->increment_votes(v);
                    v->set_route(TrackVote::add_transferred_in_exclusion_route_string(candidates[j], candidates[y]));
                    l[y].second = true;
                    break;
                }
                else if (l[y].first == x && l[y].second && candidates[y]->get_status())
                {
                    x++;
                    distribute_excluded_votes(j, x);
                }
            }*/
        }
        else
        {
            //if (countNumber == 5)
            //{
            //    check = true;
            //}
            v->set_route(TrackVote::add_non_transferable_route_string(countNumber));
                nonTransferableVotesNotEffective.append(v);

        }
    }

    /*for(int i = 0; i < size; i++)
    {
        if (candidates[i]->get_status() == 0)
        {
            candidates[i]->set_votes(lists[i]);
        }
    }*/
    //candidates[j]->set_status(2);
    //candidates[j]->clear_votes();
}

bool STV::separating_transferable_nonTransferable(int j, Vote *v, bool q)
{
    QList<QPair<int, bool>> list = v->get_preferences();
    //int size1 = list.size();
    bool t = false;
    bool check = false;
    //if (v->get_id() == 10224 && countNumber == 5)
    //{
    //    t = true;
    //}
    for (int i = 0; i < size; i++)
    {
            if ((list[i].first == j) && !list[i].second/* && candidates[i]->get_status() == 0*/)
            {
                if (candidates[i]->get_status() == 0)
                {
                    check = true;
                    q = true;
                    v->set_transferable_to(candidates[i]->get_id());
                    list[i].second = true;
                    break;
                }
                else
                {
                    list[i].second = true;
                    j++;
                    if (separating_transferable_nonTransferable(j, v, q))
                    {
                        check = false;
                        q = true;
                        break;
                    }
                }

            }
            else if ((list[i].first == j) && list[i].second/* && candidates[i]->get_status() == 0*/)
            {
                j++;
                if (separating_transferable_nonTransferable(j, v, q))
                {
                    check = false;
                    q = true;
                    break;
                }
            }
    }
    if (check)
    {
        transferableVotes.append(v);
        v->set_preferences(list);
        q = true;
    }
    /*else
    {
        v->set_transferable(false);
        v->set_transferable_to(0);
        //nonTransferableVotesNotEffective.append(v);
    }*/
    return q;
}

QList<QList<Vote *>> STV::finding_next_valid_preference(int j, QList<Vote *> votes)
{
    QList<QList<Vote *>> amounts;
    QList<Vote *> vs;
    for (int i =0; i < size; i++)
    {
        amounts.append(vs);
    }
    for (int x = 0; x < votes.size(); x++)
    {
        Vote *v = votes[x];
        int candidate_to_transfer_to = votes[x]->get_transferable_to();
        if (candidate_to_transfer_to != 0)
        {
            for (int i = 0; i < size; i++)
            {
                if (candidates[i]->get_id() == candidate_to_transfer_to)
                {
                    amounts[i].append(v);
                    break;
                }
            }
        }
        /*if (v->is_transferable())
        {
            QList<QPair<int, bool>> list = v->get_preferences();
            int size1 = list.size();
            bool check = false;
            for (int i = 0; i < size1; i++)
            {
                if ((list[i].first == j) && !list[i].second && candidates[i]->get_status() == 0)
                {
                    check = true;
                    amounts.at(i).append(v);
                    //list[i].second = true;
                    //candidates[i]->increment_votes(v);
                    break;
                    //temp[i]->increment_votes(countNumber, v);
                    //candidates[i]->getVotesPerCount().at(countNumber-1).append(v);
                    //c->getVotesPerCount().at(countNumber-1).remove
                }
                else if (list[i].first == j && list[i].second && candidates[i]->get_status() == 0)
                {
                    j++;
                    finding_next_valid_preference(j, votes);
                }
            }
        }*/
    }
    return amounts;
}

int STV::distributing_by_next_valid_preference(int j, Vote *v, const int &distribution_type)
{
    int i = 100;
    QList<QPair<int, bool>> list = v->get_preferences();
    int size1 = list.size();
    bool check = false;
    for (i = 0; i < size1; i++)
    {
        if ((list[i].first == j) && !list[i].second && candidates[i]->get_status() == 0)
        {
            check = true;
            list[i].second = true;
            //candidates[i]->increment_votes(countNumber-1, v);
            if (distribution_type == 1)
            {
                 v->set_route(TrackVote::add_transferred_in_surplus_route_string(candidates[i]));
            }
            else
            {
                v->set_route(TrackVote::add_transferred_in_exclusion_route_string(candidates[i]));
            }
            break;
            //temp[i]->increment_votes(countNumber, v);
            //candidates[i]->getVotesPerCount().at(countNumber-1).append(v);
            //c->getVotesPerCount().at(countNumber-1).remove
        }
        else if (list[i].first == j && list[i].second && candidates[i]->get_status())
        {
            j++;
            distributing_by_next_valid_preference(j, v, distribution_type);
        }
    }
    //v->set_preferences(list);
    if (!check)
    {
        nonTransferableVotesNotEffective.append(v);
    }
    return i;
}

int STV::get_total_candidate_votes(int j)
{
    int total = 0;
    for (int i = 0; i < counts.size(); i++)
    {
        //total += counts[i]->get_candidates().at(j)->getVotes().size();
    }
    return total;
}

int STV::get_current_surplus_total()
{
    int surpluses = 0;
    for (int i = 0; i < elected.size(); i++)
    {
        surpluses += elected[i]->get_surplus();
    }
    return surpluses;
}

void STV::clear_candidates_votes()
{
    for (int i = 0; i < size; i++)
    {
        candidates[i]->clear_votes();
    }
}

void STV::count_complete()
{
    textForLogFile += "\nCount Complete";
    WriteToLogFile::write_to_file(textForLogFile);
    countDialog->disable_continue_button();
    //QMessageBox box("Count Complete", "CountComplete", countDialog);
    //box.show();
}

void STV::drawing_lots()
{

}

QStringList STV::get_valids()
{
    return valids;
}

QList<Vote *> STV::get_votes()
{
    return validVotes;
}


#include "stv.h"
#include <QStringList>
#include <QMessageBox>
#include "countdialog.h"
#include <QSplashScreen>
#include "trackvote.h"
#include <QProgressDialog>
#include <QProgressBar>
#include <QPushButton>

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
        dialog->setValue(i);
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
    dialog->setValue(valids.size());
}

void STV::calculate_quota()
{
    quota = (valids.size()/(seats+1))+1;//23;
    expenses_recoupment_point = quota/4;
}

void STV::start_count()
{
    distributionInfo = "";
    countNumber = 1;
    distribute_first_preferences();
    check_for_elected();
    display_count_info();
}

void STV::distribute_first_preferences()
{
    QVector<QList<Vote *>> lists (size);
    for (int i = 0; i < validVotes.size(); i++)
    {
        dialog->setValue(valids.size()+i);
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
    //active = candidates;
}

void STV::check_for_elected()
{
    bool check = false;
    for (int i = 0; i < size; i++)
    {
        if (candidates[i]->get_status() == 0)
        {
            int total = candidates[i]->get_total_votes().size();
            if (total >= quota)
            {
                check = true;
                elected.append(candidates[i]);
                //active.removeAt(i);
                candidates[i]->set_status(1);
                int surplus = total - quota;
                candidates[i]->set_surplus(surplus);
            }
        }
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
    dialog->setValue(100000);
    //clear_candidates_votes();
    //check_for_elected();
    //continue_count();
}

void STV::continue_count()
{
    //countDialog->display_progress();
    distributionInfo = "";
    transferableVotes.clear();
    countNumber++;
    int count = 0;
    if (elected.size() < seats)
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
            if (check_if_meets_criteria(count))
            {
                surplus_distribution();
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

bool STV::check_if_meets_criteria(const int &count)
{   
    bool met = false;
    Candidate *c;
    QList<Candidate *> candidates_with_surpluses;
    if (count == 0)
    {
        met = false;
    }
    else if (count == 1)
    {
        for (int i = 0; i < elected.size(); i++)
        {
            if(elected[i]->get_surplus() > 0)
                c = elected[i];
        }
        QList<Vote *> votes = c->get_votes_for_particular_count(c->index_of_count_candidate_was_elected_in());
        for (int i = 0; i < votes.size(); i++)
        {
            Vote *v = votes[i];
            int j = countNumber;
            separating_transferable_nonTransferable(j, v);
        }
        //QVector<int> type = check_surplus_type(c);
        QVector<int> next_preferences (size);
        check_surplus_type(c, next_preferences);
        if (!elects_highest_continuing_candidate(next_preferences))
        {
            if (!brings_lowest_candidate_up(next_preferences))
            {
                if (!qualifies_for_expenses_recoupment(next_preferences))
                {
                    met = false;
                }
                else
                {
                    met = true;
                }
            }
            else
            {
                met = true;
            }
        }
        else
        {
            met = true;
        }
    }
    else
    {
        for (int i = 0; i < size; i++)
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


    }
    return met;
}

void STV::check_surplus_type(Candidate *c, QVector<int> &next_preferences)
{
    int surplus = c->get_surplus();
    //QVector<int> type (size);
    //double ratio = 0.0;
    /*for (int i = 0; i < size; i++)
    {
        //Vote *v = c->getVotesPerCount().at(countNumber-1)[i];
        Vote *v = c->getVotes().at(i);
        int j = countNumber+1;
        separating_transferable_nonTransferable(j, v);
    }*/

    if (transferableVotes.size() > surplus)
    {
        transferables_greater_than_surplus(c, next_preferences);
        /*QList<Vote *> temp;
        ratio = surplus/transferableVotes.size();
        for (int x = 0; x < surplus; x++)
        {
            Vote *v = c->getVotesPerCount().at(countNumber-1)[x];
            QList<QPair<int, bool>> list = v->get_preferences();
            int j = countNumber+1;
            finding_next_valid_preference(j, list, c, v, temp);
        }
        for (int x = 0; x < temp.size(); x++)
        {
            int num_of_votes = (temp[x]->getVotesPerCount().at(countNumber-1).size()*ratio);
            for (int z = 0; z < num_of_votes; z++)
            {
                Vote *v1 = temp[x];
                //candidates[x]->increment_votes(countNumber, );
                //c->getVotesPerCount().at(countNumber-1)
            }

        }*/
    }
    else if (transferableVotes.size() == surplus)
    {
        next_preferences = transferables_equal_to_surplus();
    }
    else
    {
        next_preferences = transferables_less_than_surplus();
    }
    //return type;
}

void STV::transferables_greater_than_surplus(Candidate *c, QVector<int> &next_preferences)
{
    //QVector<int> type (size);
    float surplus = c->get_surplus();
    float transferables = transferableVotes.size();
    float ratio = surplus/transferables;
    QVector<int> amounts = finding_next_valid_preference(countNumber, c->get_votes_for_particular_count(countNumber-2));
    QVector<float> amounts_with_ratio (size);
    for (int i = 0; i < size; i++)
    {
        amounts_with_ratio[i] = amounts[i]*ratio;
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

    }
    //return type;
}

QVector<int> STV::transferables_equal_to_surplus()
{
    QVector<int> type (size);
    return type;
}

QVector<int> STV::transferables_less_than_surplus()
{
    QVector<int> type (size);
    return type;
}

bool STV::elects_highest_continuing_candidate(const QVector<int> &next_preferences)
{
    Candidate *highest_continuing_candidate;
    int index = 0;// = candidates[0];
    for (int i = 1; i < size; i++)
    {
        if (candidates[i]->get_status() == 0)
        {
            if (candidates[i]->get_total_votes().size() > candidates[index]->get_total_votes().size())
            {
                index = i;
            }
        }
    }

    highest_continuing_candidate = counts[countNumber-2]->get_candidates().at(index);

    if ((highest_continuing_candidate->get_total_votes().size() + next_preferences[index]) > quota)
    {
        return true;
    }
    else
        return false;
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

    if (num_of_votes[0]->get_total_votes().size()+ next_preferences[index_of_lowest] >= num_of_votes[1]->get_total_votes().size())
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
    QString ind = "IND";
    for (int i = 0; i < size; i++)
    {
        if (candidates[i]->get_Party().toLower() == ind.toLower())
        {
            if (candidates[i]->get_total_votes().size() < expenses_recoupment_point)
            {
                if (candidates[i]->get_total_votes().size() + next_preferences[i] >= expenses_recoupment_point)
                {
                    check = true;
                    break;
                }
            }
        }
    }
    return check;
}



void STV::surplus_distribution()
{
    int electeds_size = elected.size();
    int total = 0;
    int largest = 0;
    int index = 0;
    if (size == 1)
    {
        for (int i = 0; i < electeds_size; i++)
        {
            if (elected[i]->get_surplus() != 0)
            {
                distributionInfo += "Distribtuion of " + elected[i]->get_Name() + "'s surplus\n";
                check_surplus_type(i);
            }
        }
    }
    else
    {
        for (int i = 0; i < electeds_size; i++)
        {
            if (elected[i]->get_surplus() != 0)
            {
                if (elected[i]->get_surplus() > largest)
                {
                    largest = elected[i]->get_surplus();
                    index = i;
                }
                total += elected[i]->get_surplus();
            }
        }
        //check_surplus_type(i);
    }

    //QList<int> surpluses;
    /*if (count == 1)
    {
        for (int i = 0; i < surpluses.size(); i++)
        {
            if (surpluses[i] != 0)
            {
                check_surplus_type(i);
                break;
            }
        }
    }
    else if (count > 1)
    {
        int surplus = 0;
        int total_surplus = 0;
        for (int i = 0; i < candidates.size(); i++)
        {
            if (candidates[i]->get_surplus() != 0)
            {
                surplus = candidates[i]->get_surplus();
            }
        }
    }*/
}

/*void STV::check_surplus_type(const int &i)
{
    Candidate *c = elected[i];
    int surplus = c->get_surplus();
    //int size = c->getVotesPerCount().at(countNumber-1).size();
    //int votes_size = c->get_total_votes().size();
    //double ratio = 0.0;
    /*for (int i = 0; i < size; i++)
    {
        //Vote *v = c->getVotesPerCount().at(countNumber-1)[i];
        Vote *v = c->getVotes().at(i);
        int j = countNumber+1;
        separating_transferable_nonTransferable(j, v);
    }

    if (transferableVotes.size() > surplus)
    {
        //transferables_greater_than_surplus(c);
        /*QList<Vote *> temp;
        ratio = surplus/transferableVotes.size();
        for (int x = 0; x < surplus; x++)
        {
            Vote *v = c->getVotesPerCount().at(countNumber-1)[x];
            QList<QPair<int, bool>> list = v->get_preferences();
            int j = countNumber+1;
            finding_next_valid_preference(j, list, c, v, temp);
        }
        for (int x = 0; x < temp.size(); x++)
        {
            int num_of_votes = (temp[x]->getVotesPerCount().at(countNumber-1).size()*ratio);
            for (int z = 0; z < num_of_votes; z++)
            {
                Vote *v1 = temp[x];
                //candidates[x]->increment_votes(countNumber, );
                //c->getVotesPerCount().at(countNumber-1)
            }

        }
    }
    else if (transferableVotes.size() == surplus)
    {
        transferables_equal_to_surplus();
    }
    else
    {
        transferables_less_than_surplus();
    }
}*/

void STV::defining_candidates_for_exclusion()
{
    QList<Candidate *> num_of_votes = candidates;
    QList<Candidate *> exclusions;

    for(int i = 0; i < size-1; i++)
    {
        for (int j = i+1; j < size; j++)
        {
            if (num_of_votes[j]->get_total_votes().size() < num_of_votes[i]->get_total_votes().size())
            {
                Candidate *temp = num_of_votes[i];
                num_of_votes[i] = num_of_votes[j];
                num_of_votes[j] = temp;
            }
        }
    }
    int lowest_continuing = 0;
    int amount = get_current_surplus_total();
    for (int i = 0; i < size; i++)
    {
        if (num_of_votes[i]->get_status() == 0)
        {
            lowest_continuing = i;
            break;
        }
    }
    amount += num_of_votes[lowest_continuing]->get_total_votes().size();
    exclusions.append(num_of_votes[lowest_continuing]);
    for (int i = lowest_continuing+1; i < num_of_votes.size(); i++)
    {
        if (num_of_votes[i]->get_status() == 0)
        {
            //amount += num_of_votes[i]->getVotes().size();
            if (amount + num_of_votes[i]->get_total_votes().size() > num_of_votes[i+1]->get_total_votes().size())
            {
                exclusions.append(num_of_votes[i]);
            }
            else if (amount = num_of_votes[i]->get_total_votes().size())
            {

            }
            else
                break;
        }
    }
    excluding_candidates(exclusions);
}

void STV::excluding_candidates(QList<Candidate *> exclusions)
{
    distributionInfo += "Distribution of excluded candidates: ";
    for (int i = 0; i < exclusions.size(); i++)
    {
        distributionInfo += exclusions[i]->get_Name() + "'s, ";
        eliminated.append(exclusions[i]);
        for (int j = 0; j < size; j++)
        {
            if (exclusions[i]->get_id() == candidates[j]->get_id())
            {
                distribute_excluded_votes(j);
                break;
            }
        }
    }
    distributionInfo += " votes\n";
    check_for_elected();
    display_count_info();
}

void STV::distribute_excluded_votes(const int &j)
{
    candidates[j]->set_status(2);
    QVector<QList<Vote *>> lists (size);
    int votes_size = candidates[j]->get_total_votes().size();
    int index = 0;
    for (int i = 0; i < votes_size; i++)
    {
        Vote *v = candidates[j]->get_total_votes().at(i);
        separating_transferable_nonTransferable(countNumber, v);
    }
    //clear_candidates_votes();
    for (int i = 0; i < votes_size; i++)
    {
        Vote *v = candidates[j]->get_total_votes().at(i);
        if (v->is_transferable())
        {
            index = distributing_by_next_valid_preference(countNumber, v, 2);
            if (index != 100)
                lists[index].append(v);
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
            nonTransferableVotesNotEffective.append(v);
        }
    }

    for(int i = 0; i < size; i++)
    {
        candidates[i]->set_votes(lists[i]);
    }
    //candidates[j]->set_status(2);
    candidates[j]->clear_votes();
}

void STV::separating_transferable_nonTransferable(int j, Vote *v)
{
    QList<QPair<int, bool>> list = v->get_preferences();
    //int size1 = list.size();
    bool check = false;
    for (int i = 0; i < size; i++)
    {
        if ((list[i].first == j) && !list[i].second && candidates[i]->get_status() == 0)
        {
            check = true;
            break;
        }
        else if ((list[i].first == j) && list[i].second && candidates[i]->get_status() == 0)
        {
            j++;
            separating_transferable_nonTransferable(j, v);
        }
    }
    if (check)
    {
        transferableVotes.append(v);
    }
    else
    {
        v->set_transferable(false);
        //nonTransferableVotesNotEffective.append(v);
    }
}

QVector<int> STV::finding_next_valid_preference(int j, QList<Vote *> votes)
{
    QVector<int> amounts(size);
    for (int x = 0; x < votes.size(); x++)
    {
        Vote *v = votes[x];
        if (v->is_transferable())
        {
            QList<QPair<int, bool>> list = v->get_preferences();
            int size1 = list.size();
            bool check = false;
            for (int i = 0; i < size1; i++)
            {
                if ((list[i].first == j) && !list[i].second && candidates[i]->get_status() == 0)
                {
                    check = true;
                    amounts[i]++;
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
        }
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
    v->set_preferences(list);
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
    countDialog->disable_continue_button();
    //QMessageBox box("Count Complete", "CountComplete", countDialog);
    //box.show();
}

QStringList STV::get_valids()
{
    return valids;
}

QList<Vote *> STV::get_votes()
{
    return validVotes;
}


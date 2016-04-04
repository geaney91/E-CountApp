#include "stv.h"
#include "writetologfile.h"
#include "trackvote.h"
#include "validate.h"
//#include <QMainWindow>
#include <QStringList>
#include <QCoreApplication>
#include <QPushButton>

STV::STV()
{

}

void STV::add_info(FileWork *f, int checked)
{
    this->checked = checked;

    f->Read();
    seats = f->get_Seats();
    votes = f->get_Votes();
    names = f->get_Candidate_Names();
    parties = f->get_Candidate_Parties();
    QCoreApplication::processEvents();

    countDialog = new CountDialog();

    if (checked == 0)
        connect(countDialog->get_button(), &QPushButton::clicked, this, &STV::continue_count);
}

void STV::start()
{
    validate_votes();
    create_candidates();
    QCoreApplication::processEvents();
    candidates_size = candidates.size();
    //QCoreApplication::processEvents();
    create_valid_votes();
    calculate_quota();
    QCoreApplication::processEvents();
    start_count();
}

//Validate andshuffle the ballots
void STV::validate_votes()
{
    Validate *v = new Validate();
    valids = v->remove_invalids(votes);
    //std::random_shuffle(valids.begin(), valids.end());
    invalids = v->get_invalids();
}

//Create candidate objects
void STV::create_candidates()
{
    int size = names.size();
    for (int i = 0; i < size; i++)
    {
        candidates.append(new Candidate(i+1, names[i], parties[i]));
    }
}

//Create vote objects
void STV::create_valid_votes()
{
    int size = valids.size();
    QString l = "";
    QStringList prefs;
    QList<int> preferences;

    for (int i = 0; i < size; i++)
    {
        preferences.clear();
        l = valids[i];
        prefs = l.split(",");
        foreach (const QString pref, prefs)
        {
            if (pref == "")
            {
                preferences.append(0);
            }
            else
                preferences.append(pref.toInt());
        }
        validVotes.append(new Vote(i+1, "", preferences));
        //QCoreApplication::processEvents();
    }
    std::random_shuffle(validVotes.begin(), validVotes.end());
}

void STV::calculate_quota()
{
    quota = (valids.size()/(seats+1))+1;
    expenses_recoupment_point = quota/4;
}

void STV::start_count()
{
    distributionInfo = "";
    textForLogFile = "";
    countNumber = 1;
    distribute_first_preferences();
    QCoreApplication::processEvents();
    check_for_elected();
    QCoreApplication::processEvents();
    display_count_info();
    QCoreApplication::processEvents();
    if (checked == 1)
        continue_count();
    //else
        //display_count_info();
}

//Distribute ballots to candidates by first preferences
void STV::distribute_first_preferences()
{
    QVector<QList<Vote *>> lists (candidates_size);
    int size = validVotes.size();
    Vote *v;
    bool check = false;
    QList<int> l;
    textForLogFile += "Count " + QString::number(1) + "\n";


    for (int i = 0; i < size; i++)
    {
        v = validVotes[i];
        if (v->get_id() == 508)
            check = true;
        l = v->get_preferences();
        for (int j = 0; j < candidates_size; j++)
        {
            if (l[j] == 1)
            {                
                lists[j].append(v);
                v->set_route(TrackVote::add_assignment_route_string(candidates[j]));
                v->set_transferable_to(candidates[j]->get_id());
                break;
            }
        }
        //QCoreApplication::processEvents();
    }

    for (int i = 0; i < candidates_size; i++)
    {
        candidates[i]->set_votes(lists[i]);
    }
    //QCoreApplication::processEvents();
}

//Checks if candidates have reached quota
void STV::check_for_elected()
{
    int surplus = 0;
    int total = 0;
    Candidate *c;

    for (int i = 0; i < candidates_size && elected.size() < seats; i++)
    {
        c = candidates[i];
        if (c->get_status() == 0)
        {
            total = c->get_total_votes().size();
            if (total >= quota)
            {
                elected.append(c);
                textForLogFile += c->get_Name() + " elected\n";
                c->set_status(1);
                surplus = total - quota;
                c->set_surplus(surplus);
            }
        }
    }

    //If all seats are filled the count is complete
    if (elected.size() == seats)
    {
        count_complete();
    }
}

//Creates new Count object with all relevant info and passes to CountDialog to be displayed
//on screen
void STV::display_count_info()
{
    Count *count = new Count(candidates,
                             elected, eliminated, validVotes,
                             nonTransferableVotesNotEffective, countNumber, distributionInfo, seats);
    countDialog->set_count_object(count);
    if (countNumber == 1)
    {
        countDialog->populate_combo_box();
        countDialog->set_static_count_info(votes.size(), valids.size(), invalids.size(), quota, seats);
    }
    countDialog->set_count_info(/*count*/);

    if (checked == 0)
        countDialog->show();

}

//Called each time the button is pushed to continue the count
void STV::continue_count()
{
    countDialog->reset_ui();
    distributionInfo = "";
    QVector<int> next_preferences (candidates_size); //Will be used to store the votes to be transferred to each candidate
                                                        //in surplus distribution
    transferableVotes.clear();
    countNumber++;
    int count = 0;

    if (check_if_should_continue())
    {
        textForLogFile += "\nCount " + QString::number(countNumber) + "\n";
        int elected_size = elected.size();

        if(elected_size > 0)
        {
            for (int i = 0; i < elected_size; i++)
            {
                if(elected[i]->get_surplus() > 0)
                    count++;
            }

            //Returns index of canididate who's surplus is to be distributed if meets criteria
            int surplus_to_distribute = check_if_meets_criteria(count, next_preferences);
            if (surplus_to_distribute != -1)
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

//Checks if count can be concluded or should continue
bool STV::check_if_should_continue()
{
    bool met = true;
    QList<Candidate *> continuing_cands;
    const int surpluses = get_current_surplus_total();
    const int elected_size = elected.size();

    if (elected_size < seats)
    {
        bool check = false;
        for (int i = 0; i < candidates_size; i++)
        {
            if (candidates[i]->get_surplus() > 0)
                check = true;
            if (candidates[i]->get_status() == 0)
                continuing_cands.append(candidates[i]);
        }
        const int continuing_size = continuing_cands.size();
        //Number of caontinuing candidates is equal to number of unfilled seats
        if (continuing_size == (seats - elected_size))
        {
            for (int i = 0; i < continuing_size; i++)
            {
                textForLogFile += continuing_cands[i]->get_Name() + "elected\n";
            }
            elected.append(continuing_cands);
            met = false;
        }

        //Number of continuing candidates is one greater than number of unfilled seats
        else if ((continuing_size == ((seats - elected_size)+1)))
        {
            QList<Candidate *> cands = continuing_cands;
            Candidate *temp;

            //Sort in ascending order
            for (int i = 0; i < continuing_size-1; i++)
            {
                for (int j = i+1; j < continuing_size; j++)
                {
                    if (cands[j]->get_total_votes().size() < cands[i]->get_total_votes().size())
                    {
                        temp = cands[i];
                        cands[i] = cands[j];
                        cands[j] = temp;
                    }
                }
            }

            //If lowest candidate's votes plus any surpluses is less than second lowest candidate's votes,
            //all continuing candidatea bar the lowest are deemed elected
            if (cands[0]->get_total_votes().size()+surpluses < cands[1]->get_total_votes().size())
            {
                cands.removeFirst();
                for (int i = cands.size()-1; i >= 0; i--)
                {
                    textForLogFile += cands[i]->get_Name() + " elected\n";
                    elected.append(cands[i]);
                }
                met = false;
            }
        }

        //One unfilled seat remaining
        else if (elected_size == (seats-1))
        {
            int largest = 0;
            int rest = 0;

            //If candidate with largest number of votes is greater than all other candidates plus surpluses
            //then that candidate is deemed elected
            for (int i = 0; i < continuing_size; i++)
            {
                largest = continuing_cands[i]->get_total_votes().size();
                for (int j = 0; j < continuing_size; j++)
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
        countNumber--;
        display_count_info();
    }
    return met;
}

//Check if surplus meets the criteria to be distributed.
int STV::check_if_meets_criteria(int count, QVector<int> &next_preferences)
{   
    int distributable_candidate = -1;
    const int elected_size = elected.size();
    QList<Candidate *> candidates_with_surpluses;
    QList<Vote *> electeds_votes;
    Candidate *c;

    //No surplus available
    if (count == 0)
    {
        distributable_candidate = -1;
    }

    //1 surplus available
    else if (count == 1)
    {

        for (int i = 0; i < elected_size; i++)
        {
            if(elected[i]->get_surplus() > 0)
                c = elected[i];
        }

        //Get last packet of votes of candidate with surplus
        electeds_votes = c->get_votes_for_particular_count(c->index_of_count_candidate_was_elected_in());
        const int elected_votes_size = electeds_votes.size();
        //Separating transferables and non-transferables
        Vote *v;
        int transfer_to = 0;
        int next_pref = 0;
        bool done = false;

        for (int i = 0; i < elected_votes_size; i++)
        {
            done = false;
            v = electeds_votes[i];
            transfer_to = v->get_transferable_to();
            next_pref = (v->get_preferences().at(transfer_to-1));
            separating_transferable_nonTransferable(next_pref, v, done);
        }
        int surplus = c->get_surplus();
        //Check type of surplus e.g. transferables > surplus
        check_surplus_type(surplus, electeds_votes, next_preferences);
        distributable_candidate = check_criteria(c, next_preferences);
    }

    //More than one surplus available
    else
    {
        distributable_candidate  = more_than_one_surplus(candidates_with_surpluses, next_preferences);
    }

    return distributable_candidate;
}

//More than one surplus available
int STV::more_than_one_surplus(QList<Candidate *> &candidates_with_surpluses, QVector<int> &next_preferences)
{
    int index_to_distribute = -1;

    Candidate *temp;
    Candidate *c;
    QList<Vote *> electeds_votes;

    const int elected_size = elected.size();
    int number_of_equals = 1;

    //Put canididates with surpluses in a list
    for (int i = 0; i < elected_size; i++)
    {
        if (elected[i]->get_surplus() > 0)
        {
            candidates_with_surpluses.append(elected[i]);
        }
    }

    //Sort list in descending order by surplus size
    const int surplus_candidates_size = candidates_with_surpluses.size();
    for (int i = 0; i < surplus_candidates_size - 1; i++)
    {
        for (int j = i+1; j < surplus_candidates_size; j++)
        {
            if (candidates_with_surpluses[j]->get_surplus() > candidates_with_surpluses[i]->get_surplus())
            {
                temp = candidates_with_surpluses[i];
                candidates_with_surpluses[i] = candidates_with_surpluses[j];
                candidates_with_surpluses[j] = temp;
            }
        }
    }

    //Check for equal surpluses
    for (int j = 1; j < surplus_candidates_size; j++)
    {
        if (candidates_with_surpluses[0]->get_surplus() == candidates_with_surpluses[j]->get_surplus())
            number_of_equals++;
        else
            break;
    }

    //If no equal surpluses, c is candidate with largest surplus
    if (number_of_equals == 1)
        c = candidates_with_surpluses[0];

    //If 2 or more equal surpluses, pass list to "equal_surpluses" which returns index of candidate with
    //"largest" surplus
    else
        c = candidates_with_surpluses[equal_surpluses(candidates_with_surpluses)];

    electeds_votes = c->get_votes_for_particular_count(c->index_of_count_candidate_was_elected_in());
    const int elected_votes_size = electeds_votes.size();
    Vote *v;
    bool done = false;
    int next_pref = 0;
    int transfer_to = 0;
    for (int i = 0; i < elected_votes_size; i++)
    {
        done = false;
        v = electeds_votes[i];
        transfer_to = v->get_transferable_to();
        next_pref = (v->get_preferences().at(transfer_to-1));
        separating_transferable_nonTransferable(next_pref, v, done);
    }

    int surplus = c->get_surplus();
    check_surplus_type(surplus, electeds_votes, next_preferences);
    index_to_distribute = check_criteria(c, next_preferences);

    //If largest surplus doesn't meet the criteria for distribution, check if sum of all surpluses does
    if (index_to_distribute == -1)
    {
        index_to_distribute = check_all_surpluses(candidates_with_surpluses, c);
    }

    return index_to_distribute;
}

//Check if sum of all surpluses meets criteria to be distributed
int STV::check_all_surpluses(const QList<Candidate *> &candidates_with_surpluses, Candidate *c)
{
    int distribute = -1;

    QVector<int> next_preferences_for_all (candidates_size); //Will hold the sub parcels of next preferences of all surpluses
    QList<Vote *> electeds_votes;
    transferableVotes.clear();

    const int surplus_candidates_size = candidates_with_surpluses.size();
    int surplus = 0;

    for (int i = 0; i < surplus_candidates_size; i++)
    {
        electeds_votes.append(candidates_with_surpluses[i]->get_votes_for_particular_count(c->index_of_count_candidate_was_elected_in()));
        surplus += candidates_with_surpluses[i]->get_surplus();
    }

    const int elected_votes_size = electeds_votes.size();
    bool done = false;
    Vote *v;
    int transfer_to = 0;
    int next_pref = 0;
    for (int i = 0; i < elected_votes_size; i++)
    {
        done = false;
        v = electeds_votes[i];
        transfer_to = v->get_transferable_to();
        next_pref = (v->get_preferences().at(transfer_to-1));
        separating_transferable_nonTransferable(next_pref, v, done);
    }

    check_surplus_type(surplus, electeds_votes, next_preferences_for_all);
    distribute = check_criteria(c, next_preferences_for_all);

    return distribute;
}

//3 checks to find "largest" of the equal surpluses
int STV::equal_surpluses(QList<Candidate *> &candidates_with_surpluses)
{
    int index_of_surplus_to_distribute = -1;
    QList<int> list;

    index_of_surplus_to_distribute = equal_surpluses_first_check(candidates_with_surpluses, list);
    if (index_of_surplus_to_distribute == -1)
    {
        index_of_surplus_to_distribute = equal_surpluses_second_check(candidates_with_surpluses, list);
        if (index_of_surplus_to_distribute == -1)
        {
            index_of_surplus_to_distribute = equal_surpluses_third_check(candidates_with_surpluses, list);
            if (index_of_surplus_to_distribute == -1)
            {
                //If 2 or more surpluses are still "equal" after checks, the "largest" is found by drawing lots
                //int index = candidates_with_surpluses[];
                index_of_surplus_to_distribute = drawing_lots(list);
            }
        }
    }
    return index_of_surplus_to_distribute;
}

//First check - if one surplus arose at any erlier count than the others it is the "largest"
int STV::equal_surpluses_first_check(QList<Candidate *> &candidates_with_surpluses, QList<int> &list)
{
    int index_of_surplus_to_distribute = -1;
    const int surplus_candidates_size  = candidates_with_surpluses.size();
    int counts_surpluses_occured_in_size = 0;
    QList<int> counts_surpluses_occured_in;
    //QList<int> earliest_surpluses_indices;

    for (int i = 0; i < surplus_candidates_size; i++)
    {
        counts_surpluses_occured_in.append(candidates_with_surpluses[i]->index_of_count_candidate_was_elected_in());
    }

    list.append(0);
    counts_surpluses_occured_in_size = counts_surpluses_occured_in.size();

    for (int i = 1; i < counts_surpluses_occured_in_size; i++)
    {
        if (counts_surpluses_occured_in[i] < counts_surpluses_occured_in[0])
        {
            list[0] = i;
        }
        else if (counts_surpluses_occured_in[i] == counts_surpluses_occured_in[0])
        {
            list.append(i);
        }
    }

    if (list.size() == 1)
    {
        index_of_surplus_to_distribute = list[0];
    }

    return index_of_surplus_to_distribute;
}

//Second check - The surplus of the candidate with the highest number of first preference votes
//is the "largest"
int STV::equal_surpluses_second_check(QList<Candidate *> &candidates_with_surpluses, QList<int> &list)
{
    int index_of_surplus_to_distribute = -1;
    QList<int> indices;
    const int surplus_candidates_size  = candidates_with_surpluses.size();
    const int list_size = list.size();

    for (int j = 0; j < surplus_candidates_size; j++)
    {
        for (int i = 0; i < candidates_size; i++)
        {
            if (candidates[i]->get_id() == candidates_with_surpluses[j]->get_id())
            {
                indices.append(i);
                break;
            }
        }
    }

    QList<int> highest_first_preferences_indices;
    highest_first_preferences_indices.append(list[0]);//->get_votes_for_particular_count(0).size());
    for (int i = 1; i < list_size; i++)
    {
        if (candidates[indices[i]]->get_votes_for_particular_count(0).size() >
                candidates[indices[0]]->get_votes_for_particular_count(0).size())
        {
            highest_first_preferences_indices[0] = list[i];//->get_votes_for_particular_count(0).size();
        }
        else if (candidates[indices[i]]->get_votes_for_particular_count(0).size() ==
                 candidates[indices[0]]->get_votes_for_particular_count(0).size())
        {
            highest_first_preferences_indices.append(list[i]);//->get_votes_for_particular_count(0).size());
        }
    }
    if (highest_first_preferences_indices.size() == 1)
    {
        index_of_surplus_to_distribute = highest_first_preferences_indices[0];
    }
    else
        list = highest_first_preferences_indices;

    return index_of_surplus_to_distribute;
}

//Third check - The surplus of the candidate with the highest number of votes at the first count at which
//the candidate's votes aren't equal is the "largest"
int STV::equal_surpluses_third_check(QList<Candidate *> &candidates_with_surpluses, QList<int> &list)
{
    int index_of_surplus_to_distribute = -1;
    QList<int> highest_at_any_count_indices;
    bool highest_found = false;
    QList<int> indices;
    const int surplus_candidates_size  = candidates_with_surpluses.size();
    const int list_size = list.size();

    for (int j = 0; j < surplus_candidates_size; j++)
    {
        for (int i = 0; i < candidates_size; i++)
        {
            if (candidates[i]->get_id() == candidates_with_surpluses[j]->get_id())
            {
                indices.append(i);
                break;
            }
        }
    }

    //Finds lowest count at which all candidates were active
    int count_num = candidates[indices[0]]->get_VotesPerCount().size();
    for (int i = 1; i < list_size; i++)
    {
        if (candidates[indices[i]]->get_VotesPerCount().size() <
                count_num)
        {
            count_num = candidates[indices[i]]->get_VotesPerCount().size();
        }
    }

    for (int i = 1; i < count_num; i++)
    {
        highest_at_any_count_indices.clear();
        highest_at_any_count_indices.append(list[0]);
        for (int j = 1; j < list.size(); j++)
        {
            if (candidates[indices[j]]->get_votes_up_to_particular_count(i).size() >
                    candidates[indices[0]]->get_votes_up_to_particular_count(i).size())
            {
                highest_at_any_count_indices[0] = list[j];
            }
            else if (candidates[indices[j]]->get_votes_up_to_particular_count(i).size() ==
                     candidates[indices[0]]->get_votes_up_to_particular_count(i).size())
            {
                highest_at_any_count_indices.append(list[j]);
            }
        }
        if (highest_at_any_count_indices.size() == 1)
        {
            highest_found = true;
            break;
        }
    }
    if (highest_found)
    {
        index_of_surplus_to_distribute = highest_at_any_count_indices[0];
    }
    else if (count_num > 1)
        list = highest_at_any_count_indices;


    return index_of_surplus_to_distribute;
}

//Check if the surplus meets criteria to be distributed
int STV::check_criteria(Candidate *c, QVector<int> &next_preferences)
{
    int surplus_to_distribute = -1;
    int index = 0;

    //Gets index of candidate
    for (int i = 0; i < candidates_size; i++)
    {
        if (candidates[i]->get_id() == c->get_id())
        {
            index = i;
            break;
        }
    }

    //3 checks to decide on distribution or not
    if (!elects_highest_continuing_candidate(next_preferences))
    {
        if (!brings_lowest_candidate_up(next_preferences))
        {
            if (!qualifies_for_expenses_recoupment(next_preferences))
            {
            }
            else
            {
                surplus_to_distribute = index;
            }
        }
        else
        {
            surplus_to_distribute = index;
        }
    }
    else
    {
        surplus_to_distribute = index;
    }

    return surplus_to_distribute;
}

//Check the surplus type i.e. transferables > surplus, transferables < surplus, transferables = surplus
void STV::check_surplus_type(int surplus, QList<Vote *> votes, QVector<int> &next_preferences)
{
    if (transferableVotes.size() > surplus)
    {
        transferables_greater_than_surplus(surplus, votes, next_preferences);
    }
    else
    {
        transferables_equal_to_or_less_than_surplus(votes, next_preferences);
    }
}

//The number of transferable ballots is greater than the surplus
void STV::transferables_greater_than_surplus(int surplus, const QList<Vote *> &votes, QVector<int> &next_preferences)
{
    QList<QList<Vote *>> amounts;
    QVector<float> amounts_with_ratio (candidates_size);
    QVector<int> amounts_without_ratio (candidates_size);

    float surplusf = surplus;
    float transferables = transferableVotes.size();
    const float ratio = surplusf/transferables;

    int total = 0;
    amounts = finding_next_valid_preference(votes);

    for (int i = 0; i < candidates_size; i++)
    {
        amounts_with_ratio[i] = amounts[i].size()*ratio; //Apply ratio to all sub-parcels
        amounts_without_ratio[i] = amounts[i].size();
    }

    //Get total of all sub-parcels
    for (int i = 0; i < candidates_size; i++)
    {
        if (candidates[i]->get_status() == 0)
            total += qRound(amounts_with_ratio[i]-0.5);
    }

    //Total is less than surplus due to fractions
    if (total != surplus)
    {
        const int diff = surplus - total;

        //Sorting the fractions
        sorting_fractions(amounts_with_ratio, amounts_without_ratio, diff);
    }

    for (int i = 0; i < candidates_size; i++)
    {
        next_preferences[i] = qRound(amounts_with_ratio[i]-0.5);
    }
}

//Sorting the fractions to ensure the total of all sub-parcels equals the surplus
void STV::sorting_fractions(QVector<float> &amounts_with_ratio, const QVector<int> &a, int diff)
{
    QList<float> final_fractions;
    QList<int> final_indices;
    QVector<float> temp = amounts_with_ratio; // Temporary list of all sub-parcels with ratio applied

    int temp_size = temp.size();
    int final_fractions_size = 0;
    int final_indices_size = 0;
    int count = 0;
    int j = 0;
    int i = 0;
    float f;
    float f1;
    float temp_float;
    bool check = false;

    // Sort temp list in descending order by size of fraction.
    for (int i = 0; i < temp_size-1; i++)
    {
        f = temp[i]-floor(temp[i]);
        for (int j = i+1; j < temp_size; j++)
        {
            f1 = temp[j]-floor(temp[j]);
            if (f1 > f)
            {
                temp_float = temp[i];
                temp[i] = temp[j];
                temp[j] = temp_float;
                f = temp[i]-floor(temp[i]);
            }
        }
    }

    //Defining the fractions to be rounded up instead of down in order to equal the surplus
    for (i = 0; i < diff && !check; i++)
    {
        final_fractions.append(temp[i]);
        count = 0;
        f = temp[i]-floor(temp[i]);
        for (j = i+1; j <= diff; j++)
        {
            f1 = temp[j]-floor(temp[j]);
            if (f1 == f)
            {
                count++;
            }
            else
                break;
        }
        if (j >= diff && count > 0)
        {
            check = true;
            final_fractions.removeAt(i);
        }
    }

    final_fractions_size = final_fractions.size();
    //Get the indices of the fractions to be rounded up
    for (int i = 0; i < final_fractions_size; i++)
    {
        for (int j = 0; j < temp_size; j++)
        {
            if (final_fractions[i] == amounts_with_ratio[j])
            {
                final_indices.append(j);
                break;
            }
        }
    }

    //There are 2 or more equal fractions that can't all be rounded up
    if (check)
    {
        sorting_equal_fractions(final_indices, amounts_with_ratio, temp, i, diff, a);
    }

    //Rounding up the required fractions
    final_indices_size = final_indices.size();
    for (int i = 0; i < final_indices_size; i++)
    {
        amounts_with_ratio[final_indices[i]] += 1.0;
    }
}

//Sorting any equal fractions to find which ones should be rounded up and which one or more shouldn't
void STV::sorting_equal_fractions(QList<int> &final_indices, QVector<float> &amounts_with_ratio, QVector<float> &temp, int k, int diff, const QVector<int> &a)
{
    QList<int> indices_of_equals;
    float f;
    float f1;
    int amounts_with_ratio_size = amounts_with_ratio.size();

    //Putting indices of the equal fractions in a list
    f = temp[k]-floor(temp[k]);
    for (int i = 0; i < amounts_with_ratio_size; i++)
    {
        f1 = amounts_with_ratio[i]-floor(amounts_with_ratio[i]);
        if (f1 == f)
            indices_of_equals.append(i);
    }

    equal_fractions_checks(final_indices, temp, diff, indices_of_equals, a);
}

//Checks to find the one or more "largest" of the equal fractions
void STV::equal_fractions_checks(QList<int> &final_indices, QVector<float> &temp, int num_needed, QList<int> &indices_of_equals, const QVector<int> &a)
{
    QList<int> largest_indices = indices_of_equals;
    QList<int> indices_after_first_check;
    int largest_indices_size = largest_indices.size();
    int temp_num = 0;

    //First fractions check - organise indices in descending order by sub parcel
    for (int i = 0; i < largest_indices_size-1; i++)
    {
        for (int j = i+1; j < largest_indices_size; j++)
        {
            if (a[largest_indices[j]] > a[largest_indices[i]])
            {
                temp_num = largest_indices[i];
                largest_indices[i] = largest_indices[j];
                largest_indices[j] = temp_num;
            }
        }
    }

    for (int i = 0; i < largest_indices_size-1 && final_indices.size() < num_needed; i++)
    {
        indices_after_first_check.append(largest_indices[i]);
        for (int j = i+1; j < largest_indices_size; j++)
        {
            if (a[largest_indices[j]] != a[largest_indices[i]])
            {
                final_indices.append(largest_indices[i]);
                break;
            }
            else
            {
                indices_after_first_check.append(largest_indices[j]);
            }
        }
    }

    //If more checks needs to be done, go to second check
    if (final_indices.size() != num_needed)
        fractions_second_check(final_indices, temp, num_needed, indices_after_first_check, a);
}

//Second fractions check - Finding indice of candidate with largest number of original votes
void STV::fractions_second_check(QList<int> &final_indices, QVector<float> &temp, int num_needed, QList<int> &indices_after_first_check, const QVector<int> &a)
{
    QList<int> indices_after_second_check;
    int indices_after_first_check_size = indices_after_first_check.size();
    int temp_num = 0;

    for (int i = 0; i < indices_after_first_check_size-1; i++)
    {
        for (int j = i+1; j < indices_after_first_check_size; j++)
        {
            if (candidates[indices_after_first_check[j]]->get_votes_for_particular_count(0).size() >
                    candidates[indices_after_first_check[i]]->get_votes_for_particular_count(0).size())
            {
                temp_num = indices_after_first_check[i];
                indices_after_first_check[i] = indices_after_first_check[j];
                indices_after_first_check[j] = temp_num;
            }
        }
    }

    for (int i = 0; i < indices_after_first_check_size-1 && final_indices.size() < num_needed; i++)
    {
        indices_after_second_check.append(indices_after_first_check[i]);
        for (int j = i+1; j < indices_after_first_check_size; j++)
        {
            if (candidates[indices_after_first_check[j]]->get_votes_for_particular_count(0).size() !=
                    candidates[indices_after_first_check[i]]->get_votes_for_particular_count(0).size())
            {
                final_indices.append(indices_after_first_check[i]);
                break;
            }
            else
            {
                indices_after_second_check.append(indices_after_first_check[j]);
            }
        }
    }

    //If more checks need to be done, go to third check
    if (final_indices.size() != num_needed)
    {
        int count_num = candidates[indices_after_second_check[0]]->get_VotesPerCount().size();

        //Gets lowest count number at which all required candidates were active
        for (int i = 0; i < indices_after_second_check.size(); i++)
        {
            if (candidates[indices_after_second_check[i]]->get_VotesPerCount().size() <
                    count_num)
            {
                count_num = candidates[indices_after_second_check[i]]->get_VotesPerCount().size();
            }
        }

        //Only enter this if the candidates were all active at a count other than first count. Otherwise there is no point as check of originals has already been done.
        if (count_num > 1)
        {
            int k=1;
            fractions_third_check(k, count_num, final_indices, temp, num_needed, indices_after_second_check, a);
        }
        else
        {
            //The fourth check is drawing lots
            while(final_indices.size() != num_needed)
            {
                final_indices.append(drawing_lots(indices_after_second_check));
            }
        }
    }
}

//Fractions third check - Find candidate with largest number of votes at any count at which the candidates' votes aren't equal.
void STV::fractions_third_check(int k, int count_num, QList<int> &final_indices, QVector<float> &temp, int num_needed, QList<int> &indices_after_second_check, const QVector<int> &a)
{
    QList<int> indices_after_third_check;
    int indices_after_second_check_size = indices_after_second_check.size();

    for (int i = 0; i < indices_after_second_check_size-1; i++)
    {
        for (int j = i+1; j < indices_after_second_check_size; j++)
        {
            if (candidates[indices_after_second_check[j]]->get_votes_up_to_particular_count(k).size() >
                    candidates[indices_after_second_check[i]]->get_votes_up_to_particular_count(k).size())
            {
                int temp = indices_after_second_check[i];
                indices_after_second_check[i] = indices_after_second_check[j];
                indices_after_second_check[j] = temp;
            }
        }
    }

    for (int i = 0; i < indices_after_second_check_size-1 && final_indices.size() < num_needed; i++)
    {
        indices_after_third_check.append(indices_after_second_check[i]);
        for (int j = i+1; j < indices_after_second_check_size; j++)
        {
            if (candidates[indices_after_second_check[j]]->get_votes_up_to_particular_count(k).size() !=
                    candidates[indices_after_second_check[i]]->get_votes_up_to_particular_count(k).size())
            {
                final_indices.append(indices_after_second_check[i]);
                break;
            }
            else
            {
                indices_after_third_check.append(indices_after_second_check[j]);
            }
        }
    }

    if (final_indices.size() != num_needed)
    {
        //Run third check recursively, moving on to the next count.
        if (k != count_num)
        {
            k++;
            fractions_third_check(k, count_num, final_indices, temp, num_needed, indices_after_third_check, a);
        }
        else
        {
            //Fourth check is drawing lots
            while(final_indices.size() != num_needed)
            {
                int index = drawing_lots(indices_after_third_check);
                final_indices.append(index);
                indices_after_third_check.removeAt(index);
            }
        }
    }
}

//Number of transferables is less than or equal to surplus, split all transferables into sub parcels by next preference
void STV::transferables_equal_to_or_less_than_surplus(const QList<Vote *> &votes, QVector<int> &next_preferences)
{
    QList<QList<Vote *>> amounts = finding_next_valid_preference(votes);

    for (int i = 0; i < candidates_size; i++)
    {
        next_preferences[i] = amounts[i].size();
    }
}

//Checks if surplus can elect highest continuing candidate
bool STV::elects_highest_continuing_candidate(const QVector<int> &next_preferences)
{
    bool check = false;
    QList<int> indices;
    int indices_size = 0;
    int index = 0;

    for (int i = 0; i < candidates_size; i++)
    {
        if (candidates[i]->get_status() == 0)
        {
            index = i;
            break;
        }
    }

    indices.append(index);
    for (int i = 0; i < candidates_size; i++)
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

    indices_size = indices.size();
    for (int i = 0; i < indices_size; i++)
    {
        if ((candidates[indices[i]]->get_total_votes().size() + next_preferences[indices[i]]) > quota)
        {
            check = true;
            break;
        }
    }

    return check;
}

//Checks if surplus can bring lowest candidate equal with or above second lowest candidate
bool STV::brings_lowest_candidate_up(const QVector<int> &next_preferences)
{
    QList<Candidate *> num_of_votes;
    Candidate *temp;
    int index_of_lowest = 0;
    int num_of_votes_size = 0;
    for (int i = 0; i < candidates_size; i++)
    {
        if (candidates[i]->get_status() == 0)
        {
            num_of_votes.append(candidates[i]);
        }
    }

    num_of_votes_size = num_of_votes.size();

    for (int i = 0; i < num_of_votes_size-1; i++)
    {
        for (int j = i+1; j < num_of_votes_size; j++)
        {
            if (num_of_votes[j]->get_total_votes().size() < num_of_votes[i]->get_total_votes().size())
            {
                temp = num_of_votes[i];
                num_of_votes[i] = num_of_votes[j];
                num_of_votes[j] = temp;
            }
        }
    }

    index_of_lowest = num_of_votes[0]->get_id()-1;
    int second_lowest = num_of_votes[1]->get_id()-1;
    if (num_of_votes[0]->get_total_votes().size()+ next_preferences[index_of_lowest] >= num_of_votes[1]->get_total_votes().size() + next_preferences[second_lowest])
    {
        return true;
    }
    else
        return false;
}

//Check if surplus can qualify a candidate for expenses recoupment i.e. reach one quarter of the quota of votes
bool STV::qualifies_for_expenses_recoupment(const QVector<int> &next_preferences)
{
    bool check = false;
    for (int i = 0; i < candidates_size; i++)
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
    return check;
}

//If surplus can be distributed, distribute it
void STV::surplus_distribution(Candidate *c, const QVector<int> &next_preferences)
{
    distributionInfo += "Distribution of candidate " + c->get_Name() + "'s surplus";

    QVector<int> temp = next_preferences;//temp stores the next preference. Allows me to keep track of how many have been transferred to each candidate.
    QVector<QList<Vote *>> lists (candidates_size);

    QList<Vote *> votes = c->get_votes_for_particular_count(c->index_of_count_candidate_was_elected_in());
    int votes_size = votes.size();
    Vote *v;
    int transfer_to = 0;

    //Transfer the votes
    for (int k = votes_size-1; k >= 0; k--)
    {
        v = votes[k];
        transfer_to = v->get_transferable_to();
        //Checks that vote is trasnferrable to a continuing candidate and that the number of votes transferred to said candidate has not exceeded what it should.
        if (transfer_to != 0 && candidates[transfer_to-1]->get_status() == 0 && temp[transfer_to-1] != 0)
        {
            v->set_route(TrackVote::add_transferred_in_surplus_route_string(candidates[transfer_to-1]));
            lists[transfer_to-1].append(v);
            votes.removeAt(k);
            temp[transfer_to-1]--;
        }
    }

    for (int i = 0; i < candidates_size; i++)
    {
        if (candidates[i]->get_status() == 0)
        {
            candidates[i]->set_votes(lists[i]);
        }
    }

    votes_size = votes.size();
    c->set_votes(votes, c->index_of_count_candidate_was_elected_in());

    //Will enter this if the no. of transferrables is less than surplus. The remainder become non transferable not effective.
    if (c->get_total_votes().size() > quota)
    {
        int difference = c->get_total_votes().size() - quota;
        for (int i = 0; i < difference; i++)
        {
            votes[i]->set_route(TrackVote::add_non_transferable_route_string(countNumber));
            nonTransferableVotesNotEffective.append(votes[i]);
            votes.removeAt(i);
        }
        c->set_votes(votes, c->index_of_count_candidate_was_elected_in());
    }

    c->set_surplusBeingDistributed(true);
    countDialog->set_votes_changes(candidates);

    check_for_elected();
    display_count_info();

    c->set_surplus(0);

    if (checked == 1)
        continue_count();
}

//Setting out the candidate(s) that is/are to be excluded
void STV::defining_candidates_for_exclusion()
{
    QList<Candidate *> candidates_ascending_by_num_of_votes = candidates;
    QList<Candidate *> exclusions;
    Candidate *highest;
    Candidate *temp;

    int ascending_candidates_size = 0;
    int total = 0;
    int amount = 0;
    int nums = 0;
    bool check = false;

    //Removes all non continuing candidates from the list
    for (int i = 0; i < candidates_ascending_by_num_of_votes.size();)
    {
        if (candidates_ascending_by_num_of_votes[i]->get_status() != 0)
        {
            candidates_ascending_by_num_of_votes.removeAt(i);
        }
        else
            i++;
    }

    //Puts list in ascending order by total number of votes
    ascending_candidates_size = candidates_ascending_by_num_of_votes.size();
    for(int i = 0; i < ascending_candidates_size-1; i++)
    {
        for (int j = i+1; j < ascending_candidates_size; j++)
        {
            if (candidates_ascending_by_num_of_votes[j]->get_total_votes().size() < candidates_ascending_by_num_of_votes[i]->get_total_votes().size())
            {
                temp = candidates_ascending_by_num_of_votes[i];
                candidates_ascending_by_num_of_votes[i] = candidates_ascending_by_num_of_votes[j];
                candidates_ascending_by_num_of_votes[j] = temp;
            }
        }
    }

    amount = get_current_surplus_total();
    total = amount;
    int highest_total = 0;

    //Finds how many candidates should be excluded together. Starts with highest candidate's votes, gets the total of all other candidates' votes and checks if said total is higher.
    //If it is, it moves down the list to the second highest candidate and repeats the process. This continues until there is a total of votes that is less than the "highest" candidate
    //they are being compared to. When this happens, all those candidates that made up the total of votes are excluded together.
    for (int i = ascending_candidates_size-1; i >= 0  && !check; i--)
    {
        total = amount;
        highest = candidates_ascending_by_num_of_votes[i];
        highest_total = highest->get_total_votes().size();
        for (nums = 0; nums < i; nums++)
        {
            total += candidates_ascending_by_num_of_votes[nums]->get_total_votes().size();
            if (total > highest_total)
                break;
        }
        if (total < highest_total)
        {
            check = true;
            for (int x = 0; x < nums && ((candidates_size-exclusions.size())>seats); x++)
            {
                    exclusions.append(candidates_ascending_by_num_of_votes[x]);
            }
        }
        else if (i == 1 && !check)
        {
            check = true;
            if (candidates_ascending_by_num_of_votes[0]->get_total_votes().size() == candidates_ascending_by_num_of_votes[1]->get_total_votes().size())
            {
                //Sort out any candidates with equal no. of votes that cannot be excluded together
                equal_lowest_candidates(candidates_ascending_by_num_of_votes, exclusions);
            }
            else
            {
                exclusions.append(candidates_ascending_by_num_of_votes[0]);
            }
        }
    }
    excluding_candidates(exclusions);
}

//3 checks to perform to find which of the equal lowest can be considered the "lowest".
void STV::equal_lowest_candidates(QList<Candidate *> &num_of_votes, QList<Candidate *> &exclusions)
{
    for (int i = 1; i < num_of_votes.size(); i++)
    {
        if (num_of_votes[i]->get_total_votes().size() != num_of_votes[0]->get_total_votes().size())
            num_of_votes.removeAt(i);
    }
    QList<Candidate *> list;
    bool check = false;

    if (!equal_lowest_first_check(check, num_of_votes, exclusions, list))
    {
        if (!equal_lowest_second_check(check, exclusions, list))
        {
            exclusions.append(drawing_lots(list));
        }
    }
}

//Equal lowest first check - Which candidate(s) had the lowest no. of original votes
bool STV::equal_lowest_first_check(bool check, const QList<Candidate *> &num_of_votes, QList<Candidate *> &exclusions, QList<Candidate *> &list)
{
    QList<Candidate *> lowest_original_votes;
    int i = 1;

    lowest_original_votes.append(num_of_votes[0]);
    while (num_of_votes.size() > i && num_of_votes[i]->get_total_votes().size() == num_of_votes[0]->get_total_votes().size())
    {
        if (num_of_votes[i]->get_votes_for_particular_count(0).size() < lowest_original_votes[0]->get_votes_for_particular_count(0).size())
        {
            lowest_original_votes[0] = num_of_votes[i];
        }
        else if (num_of_votes[i]->get_votes_for_particular_count(0).size() == lowest_original_votes[0]->get_votes_for_particular_count(0).size())
        {
            lowest_original_votes.append(num_of_votes[i]);
        }
        i++;
    }
    //If this is true then the "lowest" is found and no more checks need to be carried out
    if (lowest_original_votes.size() == 1)
    {
        exclusions.append(lowest_original_votes[0]);
        check = true;
    }
    else
        list = lowest_original_votes;

    return check;
}

//Equal lowest second check - Which candidate(s) had lowest no.of votes at first count at which they weren't equal
bool STV::equal_lowest_second_check(bool check, QList<Candidate *> &exclusions, QList<Candidate *> &list)
{
    QList<Candidate *> lowest_at_any_count_votes;
    int count_num = list[0]->get_VotesPerCount().size();
    int list_size = list.size();

    //Gets lowest count at which they were all active
    for (int i = 0; i < list_size; i++)
    {
        if (list[i]->get_VotesPerCount().size() <
                count_num)
        {
            count_num = list[i]->get_VotesPerCount().size();
        }
    }

    lowest_at_any_count_votes.append(list[0]);
    bool lowest_found = false;
    for (int i = 1; i < count_num; i++)
    {
        for (int j = 1; j < list_size; j++)
        {
            if (list[j]->get_votes_up_to_particular_count(i).size() <
                    lowest_at_any_count_votes[0]->get_votes_up_to_particular_count(i).size())
            {
                lowest_at_any_count_votes[0] = list[j];
            }
            else if (list[j]->get_votes_up_to_particular_count(i).size() ==
                     lowest_at_any_count_votes[0]->get_votes_up_to_particular_count(i).size())
            {
                lowest_at_any_count_votes.append(list[j]);
            }
        }
        //If at any point this is true, break because no more checks need to be carried out
        if (lowest_at_any_count_votes.size() == 1)
        {
            lowest_found = true;
            break;
        }
    }
    if (lowest_found)
    {
        exclusions.append(lowest_at_any_count_votes[0]);
        check = true;
    }
    else
        list = lowest_at_any_count_votes;

    return check;
}

void STV::excluding_candidates(QList<Candidate *> exclusions)
{
    distributionInfo += "Distribution of excluded candidate ";

    QVector<QList<Vote *>> lists (candidates_size);
    int exclusions_size = exclusions.size();

    for (int i = 0; i < exclusions_size; i++)
    {
        distributionInfo += exclusions[i]->get_Name() + "'s, ";
        eliminated.append(exclusions[i]);
        for (int j = 0; j < candidates_size; j++)
        {
            if (exclusions[i]->get_id() == candidates[j]->get_id())
            {
                candidates[j]->set_status(2);
                //Ensures that candidates are only excluded up until the point that there are seats left to be filled
                if (elected.size() < seats)
                {
                    distribute_excluded_votes(j, lists);
                }
                break;
            }
        }
    }

    for(int i = 0; i < candidates_size; i++)
    {
        if (candidates[i]->get_status() == 0)
        {
            candidates[i]->set_votes(lists[i]);
        }
    }
    distributionInfo.chop(2);
    distributionInfo += " votes";

    countDialog->set_votes_changes(candidates);

    //Clear the votes of the excluded candidates
    for (int i = 0; i < exclusions.size(); i++)
    {
        for (int j = 0; j < candidates_size; j++)
        {
            if (exclusions[i]->get_id() == candidates[j]->get_id())
            {
                candidates[j]->clear_votes();
            }
        }
    }
    check_for_elected();
    display_count_info();

    if (checked == 1)
        continue_count();
}

void STV::distribute_excluded_votes(int j, QVector<QList<Vote *>> &lists)
{
    textForLogFile += candidates[j]->get_Name() + " excluded\n";
    int votes_size = candidates[j]->get_total_votes().size();
    bool done = false;
    Vote *v;
    int transfer_to = 0;
    int next_pref = 0;

    //Finds the transferables
    for (int i = 0; i < votes_size; i++)
    {
        done = false;
        v = candidates[j]->get_total_votes().at(i);
        transfer_to = v->get_transferable_to();
        next_pref = (v->get_preferences().at(transfer_to-1));
        separating_transferable_nonTransferable(next_pref, v, done);
    }

    //Transfers votes by next preference. If non-transferable, it becomes non-transferable not effective.
    for (int i = 0; i < votes_size; i++)
    {
        v = candidates[j]->get_total_votes().at(i);
        transfer_to = v->get_transferable_to();
        if (transfer_to != 0 && candidates[transfer_to-1]->get_status() == 0)
        {
            lists[transfer_to-1].append(v);
            v->set_route(TrackVote::add_transferred_in_exclusion_route_string(candidates[transfer_to-1]));
        }
        else
        {
            v->set_route(TrackVote::add_non_transferable_route_string(countNumber));
            nonTransferableVotesNotEffective.append(v);

        }
    }
}

//Function for finding if a vote is transferable or not.
bool STV::separating_transferable_nonTransferable(int j, Vote *v, bool q)
{
    QList<int> list = v->get_preferences();
    bool check = false;
    for (int i = 0; i < candidates_size; i++)
    {
        if (list[i] == j)
        {
            if (candidates[i]->get_status() == 0)
            {
                check = true;
                q = true;
                v->set_transferable_to(candidates[i]->get_id());
                break;
            }
            else
            {
                //Recursive call. If a next pref is found but it is for a non-continuing candidate, start function again looking for the next next pref.
                j++;
                if (separating_transferable_nonTransferable(j, v, q))
                {
                    check = false;
                    q = true;
                    break;
                }
            }
        }
    }
    //Will only ever be true if the vote is transferable
    if (check)
    {
        transferableVotes.append(v);
        //v->set_preferences(list);
        q = true;
    }

    return q;
}

//Splits votes out into sub-parcels by next preference
QList<QList<Vote *>> STV::finding_next_valid_preference(const QList<Vote *> &votes)
{
    QList<QList<Vote *>> amounts;
    QList<Vote *> vs;
    const int votes_size = votes.size();
    int transfer_to = 0;
    Vote *v;

    for (int i =0; i < candidates_size; i++)
    {
        amounts.append(vs);
    }

    for (int x = 0; x < votes_size; x++)
    {
        v = votes[x];
        transfer_to = v->get_transferable_to();
        if (transfer_to != 0)
        {
            if (candidates[transfer_to-1]->get_status() == 0)
            {
                amounts[transfer_to-1].append(v);
            }
        }
    }
    return amounts;
}

//Gets total of all current surpluses
int STV::get_current_surplus_total()
{
    int surpluses = 0;
    int elected_size = elected.size();
    for (int i = 0; i < elected_size; i++)
    {
        surpluses += elected[i]->get_surplus();
    }
    return surpluses;
}

//The count is complete
void STV::count_complete()
{
    textForLogFile += "\nCount Complete";
    WriteToLogFile::write_to_file(textForLogFile);

    countDialog->disable_continue_button();
    countDialog->enable_final_results_button();
    countDialog->set_count_distribution_info_lbl("**Count Complete**");

    if (checked == 1)
    {
        countDialog->show();
    }
}

int STV::drawing_lots(QList<int> list)
{
    return rand()% list.size();
}

Candidate* STV::drawing_lots(QList<Candidate *> list)
{
    int random = rand()% list.size();
    return list[random];
}


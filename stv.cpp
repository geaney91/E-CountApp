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
    fileWork = f;
    fileWork->Read();
    seats = fileWork->get_Seats();
    votes = fileWork->get_Votes();
    names = fileWork->get_Candidate_Names();
    parties = fileWork->get_Candidate_Parties();
    countDialog = new CountDialog();
    connect(countDialog->get_button(), &QPushButton::clicked, this, &STV::continue_count);
}

void STV::start(QProgressDialog *pd)
{
    dialog = pd;
    validate_votes();
}

void STV::validate_votes()
{
    Validate *v = new Validate();
    valids = v->remove_invalids(votes, names);
    invalids = v->get_invalids();
    create_candidates();
}

void STV::create_candidates()
{
    for (int i = 0; i < names.size(); i++)
    {
        candidates.append(new Candidate(names[i], parties[i]));
    }
    create_valid_votes();
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
    display_static_count_info();
}

void STV::display_static_count_info()
{
    calculate_quota();
    start_count();
}

void STV::calculate_quota()
{
    quota = (valids.size()/(seats+1))+1;
}

void STV::start_count()
{
    countNumber = 1;
    distribute_votes();
}

void STV::distribute_votes()
{
    for (int i = 0; i < validVotes.size(); i++)
    {
        dialog->setValue(valids.size()+i);
        Vote *v = validVotes[i];
        //QStringList l = v->get_preferences();
        QList<QPair<int, bool>> l = v->get_preferences();
        int size = l.size();
        for (int j = 0; j < size; j++)
        {
            if (l[j].first == countNumber && !l[j].second && candidates[j]->get_status())
            {                
                candidates[j]->increment_votes(countNumber-1, v);
                v->set_route(TrackVote::add_assignment_route_string(candidates[j]));
                l[j].second = true;
                break;
            }
        }
    }
    //check_for_elected();
    display_dynamic_count_info();
}

void STV::display_dynamic_count_info()
{
    Count *count = new Count(candidates, elected, eliminated, active, validVotes, nonTransferableVotesNotEffective, countNumber);   
    countDialog->set_count_info(count);
    countDialog->set_list(validVotes);
    countDialog->set_static_count_info(votes.size(), valids.size(), invalids.size(), quota, seats);
    countDialog->show();
    dialog->setValue(100000);
    //continue_count();
}

void STV::continue_count()
{
    //countDialog->display_progress();
    if (elected.size() < seats)
    {
        //distribute_votes();
        check_for_elected();
    }
}

void STV::check_for_elected()
{
    bool check = false;
    int count = 0;
    QList<int> surpluses;
    for (int i = 0; i < candidates.size(); i++)
    {
        QList<QList<Vote *>> list = candidates[i]->getVotesPerCount();
        if (list[countNumber-1].size() >= quota)
        {
            check = true;
            elected.append(candidates[i]);
            //active.removeAt(i);
            candidates[i]->set_status(false);
            int surplus = list[countNumber-1].size() - quota;
            candidates[i]->set_surplus(surplus);
            count++;
            surpluses.append(surplus);
            //surpluses.append(candidates[i]->get_surplus());
        }
        else
        {
            surpluses.append(0);
        }
    }
    if(check)
        surplus_distribution(surpluses);
    else
        excluding_candidates();
}

void STV::surplus_distribution(const QList<int> &surpluses)
{
    //QList<int> surpluses;
    if (count == 1)
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
    }
}

void STV::check_surplus_type(const int &i)
{
    Candidate *c = candidates[i];
    int surplus = c->get_surplus();
    int size = c->getVotesPerCount().at(countNumber-1).size();
    double ratio = 0.0;
    for (int i = 0; i < size; i++)
    {
        Vote *v = c->getVotesPerCount().at(countNumber-1)[i];
        QList<QPair<int, bool>> list = v->get_preferences();
        int j = countNumber+1;
        separating_transferable_nonTransferable(j, list, c, v);
    }

    if (transferableVotes.size() > surplus)
    {
        transferables_greater_than_surplus();
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
        transferables_equal_to_surplus();
    }
    else
    {
        transferables_less_than_surplus();
    }
}

void STV::transferables_greater_than_surplus()
{

}

void STV::transferables_equal_to_surplus()
{

}

void STV::transferables_less_than_surplus()
{

}

void STV::separating_transferable_nonTransferable(int j, const QList<QPair<int, bool>> &list, Candidate *c, Vote *v)
{
    int size1 = list.size();
    bool check = false;
    for (int i = 0; i < size1; i++)
    {
        if ((list[i].first == j) && !list[i].second && c->get_status())
        {
            check = true;
            transferableVotes.append(v);
            //c->getVotesPerCount().at(countNumber-1).remove
        }
        else if ((list[i].first == j) && list[i].second && c->get_status())
        {
            j++;
            separating_transferable_nonTransferable(j, list, c, v);
        }
        else
        {
            v->set_transferable(false);
            nonTransferableVotesNotEffective.append(v);
        }
    }
}

void STV::finding_next_valid_preference(int j, const QList<QPair<int, bool>> &list, Candidate *c, Vote *v, QList<Vote *> temp)
{
    int size1 = list.size();
    bool check = false;
    for (int i = 0; i < size1; i++)
    {
        if ((list[i].first == j) && !list[i].second && c->get_status())
        {
            check = true;
            //temp[i]->increment_votes(countNumber, v);
            //candidates[i]->getVotesPerCount().at(countNumber-1).append(v);
            //c->getVotesPerCount().at(countNumber-1).remove
        }
        else if ((list[i].first == j) && list[i].second && c->get_status())
        {
            j++;
            finding_next_valid_preference(j, list, c, v, temp);
        }
        else
        {
            nonTransferableVotesNotEffective.append(v);
        }
    }
}

void STV::excluding_candidates()
{
    int lowest = 0;
    //QList<int> list = c->getVotesPerCount()[countNumber-1];
    int size = candidates.size();
    for (int i = 0; i < size; i++)
    {
        int numOfVotes = candidates[i]->getVotesPerCount()[countNumber-1].size();
        if (numOfVotes < lowest)
            lowest = numOfVotes;

    }
}

QStringList STV::get_valids()
{
    return valids;
}

QList<Vote *> STV::get_votes()
{
    return validVotes;
}


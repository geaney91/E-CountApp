#include "count.h"
#include <QStringList>
#include <QMessageBox>
#include "countdialog.h"

Count::Count()
{

}

Count::Count(FileWork *f)
{
    fileWork = f;
    fileWork->Read();
    seats = fileWork->get_Seats();
    votes = fileWork->get_Votes();
    names = fileWork->get_Candidate_Names();
    parties = fileWork->get_Candidate_Parties();
    c = new CountDialog();
}

void Count::start()
{
    validate_votes();
    create_candidates();
    create_valid_votes();
    display_count_info();
}

void Count::validate_votes()
{
    Validate *v = new Validate();
    valids = v->remove_invalids(votes, names);
    invalids = v->get_invalids();
}

void Count::create_candidates()
{
    for (int i = 0; i < names.size(); i++)
    {
        candidates.append(new Candidate(names[i], parties[i]));
    }
    //c->set_list(candidates);
}

void Count::create_valid_votes()
{
    for (int i = 0; i < valids.size(); i++)
    {
        QString l = valids[i];
        QStringList prefs = l.split(",");
        validVotes.append(new Vote(i+1, "", prefs));
    }
    c->set_list(validVotes);
}

void Count::display_count_info()
{
    c->show();
    calculate_quota();
    c->set_count_info(votes.size(), valids.size(), invalids.size(), quota, seats);
    //c->set_list(invalids);
    start_count();
}

void Count::calculate_quota()
{
    quota = (valids.size()/(seats+1))+1;
}

void Count::start_count()
{
    for (int i = 0; i < validVotes.size(); i++)
    {
        Vote *v = validVotes[i];
        QStringList l = v->get_preferences();
        for (int j = 0; j < l.size(); j++)
        {
            if (l[j] == "1")
            {
                candidates[j]->votesPerCount[0]++;
                break;
            }
        }
    }
    //c->set_list(candidates);
}

QStringList Count::get_valids()
{
    return valids;
}

QList<Vote *> Count::get_votes()
{
    return validVotes;
}


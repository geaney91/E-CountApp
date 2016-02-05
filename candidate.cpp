#include "candidate.h"
#include <QMainWindow>

Candidate::Candidate()
{

}

Candidate::Candidate(QString n, QString p)
{
    name = n;
    party = p;
    surplus = 0;
    active = true;
    //votesPerCount.append(votes);
}

void Candidate::increment_votes(/*int countNum, */Vote *v)
{
    //votesPerCount[countNum].append(v);
    votes.append(v);
}

/*QList<QList<Vote *>> Candidate::getVotesPerCount()
{
    return votesPerCount;
}*/

QList<Vote *> Candidate::getVotes()
{
    return votes;
}

QString Candidate::get_Name()
{
    return name;
}

QString Candidate::get_Party()
{
    return party;
}

int Candidate::get_surplus()
{
    return surplus;
}

void Candidate::set_surplus(int s)
{
    surplus = s;
}

bool Candidate::get_status()
{
    return active;
}

void Candidate::set_status(bool set)
{
    active = set;
}



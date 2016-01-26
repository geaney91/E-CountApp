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
    votesPerCount.append(0);
}

void Candidate::increment_votes(int num)
{
    votesPerCount[num]++;
}

QList<int> Candidate::getVotesPerCount()
{
    return votesPerCount;
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



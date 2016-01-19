#include "candidate.h"
#include <QMainWindow>

Candidate::Candidate()
{

}

Candidate::Candidate(QString n, QString p)
{
    name = n;
    party = p;
    votesPerCount.append(0);
}

QList<int> Candidate::getList()
{
    return votesPerCount;
}

QString Candidate::getName()
{
    return name;
}

QString Candidate::getParty()
{
    return party;
}
#include "candidate.h"
#include <QMainWindow>

Candidate::Candidate()
{

}

Candidate::Candidate(int id, QString n, QString p)
{
    this->id = id;
    name = n;
    party = p;
    surplus = 0;
    status = 0;
    surplusBeingDistributed = false;
    //votesPerCount.append(votes);
    //votesPerCount.append(new QList<Vote *>);
}

void Candidate::increment_votes(int countNum, /*int countNum, */Vote *v)
{
    votesPerCount[countNum].append(v);
    //votes.append(v);
}

void Candidate::set_votes(QList<Vote *> votes)
{
    this->votes = votes;
    votesPerCount.append(this->votes);
}

void Candidate::set_votes(QList<Vote *> votes, int index)
{
    this->votes = votes;
    votesPerCount[index] = this->votes;
}

QList<QList<Vote *>> Candidate::get_VotesPerCount()
{
    return votesPerCount;
}

QList<Vote *> Candidate::get_votes_for_particular_count(int countNum)
{
    return votesPerCount[countNum];
}

QList<Vote *> Candidate::get_total_votes()
{
    QList<Vote *> votes;
    for (int i  = 0; i < votesPerCount.size(); i++)
    {
        votes.append(votesPerCount[i]);
    }
    return votes;
}

/*QList<Vote *> Candidate::getVotes()
{
    return votes;
}*/

int Candidate::get_id()
{
    return id;
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

bool Candidate::get_surplusBeingDistributed()
{
    return surplusBeingDistributed;
}

void Candidate::set_surplusBeingDistributed(bool d)
{
    surplusBeingDistributed = d;
}

int Candidate::get_status()
{
    return status;
}

void Candidate::set_status(int set)
{
    status = set;
}

void Candidate::clear_votes()
{
    for (int i = 0; i < votesPerCount.size(); i++)
    {
        votesPerCount[i].clear();
    }
}

int Candidate::index_of_count_candidate_was_elected_in()
{
    return votesPerCount.size()-1;
}


void Candidate::remove_vote()
{
    votesPerCount[index_of_count_candidate_was_elected_in()].removeLast();
}

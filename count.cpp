#include "count.h"
#include "candidate.h"
#include "vote.h"

Count::Count()
{

}

Count::Count(QList<Candidate *> c, QList<Candidate *> elec, QList<Candidate *> elim, QList<Candidate *> act, QList<Vote *> v,
             QList<Vote *> ntv, int countNum)
{
    candidates = c;
    elected = elec;
    eliminated = elim;
    active = act;
    validVotes = v;
    nonTransferableVotes = ntv;
    countNumber = countNum;
}

QList<Candidate *> Count::get_candidates()
{
    return candidates;
}

QList<Candidate *> Count::get_elected()
{
    return elected;
}

QList<Candidate *> Count::get_eliminated()
{
    return eliminated;
}

QList<Candidate *> Count::get_active()
{
    return active;
}

QList<Vote *> Count::get_validVotes()
{
    return validVotes;
}

QList<Vote *> Count::get_nonTransferableVotes()
{
    return nonTransferableVotes;
}

int Count::get_countNumber()
{
    return countNumber;
}

#include "count.h"
#include "candidate.h"
#include "vote.h"

Count::Count()
{

}

Count::Count(QList<Candidate *> c, QList<Candidate *> elec, QList<Candidate *> excl, QList<Vote *> v,
             QList<Vote *> ntv, int countNum, QString distribInfo)
{
    candidates = c;
    elected = elec;
    excluded = excl;
    validVotes = v;
    nonTransferableVotesNotEffective = ntv;
    countNumber = countNum;
    distributionInfo = distribInfo;
}

QList<Candidate *> Count::get_candidates()
{
    return candidates;
}

QList<Candidate *> Count::get_elected()
{
    return elected;
}

QList<Candidate *> Count::get_excluded()
{
    return excluded;
}

QList<Vote *> Count::get_validVotes()
{
    return validVotes;
}

QList<Vote *> Count::get_nonTransferable_votes_not_effective()
{
    return nonTransferableVotesNotEffective;
}

int Count::get_countNumber()
{
    return countNumber;
}

QString Count::get_distribution_info()
{
    return distributionInfo;
}

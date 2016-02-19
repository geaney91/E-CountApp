#ifndef COUNT_H
#define COUNT_H

#include <QMainWindow>
#include "candidate.h"
#include "vote.h"

class Count
{
public:
    Count();
    Count(QList<Candidate *> c, QList<Candidate *> elec, QList<Candidate *> excl, QList<Vote *> v,
          QList<Vote *> ntv, int countNum, QString distribInfo);

    QList<Candidate *> get_candidates();
    QList<Candidate *> get_elected();
    QList<Candidate *> get_excluded();
    QList<Candidate *> get_active();
    QList<Vote *> get_validVotes();
    QList<Vote *> get_nonTransferable_votes_not_effective();
    int get_countNumber();
    QString get_distribution_info();

private:
    QList<Candidate *> candidates;
    QList<Vote *> validVotes;
    QList<Vote *> nonTransferableVotesNotEffective;
    QList<Candidate *> elected;
    QList<Candidate *> excluded;

    int countNumber;
    int seats;
    int quota;

    QString distributionInfo;

signals:

public slots:
};

#endif // COUNT_H

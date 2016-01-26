#ifndef COUNT_H
#define COUNT_H

#include <QMainWindow>
#include "candidate.h"
#include "vote.h"

class Count
{
public:
    Count();
    Count(QList<Candidate *> c, QList<Candidate *> elec, QList<Candidate *> elim, QList<Candidate *> active, QList<Vote *> v,
          QList<Vote *> ntv, int countNum);

    QList<Candidate *> get_candidates();
    QList<Candidate *> get_elected();
    QList<Candidate *> get_eliminated();
    QList<Candidate *> get_active();
    QList<Vote *> get_validVotes();
    QList<Vote *> get_nonTransferableVotes();
    int get_countNumber();

private:
    QList<Candidate *> candidates;
    QList<Vote *> validVotes;
    QList<Vote *> nonTransferableVotes;
    QList<Candidate *> elected;
    QList<Candidate *> eliminated;
    QList<Candidate *> active;

    int countNumber;
    int seats;
    int quota;

signals:

public slots:
};

#endif // COUNT_H

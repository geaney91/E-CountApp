#ifndef CANDIDATE_H
#define CANDIDATE_H

#include <QMainWindow>
#include "vote.h"

class Candidate
{
public:
    Candidate();
    Candidate(QString n, QString p);
    QList<QList<Vote *>> getVotesPerCount();
    QString get_Name();
    QString get_Party();
    int get_surplus();
    void set_surplus(int s);
    bool get_status();
    void set_status(bool set);
    void increment_votes(int countNum, Vote *v);


private:
    QList<QList<Vote *>> votesPerCount;
    QList<Vote *> votes;
    QString name;
    QString party;
    int surplus;
    bool active;

signals:

public slots:
};

#endif // CANDIDATE_H

#ifndef CANDIDATE_H
#define CANDIDATE_H

#include <QMainWindow>
#include "vote.h"

class Candidate
{
public:
    Candidate();
    Candidate(int id, QString n, QString p);
    QList<QList<Vote *>> get_VotesPerCount();
    QList<Vote *> get_votes_for_particular_count(int countNum);
    QList<Vote *> get_total_votes();
    //QList<Vote *> getVotes();
    QString get_Name();
    int get_id();
    QString get_Party();
    int get_surplus();
    void set_surplus(int s);
    bool get_status();
    void set_status(int set);
    void increment_votes(int countNum, Vote *v);
    void set_votes(QList<Vote *> votes);
    void clear_votes();
    int index_of_count_candidate_was_elected_in();

private:
    QList<QList<Vote *>> votesPerCount;
    QList<Vote *> votes;
    int id;
    QString name;
    QString party;
    int surplus;
    int status;

signals:

public slots:
};

#endif // CANDIDATE_H

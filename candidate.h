#ifndef CANDIDATE_H
#define CANDIDATE_H

#include "vote.h"

class Candidate
{
public:
    Candidate();
    Candidate(int id, QString n, QString p);

    QList<QList<Vote *>> get_VotesPerCount();
    QList<Vote *> get_votes_for_particular_count(int countNum);
    QList<Vote *> get_votes_up_to_particular_count(int countNum);
    QList<Vote *> get_total_votes();

    QString get_Name();
    int get_id();
    QString get_Party();
    int get_surplus();
    bool get_surplusBeingDistributed();
    int get_status();

    void set_surplus(int s);
    void set_surplusBeingDistributed(bool d);
    void set_status(int set);
    void set_votes(QList<Vote *> votes);
    void set_votes(QList<Vote *> votes, int index);

    void increment_votes(int countNum, Vote *v);
    void clear_votes();
    int index_of_count_candidate_was_elected_in();
    void remove_vote();

private:
    QList<QList<Vote *>> votesPerCount;
    QList<Vote *> votes;
    int id;
    QString name;
    QString party;
    int surplus;
    int status;
    bool surplusBeingDistributed;

signals:

public slots:
};

#endif // CANDIDATE_H

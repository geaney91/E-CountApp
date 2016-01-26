#ifndef CANDIDATE_H
#define CANDIDATE_H

#include <QMainWindow>

class Candidate
{
public:
    Candidate();
    Candidate(QString n, QString p);
    QList<int> getVotesPerCount();
    QString get_Name();
    QString get_Party();
    int get_surplus();
    void set_surplus(int s);
    void increment_votes(int num);


private:
    QList<int> votesPerCount;
    QString name;
    QString party;
    int surplus;

signals:

public slots:
};

#endif // CANDIDATE_H

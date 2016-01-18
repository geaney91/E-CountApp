#ifndef CANDIDATE_H
#define CANDIDATE_H

#include <QMainWindow>

class Candidate
{
public:
    Candidate();
    Candidate(QString n, QString p);
    QList<int> getList();
    QString getName();
    QString getParty();

    QList<int> votesPerCount;

private:
    QString name;
    QString party;

signals:

public slots:
};

#endif // CANDIDATE_H

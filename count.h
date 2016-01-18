#ifndef COUNT_H
#define COUNT_H

#include <QMainWindow>
#include "mainwindow.h"
#include "candidate.h"
#include "vote.h"
#include "validate.h"
#include "countdialog.h"

class Count
{
public:
    Count(FileWork *f);

    void create_candidates();
    void create_votes();
    void create_valid_votes();
    void validate_votes();
    void start();
    void display_count_info();
    void calculate_quota();
    void start_count();

    QStringList get_valids();

private:
    //MainWindow *mw;
    CountDialog *c;
    FileWork *fileWork;

    QList<Candidate *> candidates;
    QList<Vote *> validVotes;
    //QList<Vote> invalidVotes;
    QList<Candidate *> elected;
    QList<Candidate *> eliminated;
    QList<Candidate *> active;

    int seats;
    int quota;
    QStringList votes;
    QStringList valids;
    QStringList invalids;
    QStringList names;
    QStringList parties;


signals:

public slots:
};

#endif // COUNT_H

#ifndef STV_H
#define STV_H

#include <QMainWindow>
#include "mainwindow.h"
#include "candidate.h"
#include "vote.h"
#include "validate.h"
#include "countdialog.h"
#include <QSplashScreen>
#include <QThread>
#include <QProgressDialog>

class STV : public QObject
{
public:
    STV();
    STV(FileWork *f);
    //STV get_instance();
    void start(QProgressDialog *pd);

    QStringList get_valids();
    QList<Vote *> get_votes();

private:
    //MainWindow *mw;
    CountDialog *countDialog;
    FileWork *fileWork;
    QProgressDialog *dialog;

    QList<Candidate *> candidates;
    QList<Vote *> validVotes;
    QList<Vote *> nonTransferableVotes;
    QList<Candidate *> elected;
    QList<Candidate *> eliminated;
    QList<Candidate *> active;

    void display_static_count_info();
    void calculate_quota();
    void start_count();
    void check_for_elected();

    int seats;
    int quota;
    int countNumber;
    QStringList votes;
    QStringList valids;
    QStringList invalids;
    QStringList names;
    QStringList parties;


signals:

public slots:
    void validate_votes();
    void create_candidates();
    void create_valid_votes();
    void display_dynamic_count_info();
    void distribute_votes();
    void continue_count();
    void surplus_distribution(int count);
    void check_surplus_type(Candidate *c);
    void excluding_candidates();
};

#endif // STV_H

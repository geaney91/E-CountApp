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
    QList<Vote *> nonTransferableVotesNotEffective;
    QList<Vote *> transferableVotes;
    QList<Candidate *> elected;
    QList<Candidate *> eliminated;
    QList<Candidate *> active;
    QList<Count *> counts;

    void display_static_count_info();
    void calculate_quota();
    void start_count();
    void check_for_elected();
    void separating_transferable_nonTransferable(int j, const QList<QPair<int, bool>> &list, Candidate *c, Vote *v);
    void transferables_greater_than_surplus();
    void transferables_equal_to_surplus();
    void transferables_less_than_surplus();
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
    void display_count_info();
    void distribute_votes();
    void continue_count();
    void surplus_distribution(const QList<int> &surpluses, int count);
    void check_surplus_type(const int &i);
    void finding_next_valid_preference(int j, const QList<QPair<int, bool> > &list, Candidate *c, Vote *v, QList<Vote *> temp);
    void excluding_candidates();
};

#endif // STV_H

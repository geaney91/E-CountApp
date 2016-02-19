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
    void validate_votes();
    void create_candidates();
    void create_valid_votes();
    void calculate_quota();

    void start_count();
    void distribute_first_preferences();
    void check_for_elected();
    void display_count_info();


    bool check_if_meets_criteria(const int &count);
    bool elects_highest_continuing_candidate(const QVector<int> &next_preferences);
    bool brings_lowest_candidate_up(const QVector<int> &next_preferences);
    bool qualifies_for_expenses_recoupment(const QVector<int> &next_preferences);

    void surplus_distribution();
    void check_surplus_type(Candidate *c, QVector<int> &next_preferences);
    void check_surplus_type(const int &i);
    void separating_transferable_nonTransferable(int j, Vote *v);
    void transferables_greater_than_surplus(Candidate *c, QVector<int> &next_preferences);
    QVector<int> transferables_equal_to_surplus();
    QVector<int> transferables_less_than_surplus();
    QVector<int> finding_next_valid_preference(int j, QList<Vote *> votes);
    int distributing_by_next_valid_preference(int j, Vote *v, const int &distribution_type);
    void defining_candidates_for_exclusion();
    void excluding_candidates(QList<Candidate *> exclusions);
    void distribute_excluded_votes(const int &j);

    int get_current_surplus_total();
    int get_total_candidate_votes(int i);
    void clear_candidates_votes();

    void count_complete();

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

    int seats;
    int quota;
    int expenses_recoupment_point;
    int countNumber;
    QStringList votes;
    QStringList valids;
    QStringList invalids;
    QStringList names;
    QStringList parties;

    QString distributionInfo;
    int size;

signals:

public slots:
    void continue_count();
};

#endif // STV_H

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
    bool check_if_should_continue();

    int check_if_meets_criteria(const int &count, QVector<int> &next_preferences);
    void one_surplus(QList<Vote *> &electeds_votes, int &surplus, Candidate *c);
    int more_than_one_surplus(QList<Candidate *> candidates_with_surpluses, Candidate *c, QVector<int> &next_preferences);
    int equal_surpluses(QList<Candidate *> &candidates_with_surpluses);
    int check_criteria(Candidate *c, QVector<int> &next_preferences);
    bool elects_highest_continuing_candidate(const QVector<int> &next_preferences);
    bool brings_lowest_candidate_up(const QVector<int> &next_preferences);
    bool qualifies_for_expenses_recoupment(const QVector<int> &next_preferences);

    void surplus_distribution(Candidate *c, QVector<int> &next_preferences);
    void check_surplus_type(const int &surplus, QList<Vote *> electeds_votes, QVector<int> &next_preferences);
    bool separating_transferable_nonTransferable(int j, Vote *v, bool q);
    void transferables_greater_than_surplus(const int &surplus, QList<Vote *> votes, QVector<int> &next_preferences);
    void sorting_out_fractions(QVector<float> &amounts_with_ratio, const QVector<int> &a, QVector<int> &a1, const int &total, const int &surplus);
    void two_fractions();
    void more_than_2_fractions();
    void transferables_equal_to_or_less_than_surplus(const int &surplus, QList<Vote *> votes, QVector<int> &next_preferences);
    //void transferables_less_than_surplus(const int &surplus, QList<Vote *> votes, QVector<int> &next_preferences);
    QList<QList<Vote *> > finding_next_valid_preference(int j, QList<Vote *> votes);
    int distributing_by_next_valid_preference(int j, Vote *v, const int &distribution_type);
    void defining_candidates_for_exclusion();
    void equal_lowest_candidates(const int &equal, QList<Candidate *> &exclusions);
    bool should_candidate_be_excluded();
    void excluding_candidates(QList<Candidate *> exclusions);
    void distribute_excluded_votes(const int &j, QVector<QList<Vote *> > &lists);

    int get_current_surplus_total();
    int get_total_candidate_votes(int i);
    void clear_candidates_votes();

    void count_complete();

    void drawing_lots();
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

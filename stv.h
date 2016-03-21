#ifndef STV_H
#define STV_H

#include <QMainWindow>
#include "candidate.h"
#include "vote.h"
#include "validate.h"
#include "countdialog.h"
#include "filework.h"
#include <QSplashScreen>
#include <QThread>
#include <QProgressDialog>
#include <QMessageBox>
//#include <QObject>

class STV : public QObject
{
    Q_OBJECT

public:
    STV();
    STV(FileWork *f, int checked);
    void add_info(FileWork *f, int checked);

    void start(/*QProgressDialog *pd*/);
    void validate_votes();
    void create_candidates();
    void create_valid_votes();
    void calculate_quota();

    void display_count_info();
    void start_count();
    void distribute_first_preferences();
    void check_for_elected();
    bool check_if_should_continue();

    int check_if_meets_criteria(const int &count, QVector<int> &next_preferences);
    int more_than_one_surplus(QList<Candidate *> candidates_with_surpluses, Candidate *c, QVector<int> &next_preferences);
    int equal_surpluses(QList<Candidate *> &candidates_with_surpluses);
    int check_criteria(Candidate *c, QVector<int> &next_preferences);
    bool elects_highest_continuing_candidate(const QVector<int> &next_preferences);
    bool brings_lowest_candidate_up(const QVector<int> &next_preferences);
    bool qualifies_for_expenses_recoupment(const QVector<int> &next_preferences);

    void surplus_distribution(Candidate *c, const QVector<int> &next_preferences);
    void check_surplus_type(const int &surplus, QList<Vote *> electeds_votes, QVector<int> &next_preferences);
    void transferables_greater_than_surplus(const int &surplus, QList<Vote *> votes, QVector<int> &next_preferences);
    void sorting_out_fractions(QVector<float> &amounts_with_ratio, const QVector<int> &a, const int &total, const int &surplus);
    void total_greater_than_surplus(const QList<int> &equal_fractions, int lowest, QVector<float> &amounts_with_ratio, const QVector<int> &a);
    void total_less_than_surplus(const QList<int> &indices, int highest, QVector<float> &amounts_with_ratio, const QVector<int> &a);
    void sorting_equal_fractions(int type, int &t, const QList<int> &equal_fractions,  const QVector<int> &a);
    void more_than_2_fractions();
    void transferables_equal_to_or_less_than_surplus(QList<Vote *> votes, QVector<int> &next_preferences);

    bool separating_transferable_nonTransferable(int j, Vote *v, bool q);
    QList<QList<Vote *> > finding_next_valid_preference(int j, QList<Vote *> votes);
    int distributing_by_next_valid_preference(int j, Vote *v, const int &distribution_type);

    void defining_candidates_for_exclusion();
    void equal_lowest_candidates(const QList<Candidate *> &num_of_votes, QList<Candidate *> &exclusions);
    bool should_candidate_be_excluded();
    void excluding_candidates(QList<Candidate *> exclusions);
    void distribute_excluded_votes(const int &j, QVector<QList<Vote *> > &lists);

    int get_current_surplus_total();
    int get_total_candidate_votes(int i);
    void clear_candidates_votes();

    void count_complete();

    int drawing_lots(QList<int> list);
    Candidate *drawing_lots(QList<Candidate *> list);
    QStringList get_valids();
    QList<Vote *> get_votes();

private:
    //MainWindow mw;
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
    QString textForLogFile;
    int size;
    int checked;
    QMessageBox mb;

signals:
    //void start(/*QProgressDialog *pd*/);
    //void display_count_info();

public slots:
    void continue_count();
};

#endif // STV_H

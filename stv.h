#ifndef STV_H
#define STV_H

#include "candidate.h"
#include "vote.h"
#include "countdialog.h"
#include "filework.h"

class STV : public QObject
{
    Q_OBJECT

public:
    STV();
    void add_info(FileWork *f, int checked);

    void start();
    void validate_votes();
    void create_candidates();
    void create_valid_votes();
    void calculate_quota();

    void display_count_info();
    void start_count();
    void distribute_first_preferences();
    void check_for_elected();
    bool check_if_should_continue();

    int check_if_meets_criteria(int count, QVector<int> &next_preferences);
    int more_than_one_surplus(QVector<int> &next_preferences);
    int check_all_surpluses(Candidate *c);
    int equal_surpluses();
    int equal_surpluses_first_check(QList<int> &list);
    int equal_surpluses_second_check(QList<int> &list);
    int equal_surpluses_third_check(QList<int> &list);

    int check_criteria(Candidate *c, QVector<int> &next_preferences);
    bool elects_highest_continuing_candidate(const QVector<int> &next_preferences);
    bool brings_lowest_candidate_up(const QVector<int> &next_preferences);
    bool qualifies_for_expenses_recoupment(const QVector<int> &next_preferences);

    void check_surplus_type(int surplus, QList<Vote *> electeds_votes, QVector<int> &next_preferences);
    void transferables_greater_than_surplus(int surplus, const QList<Vote *> &votes, QVector<int> &next_preferences);
    void transferables_equal_to_or_less_than_surplus(const QList<Vote *> &votes, QVector<int> &next_preferences);

    void surplus_distribution(Candidate *c, const QVector<int> &next_preferences);

    void sorting_fractions(QVector<float> &amounts_with_ratio, const QVector<int> &a, int diff);
    void sorting_equal_fractions(QVector<float> &amounts_with_ratio, QVector<float> &temp, int k, int diff, const QVector<int> &a);
    void equal_fractions_checks(QVector<float> &temp, int num_needed, QList<int> &indices_of_equals, const QVector<int> &a);
    void fractions_second_check(QVector<float> &temp, int num_needed, QList<int> &indices_after_first_check, const QVector<int> &a);
    void fractions_third_check(int k, int count_num, QVector<float> &temp, int num_needed, QList<int> &indices_of_equals, const QVector<int> &a);

    bool separating_transferable_nonTransferable(int j, Vote *v, bool q);
    QList<QList<Vote *> > finding_next_valid_preference(const QList<Vote *> &votes);

    void defining_candidates_for_exclusion();

    void equal_lowest_candidates(QList<Candidate *> &num_of_votes);
    bool equal_lowest_first_check(bool check, const QList<Candidate *> &num_of_votes, QList<Candidate *> &list);
    bool equal_lowest_second_check(bool check, QList<Candidate *> &list);

    void excluding_candidates();
    void distribute_excluded_votes(int j, QVector<QList<Vote *> > &lists);

    int get_current_surplus_total();

    int drawing_lots(QList<int> list);
    Candidate *drawing_lots(QList<Candidate *> list);

    void count_complete();

private:

    CountDialog *countDialog;
    FileWork *fileWork;

    QList<Candidate *> candidates;
    QList<Vote *> validVotes;
    QList<Vote *> nonTransferableVotesNotEffective;
    QList<Vote *> transferableVotes;
    QList<Candidate *> elected;
    QList<Candidate *> eliminated;
    QList<Count *> counts;

    QList<Candidate *> exclusions;
    QList<int> final_indices;
    QList<Candidate *> candidates_with_surpluses;

    int seats;
    int quota;
    int expenses_recoupment_point;
    int countNumber;
    int candidates_size;
    int checked;

    QStringList votes;
    QStringList valids;
    QStringList invalids;
    QStringList names;
    QStringList parties;

    QString distributionInfo;
    QString textForLogFile;

signals:

public slots:
    void continue_count();
};

#endif // STV_H

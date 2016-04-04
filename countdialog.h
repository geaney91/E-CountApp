#ifndef COUNTDIALOG_H
#define COUNTDIALOG_H

#include <QDialog>
#include <QListWidget>
//#include <QProgressDialog>
#include "candidate.h"
#include "vote.h"
#include "count.h"
#include "resultsform.h"

namespace Ui {
class CountDialog;
}

class CountDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CountDialog(QWidget *parent = 0);
    ~CountDialog();
    void sync_scrollbars();
    void set_count_distribution_info_lbl(QString text);

    void display_non_transferable_votes();
    void set_static_count_info(int total, int valid, int invalid, int quota, int seats);
    void set_count_info();
    void set_votes_changes(const QList<Candidate *> &temp);
    void add_info_to_lists(QList<Candidate *> c, QListWidget *lw);
    void set_non_transferable_not_effectives(QList<Vote *> v);
    void set_distribution_info();
    void disable_continue_button();
    QPushButton *get_button();

    void populate_combo_box();
    void enable_final_results_button();
    void add_to_results_form();

    void results_form_static_info(int total, int valid, int invalid, int quota, int seats);
    void results_form_set_votes_changes();
    void results_form_set_form();
    void set_count_object(Count *c);

private slots:
    void on_votes_list_itemActivated(QListWidgetItem *item);
    void on_search_by_cand_combo_currentIndexChanged(int index);
    void on_search_by_vote_id_rb_clicked();
    void on_search_by_cand_rb_clicked();
    void on_search_by_vote_id_btn_clicked();
    void on_final_results_btn_clicked();

public slots:
    void reset_ui();

private:
    Ui::CountDialog *ui;
    ResultsForm *rf;
    Count *count;
    QList<Count *> counts;
    QStringList changes;
};

#endif // COUNTDIALOG_H

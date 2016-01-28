#ifndef COUNTDIALOG_H
#define COUNTDIALOG_H

#include <QDialog>
#include "candidate.h"
#include "vote.h"
#include "count.h"
#include <QListWidget>
#include <QProgressDialog>

namespace Ui {
class CountDialog;
}

class CountDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CountDialog(QWidget *parent = 0);
    ~CountDialog();
    void set_list(QStringList v);
    void set_list(QList<Candidate *> c);
    void set_list(QList<Vote *> votes);
    void set_static_count_info(int total, int valid, int invalid, int quota, int seats);
    void set_count_info(Count *count);
    void add_info_to_lists(QList<Candidate *> c, QListWidget *lw);
    void set_candidates(QList<Candidate *> c);
    void disable_continue_button();
    QPushButton *get_button();
    void display_progress();

//signals:
//    void on_pushButton_clicked();

private slots:
    void on_votes_list_itemActivated(QListWidgetItem *item);
    //void on_pushButton_clicked();


private:
    Ui::CountDialog *ui;
    //QProgressDialog *progressDialog;
};

#endif // COUNTDIALOG_H

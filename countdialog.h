#ifndef COUNTDIALOG_H
#define COUNTDIALOG_H

#include <QDialog>
#include "candidate.h"
#include "vote.h"
#include <QListWidget>

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
    void set_count_info(int total, int valid, int invalid, int quota, int seats);

private slots:
    void on_votes_list_itemActivated(QListWidgetItem *item);

    void on_pushButton_clicked();

private:
    Ui::CountDialog *ui;
    QVariant *q;
};

#endif // COUNTDIALOG_H

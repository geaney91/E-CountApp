#include "countdialog.h"
#include "ui_countdialog.h"
#include "count.h"

CountDialog::CountDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CountDialog)
{
    ui->setupUi(this);
}

CountDialog::~CountDialog()
{
    delete ui;
}

void CountDialog::set_list(QStringList v)
{
    for (int i = 0; i < v.size(); i++)
    {
        QString l = v[i];
        new QListWidgetItem(l, ui->excluded_list);
    }
}

void CountDialog::set_list(QList<Candidate *> c)
{
    for (int i = 0; i < c.size(); i++)
    {
        Candidate *l = c[i];
        //QString name = l->getName();
        //new QListWidgetItem(name, ui->excluded_list);
        QString name = QString::number(l->votesPerCount[0]);
        new QListWidgetItem(name, ui->excluded_list);
    }
}

void CountDialog::set_list(QList<Vote *> votes)
{
    //QVariant qv;
    const int size = votes.size();
    for (int i = 0; i < size; i++)
    {
        Vote *v = votes[i];
        new QListWidgetItem(QString::number(v->get_id()), ui->votes_list);
    }
}


void CountDialog::set_count_info(int total, int valid, int invalid, int quota, int seats)
{
    ui->total_poll_lbl2->setText(QString::number(total));
    ui->valid_poll_lbl2->setText(QString::number(valid));
    ui->invalid_ballots_lbl2->setText(QString::number(invalid));
    ui->quota_lbl2->setText(QString::number(quota));
    ui->no_of_seats_lbl2->setText(QString::number(seats));
}

void CountDialog::on_votes_list_itemActivated(QListWidgetItem *item)
{
    Count *c = new Count();
    QList<Vote *> vs = c->get_votes();
    for (int i = 0; i < vs.size(); i++)
    {
        if (item->text() == QString::number(vs[i]->get_id()))
        {
            ui->label_4->setText(vs[i]->get_route());
            break;
        }
    }
}

void CountDialog::on_pushButton_clicked()
{

}


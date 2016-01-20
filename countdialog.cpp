#include "countdialog.h"
#include "ui_countdialog.h"
#include "count.h"
#include "votelistitem.h"

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

        //QString::number(v->get_id()), ui->votes_list
        //QListWidgetItem* item;// = new QListWidgetItem();
        //item->setData(Qt::UserRole, QVariant::fromValue(votes[i]));
        //item->setText(QString::number(votes[i]->get_id()));
        VoteListItem *item = new VoteListItem(v);
        item->setText(QString::number(v->get_id()));
        ui->votes_list->addItem(item);
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

void CountDialog::on_votes_list_itemActivated(QListWidgetItem* item)
{
    VoteListItem *v1 = static_cast<VoteListItem*>(item);
    ui->label_4->setText(v1->getRoute());
    /*Count *c = new Count();
    QList<Vote *> vs = c->get_votes();
    for (int i = 0; i < vs.size(); i++)
    {
        if (item->text() == QString::number(vs[i]->get_id()))
        {
            ui->label_4->setText(vs[i]->get_route());
            break;
        }
    }*/

    //QObject* obj = qvariant_cast<QObject*>(item->data(Qt::UserRole));
    //Vote* vote = qobject_cast<Vote*>(obj);
    //Vote* vote = item->data(Qt::UserRole).value<Vote*>();
    //ui->label_4->setText(vote->get_route());
}

void CountDialog::on_pushButton_clicked()
{

}


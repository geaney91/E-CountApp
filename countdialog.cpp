#include "countdialog.h"
#include "ui_countdialog.h"

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
        QString name = l->getName();
        new QListWidgetItem(name, ui->excluded_list);
    }
}

void CountDialog::set_list(QList<Vote *> votes)
{
    //QVariant qv;
    for (int i = 0; i < votes.size(); i++)
    {
        Vote *v = votes[i];
        QString id = QString::number(v->get_id());
        new QListWidgetItem(id, ui->votes_list);
        //QListWidgetItem *item = new QListWidgetItem();
        //item->setText(id);
        //QVariant q = QVariant::fromValue(v);
        //item->setData(Qt::UserRole, q);
        //ui->votes_list->addItem(item);
        //qv.setValue(v);
        //QListWidgetItem *item = new QListWidgetItem();
        //item->setText(QString::number(v->get_id()));
        //item->setData(Qt::UserRole, qv);
        //ui->votes_list->addItem(item);
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

}

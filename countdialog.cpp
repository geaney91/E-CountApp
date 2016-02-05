#include "countdialog.h"
#include "ui_countdialog.h"
#include "stv.h"
#include "votelistitem.h"
#include <QProgressDialog>

CountDialog::CountDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CountDialog)
{
    ui->setupUi(this);
    //progressDialog = new QProgressDialog("Counting...", "Abort", 0, INT_MAX, this);
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
        //QString name = QString::number(l->getVotesPerCount()[i].size());
        QString name = QString::number(l->getVotes().size());
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

void CountDialog::set_static_count_info(int total, int valid, int invalid, int quota, int seats)
{
    ui->total_poll_lbl2->setText(QString::number(total));
    ui->valid_poll_lbl2->setText(QString::number(valid));
    ui->invalid_ballots_lbl2->setText(QString::number(invalid));
    ui->quota_lbl2->setText(QString::number(quota));
    ui->no_of_seats_lbl2->setText(QString::number(seats));
}

void CountDialog::set_count_info(Count *count)
{
    ui->results_lbl->setText("Results after Count " + QString::number(count->get_countNumber()));

    QList<Candidate *> temp = count->get_elected();
    add_info_to_lists(count->get_elected(), ui->elected_list);

    temp = count->get_eliminated();
    add_info_to_lists(temp, ui->excluded_list);

    temp = count->get_candidates();
    add_info_to_lists(temp, ui->candidates_list);

    int number_of_votes = 0;
    for (int i = 0; i < temp.size(); i++)
    {
        //number_of_votes = temp[i]->getVotesPerCount()[count->get_countNumber()-1].size();
        number_of_votes = temp[i]->getVotes().size();
        new QListWidgetItem(QString::number(number_of_votes), ui->votes_count_list);
    }
}

void CountDialog::add_info_to_lists(QList<Candidate *> temp, QListWidget *lw)
{
    int size = temp.size();
    QString name = "";
    for (int i = 0; i < size; i++)
    {
        name = temp[i]->get_Name();
        new QListWidgetItem(name, lw);
    }
}

void CountDialog::set_candidates(QList<Candidate *> c)
{

}

void CountDialog::disable_continue_button()
{
    ui->continue_btn->setEnabled(false);
}

void CountDialog::on_votes_list_itemActivated(QListWidgetItem* item)
{
    VoteListItem *v1 = static_cast<VoteListItem*>(item);
    ui->label_4->setText(v1->getRoute());
}

/*void CountDialog::on_pushButton_clicked()
{
    //STV::continue_count();
    STV *stv;
    stv->get_instance();
    stv->continue_count();
}*/

void CountDialog::display_progress()
{
    ui->continue_btn->setText("It works!");
}

QPushButton* CountDialog::get_button()
{
    return ui->continue_btn;
}

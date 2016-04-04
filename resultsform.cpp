#include "resultsform.h"
#include "ui_resultsform.h"
#include <QTextEdit>

ResultsForm::ResultsForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResultsForm)
{
    ui->setupUi(this);
    keep_count = 0;
    ui->results_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint);
}

ResultsForm::~ResultsForm()
{
    delete ui;
}

ResultsForm::ResultsForm(QList<Count *> counts)
{
    this->counts = counts;
}

void ResultsForm::set_static_info(int total, int valid, int invalid, int quota, int seats)
{
    ui->total_poll_lbl2->setText(QString::number(total));
    ui->valid_poll_lbl2->setText(QString::number(valid));
    ui->invalid_ballots_lbl2->setText(QString::number(invalid));
    ui->quota_lbl2->setText(QString::number(quota));
    ui->no_of_seats_lbl2->setText(QString::number(seats));
    //set_table_rows();
}

/*void ResultsForm::set_table_rows()
{
    Count *c = counts.at(0);
    ui->results_table->insertColumn(0);
    for (int i = 0; i < c->get_candidates().size(); i++)
    {
        ui->results_table->insertRow(i);
    }
}*/

QStringList ResultsForm::get_list()
{
    return headers;
}

void ResultsForm::set_up_form(Count *c)
{
    keep_count++;
    this->count = c;
    if (keep_count == c->get_countNumber())
    {
        if (c->get_countNumber() == 1)
        {
            ui->results_table->setRowCount(c->get_candidates().size()+3);
            ui->results_table->insertColumn(0);
            headers<<"Candidate";
            ui->results_table->setHorizontalHeaderLabels(headers);
            ui->results_table->setItem(0, 0, new QTableWidgetItem(""));
            for (int i = 1; i <= c->get_candidates().size(); i++)
            {
                ui->results_table->setItem(i, 0, new QTableWidgetItem(c->get_candidates().at(i-1)->get_Name()));
                changes.append("");
                //QCoreApplication::processEvents();
            }
            ui->results_table->setItem(c->get_candidates().size()+2, 0, new QTableWidgetItem("Non-Transferable Not Effective"));
        }
        add_info_to_form();
    }
    set_elected();
}

void ResultsForm::add_info_to_form()
{
    bool check = false;
    int row = 1;
    int column = count->get_countNumber();
    ui->results_table->insertColumn(column);
    ui->results_table->setRowHeight(0, 100);
    for (int j = 0; j < count->get_candidates().size(); j++)
    {
        Candidate *cand = count->get_candidates().at(j);
        ui->results_table->setItem(row, column,
                                   new QTableWidgetItem(QString::number(cand->get_total_votes().size()) +
                                                        "\n" + changes[j]));
        row++;
        //QCoreApplication::processEvents();
    }
    ui->results_table->setItem(row+1, column,
                               new QTableWidgetItem(QString::number(count->get_nonTransferable_votes_not_effective().size())));

    ui->results_table->setItem(0, column, new QTableWidgetItem(count->get_distribution_info()));
    QTextEdit *edit = new QTextEdit();
    edit->setReadOnly(true);
    edit->setText(count->get_distribution_info());
    ui->results_table->setCellWidget(0, column, edit);
    //ui->results_table->setItem(0, column, new QTableWidgetItem(count->get_distribution_info()));
    update_headers();
    if (headers.contains("4"))
        check = true;
}

void ResultsForm::update_headers()
{
    headers<<"Count " + QString::number(count->get_countNumber());
    ui->results_table->setHorizontalHeaderLabels(headers);
}

void ResultsForm::set_changes(QStringList changes)
{
    this->changes = changes;
    //ui->results_table->it
}

void ResultsForm::set_elected()
{
    ui->elected_list_4->clear();
    for (int i = 0; i < count->get_elected().size(); i++)
    {
        QString string = count->get_elected().at(i)->get_Name();
        new QListWidgetItem(string, ui->elected_list_4);
    }
}

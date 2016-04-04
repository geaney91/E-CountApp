#include "countdialog.h"
#include "ui_countdialog.h"
#include "votelistitem.h"
//#include "resultsform.h"
#include <QProgressDialog>
#include <QMessageBox>
#include <QScrollBar>

CountDialog::CountDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CountDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
    ui->progressBar->hide();
    rf = new ResultsForm();
    //if (checked == 0)
    //    connect(ui->continue_btn, &QPushButton::clicked, stv, &STV::continue_count);
    ui->search_by_vote_id_lbl->setVisible(false);
    ui->search_by_vote_id_spinbox->setVisible(false);
    ui->search_by_vote_id_btn->setVisible(false);
    ui->search_by_vote_id_spinbox->setRange(1, 10000000);
    ui->final_results_btn->setEnabled(false);
    sync_scrollbars();
}

CountDialog::~CountDialog()
{
    delete ui;
}

//Sync's the scrollbars so when one moves the others move as well
void CountDialog::sync_scrollbars()
{
    int current_slider_value = 0;
    QScrollBar *scroll_1 = ui->candidates_list->verticalScrollBar();
    QScrollBar *scroll_2 = ui->votes_count_list->verticalScrollBar();
    QScrollBar *scroll_3 = ui->vote_changes_list->verticalScrollBar();
    connect(scroll_1, &QAbstractSlider::valueChanged,
            [=](int aSliderPosition) mutable {if (aSliderPosition != current_slider_value)
                                        {
                                            scroll_2->setValue(aSliderPosition);
                                            scroll_3->setValue(aSliderPosition);
                                            current_slider_value = aSliderPosition;
                                        }
        });
    connect(scroll_2, &QAbstractSlider::valueChanged,
            [=](int aSliderPosition) mutable {if (aSliderPosition != current_slider_value)
                                        {
                                            scroll_1->setValue(aSliderPosition);
                                            scroll_3->setValue(aSliderPosition);
                                            current_slider_value = aSliderPosition;
                                        }
        });
    connect(scroll_3, &QAbstractSlider::valueChanged,
            [=](int aSliderPosition) mutable {if (aSliderPosition != current_slider_value)
                                        {
                                            scroll_1->setValue(aSliderPosition);
                                            scroll_2->setValue(aSliderPosition);
                                            current_slider_value = aSliderPosition;
                                        }
        });
}

void CountDialog::set_count_distribution_info_lbl(QString text)
{
    ui->count_distirubution_info_lbl->setText(text);
}

void CountDialog::display_non_transferable_votes()
{
    QList<Vote *> nonTs = count->get_nonTransferable_votes_not_effective();
    int nonTs_size = nonTs.size();
    Vote *v;
    VoteListItem *item;
    for (int i = 0; i < nonTs_size; i++)
    {
        v = nonTs[i];
        item = new VoteListItem(v);
        item->setText(QString::number(v->get_id()));
        ui->votes_list->addItem(item);
    }
}

//Sets the static count info that doesn't change ove rthe course of the count
void CountDialog::set_static_count_info(int total, int valid, int invalid, int quota, int seats)
{
    ui->total_poll_lbl2->setText(QString::number(total));
    ui->valid_poll_lbl2->setText(QString::number(valid));
    ui->invalid_ballots_lbl2->setText(QString::number(invalid));
    ui->quota_lbl2->setText(QString::number(quota));
    ui->no_of_seats_lbl2->setText(QString::number(seats));

    results_form_static_info(total, valid, invalid, quota, seats);
}

void CountDialog::set_count_object(Count *c)
{
    count = c;
}

//Displays elected, excluded, candidates, candidate's votes.
void CountDialog::set_count_info()
{
    ui->results_lbl->setText("Results after Count " + QString::number(count->get_countNumber()));

    QList<Candidate *> temp = count->get_elected();
    add_info_to_lists(count->get_elected(), ui->elected_list);

    temp = count->get_excluded();
    add_info_to_lists(temp, ui->excluded_list);

    temp = count->get_candidates();
    add_info_to_lists(temp, ui->candidates_list);

    int number_of_votes = 0;
    int temp_size = temp.size();
    for (int i = 0; i < temp_size; i++)
    {
        number_of_votes = temp[i]->get_total_votes().size();
        new QListWidgetItem(QString::number(number_of_votes), ui->votes_count_list);
    }

    set_non_transferable_not_effectives(count->get_nonTransferable_votes_not_effective());
    set_distribution_info();
    results_form_set_form();
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

//Sets the vote changes each time a surplus is distributed or candidate(s) is/are excluded
void CountDialog::set_votes_changes(const QList<Candidate *> &temp)
{
    QString string = "";
    QStringList changes1;
    int number_of_votes = 0;
    int temp_size = temp.size();
    for (int i = 0; i < temp_size; i++)
    {
        //If candidate is continuing
        if (temp[i]->get_status() == 0)
        {
            number_of_votes = temp[i]->get_votes_for_particular_count(temp[i]->index_of_count_candidate_was_elected_in()).size();
            string = "+" + QString::number(number_of_votes);
            new QListWidgetItem(string, ui->vote_changes_list);
        }
        //If candidate is elected
        else if (temp[i]->get_status() == 1)
        {
            if (temp[i]->get_surplusBeingDistributed() == true)
            {
                number_of_votes = temp[i]->get_surplus();
                string = "-" + QString::number(number_of_votes);
                new QListWidgetItem(string, ui->vote_changes_list);
            }
            else
            {
                string = "0";
                new QListWidgetItem(string, ui->vote_changes_list);
            }
        }
        //If candidate is excluded
        else
        {
            number_of_votes = temp[i]->get_total_votes().size();
            string = "-" + QString::number(number_of_votes);
            new QListWidgetItem(string, ui->vote_changes_list);
        }
        changes1.append(string);
    }
    changes = changes1;
    results_form_set_votes_changes();
}

void CountDialog::set_non_transferable_not_effectives(QList<Vote *> v)
{
    ui->votesNotEffectiveLbl2->setText(QString::number(v.size()));
}

void CountDialog::set_distribution_info()
{
    ui->count_distirubution_info_lbl->setText(count->get_distribution_info());
}

void CountDialog::disable_continue_button()
{
    ui->continue_btn->setText("Count Complete!");
    ui->continue_btn->setEnabled(false);
}

//Displays the vote route when one is selected from the votes list
void CountDialog::on_votes_list_itemActivated(QListWidgetItem* item)
{
    VoteListItem *v1 = static_cast<VoteListItem*>(item);
    ui->textEdit->setText(v1->getRoute());
}

QPushButton* CountDialog::get_button()
{
    return ui->continue_btn;
}

//Clears the ui
void CountDialog::reset_ui()
{
    ui->candidates_list->clear();
    ui->votes_list->clear();
    ui->votes_count_list->clear();
    ui->elected_list->clear();
    ui->excluded_list->clear();
    ui->vote_changes_list->clear();
    ui->votes_list->clear();
    ui->search_by_cand_combo->setCurrentIndex(0);
}

//Populates the "search by candidate" combo box
void CountDialog::populate_combo_box()
{
    ui->search_by_cand_combo->setCurrentIndex(0);
    ui->search_by_cand_combo->addItem("");
    int size = count->get_candidates().size();
    for (int i = 0; i < size; i++)
    {
        ui->search_by_cand_combo->addItem(count->get_candidates().at(i)->get_Name());
    }
    QString non_t = "non-transferables";
    ui->search_by_cand_combo->addItem(non_t);
}

//Displays the votes of the candidate selected from the combo box
void CountDialog::on_search_by_cand_combo_currentIndexChanged(int index)
{
    ui->votes_list->clear();
    int size = count->get_candidates().size();
    QList<Vote *> votes;
    int votes_size = 0;
    QCoreApplication::processEvents();
    //It's a candidate
    if (index > 0 && index <= size)
    {
        votes = count->get_candidates().at(index-1)->get_total_votes();
        votes_size = votes.size();
        Vote *v;
        VoteListItem *item;
        for (int j = votes_size-1; j >= 0; j--)
        {
            v = votes[j];
            item = new VoteListItem(v);
            item->setText(QString::number(v->get_id()));
            ui->votes_list->addItem(item);
        }
    }
    //It's the non-transferable not effectives
    else if (index == size+1)
        display_non_transferable_votes();
    //It should be blank
    else
    {
        ui->votes_list->clear();
    }

}

void CountDialog::on_search_by_vote_id_rb_clicked()
{
    ui->search_by_cand_combo->setVisible(false);
    ui->search_by_cand_lbl->setVisible(false);
    ui->search_by_cand_rb->setChecked(false);

    ui->search_by_vote_id_lbl->setVisible(true);
    ui->search_by_vote_id_spinbox->setVisible(true);
    ui->search_by_vote_id_btn->setVisible(true);
}

void CountDialog::on_search_by_cand_rb_clicked()
{
    ui->search_by_vote_id_lbl->setVisible(false);
    ui->search_by_vote_id_spinbox->setVisible(false);
    ui->search_by_vote_id_btn->setVisible(false);
    ui->search_by_vote_id_rb->setChecked(false);

    ui->search_by_cand_combo->setVisible(true);
    ui->search_by_cand_lbl->setVisible(true);
}

//Finds the vote by id chosen form the list
void CountDialog::on_search_by_vote_id_btn_clicked()
{
    bool vote_found = false;
    QString id_string = ui->search_by_vote_id_spinbox->text();
    if (id_string != "" && id_string != "0")
    {
        int id = id_string.toInt();
        int size = count->get_candidates().size();
        QList<Vote *> votes;
        int votes_size = 0;
        for (int y = 0; y < size; y++)
        {
            votes = count->get_candidates().at(y)->get_total_votes();
            votes_size = votes.size();
            Vote *v;
            VoteListItem *item;
            for (int j = votes_size-1; j >= 0; j--)
            {
                v = votes[j];
                if (v->get_id() == id)
                {
                    vote_found = true;
                    ui->votes_list->clear();
                    item = new VoteListItem(v);
                    item->setText(QString::number(v->get_id()));
                    ui->votes_list->addItem(item);
                }
            }
        }

        QList<Vote *> nonTs = count->get_nonTransferable_votes_not_effective();
        int nonTs_size = nonTs.size();
        for (int i = 0; i < nonTs_size; i++)
        {
            Vote *v = nonTs[i];
            if (v->get_id() == id)
            {
                vote_found = true;
                ui->votes_list->clear();
                VoteListItem *item = new VoteListItem(v);
                item->setText(QString::number(v->get_id()));
                ui->votes_list->addItem(item);
            }
        }
        if (ui->votes_list->count() == 0)
        {
            vote_found = false;
        }
    }
    //If vote doesn't exist, display message box telling the user.
    if (!vote_found)
    {
        QMessageBox box;
        box.setText("Invalid vote id");
        box.exec();
    }

}

void CountDialog::results_form_static_info(int total, int valid, int invalid, int quota, int seats)
{
    rf->set_static_info(total, valid, invalid, quota, seats);
}

void CountDialog::results_form_set_votes_changes()
{
    rf->set_changes(changes);
}

void CountDialog::results_form_set_form()
{
    rf->set_up_form(count);
}

void CountDialog::add_to_results_form()
{

}

void CountDialog::enable_final_results_button()
{
    ui->final_results_btn->setEnabled(true);
}

void CountDialog::on_final_results_btn_clicked()
{
    QStringList list = rf->get_list();
    rf->setGeometry(400, 200, 1000, 700);
    rf->show();
}


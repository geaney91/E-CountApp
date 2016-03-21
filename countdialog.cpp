#include "countdialog.h"
#include "ui_countdialog.h"
#include "stv.h"
#include "votelistitem.h"
#include "resultsform.h"
#include <QProgressDialog>
#include <QScrollBar>

CountDialog::CountDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CountDialog)
{
    ui->setupUi(this);
    //if (checked == 0)
        //connect(ui->continue_btn, &QPushButton::clicked, this, &STV::continue_count);
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

void CountDialog::set_list()
{
    int size = count->get_candidates().size();
    for (int y = 0; y < size; y++)
    {
        QList<Vote *> votes = count->get_candidates().at(y)->get_total_votes();
        for (int j = votes.size()-1; j >= 0; j--)
        {
            //dialog->setValue(j);
            Vote *v = votes[j];
            VoteListItem *item = new VoteListItem(v);
            item->setText(QString::number(v->get_id()));
            ui->votes_list->addItem(item);
        }
    }
    display_non_transferable_votes();
    //dialog->setValue(100000);
}

void CountDialog::display_non_transferable_votes()
{
    QList<Vote *> nonTs = count->get_nonTransferable_votes_not_effective();
    int nonTs_size = nonTs.size();
    for (int i = 0; i < nonTs_size; i++)
    {
        Vote *v = nonTs[i];
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

void CountDialog::set_count_info(Count *c)
{
    //counts.append(c);
    count = c;
    ui->results_lbl->setText("Results after Count " + QString::number(c->get_countNumber()));

    QList<Candidate *> temp = c->get_elected();
    add_info_to_lists(c->get_elected(), ui->elected_list);

    temp = c->get_excluded();
    add_info_to_lists(temp, ui->excluded_list);

    temp = c->get_candidates();
    add_info_to_lists(temp, ui->candidates_list);

    int number_of_votes = 0;
    for (int i = 0; i < temp.size(); i++)
    {
        //number_of_votes = temp[i]->getVotesPerCount()[count->get_countNumber()-1].size();
            number_of_votes = temp[i]->get_total_votes().size();//get_total_candidate_votes(i);
            new QListWidgetItem(QString::number(number_of_votes), ui->votes_count_list);
    }
    if (c->get_countNumber() > 1)
    {
        //set_votes_changes(temp);
    }
    //set_list();
    set_non_transferable_not_effectives(c->get_nonTransferable_votes_not_effective());
    set_distribution_info();
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

void CountDialog::set_votes_changes(const QList<Candidate *> temp)
{
    QString string = "";
    int number_of_votes = 0;
    //QList<Candidate *> temp = count->get_candidates();
    for (int i = 0; i < temp.size(); i++)
    {
        if (temp[i]->get_status() == 0)
        {
            number_of_votes = temp[i]->get_votes_for_particular_count(temp[i]->index_of_count_candidate_was_elected_in()).size();
            string = "+" + QString::number(number_of_votes);
            new QListWidgetItem(string, ui->vote_changes_list);
        }
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
                //number_of_votes = temp[i]->get_votes_for_particular_count(temp[i]->index_of_count_candidate_was_elected_in()).size();
                string = "+0";
                new QListWidgetItem(string, ui->vote_changes_list);
            }
        }
        else
        {
            number_of_votes = temp[i]->get_total_votes().size();
            string = "-" + QString::number(number_of_votes);
            new QListWidgetItem(string, ui->vote_changes_list);
        }
    }
}

void CountDialog::set_candidates(QList<Candidate *> c)
{

}

QList<Vote *> CountDialog::get_total_candidate_vote_objects(int j)
{
    QList<Vote *> votes = count->get_candidates().at(j)->get_total_votes();
    return votes;
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

void CountDialog::on_votes_list_itemActivated(QListWidgetItem* item)
{
    VoteListItem *v1 = static_cast<VoteListItem*>(item);
    //ui->vote_route_details_lbl->setText(v1->getRoute());
    ui->textEdit->setText(v1->getRoute());
}

void CountDialog::display_progress()
{
    ui->continue_btn->setText("It works!");
}

QPushButton* CountDialog::get_button()
{
    return ui->continue_btn;
}

void CountDialog::reset_ui()
{
    ui->candidates_list->clear();
    ui->votes_list->clear();
    ui->votes_count_list->clear();
    ui->elected_list->clear();
    ui->excluded_list->clear();
    ui->vote_changes_list->clear();
    ui->votes_list->clear();
}

void CountDialog::populate_combo_box()
{
    ui->search_by_cand_combo->clear();
    //ui->search_by_cand_combo->addItem("");
    for (int i = 0; i < count->get_candidates().size(); i++)
    {
        ui->search_by_cand_combo->addItem(count->get_candidates().at(i)->get_Name());
    }
    QString non_t = "non-transferables";
    ui->search_by_cand_combo->addItem(non_t);
}

void CountDialog::on_search_by_cand_combo_currentIndexChanged(int index)
{
    ui->votes_list->clear();
    if (index >= 0 && index < count->get_candidates().size())
    {
        QList<Vote *> votes = count->get_candidates().at(index)->get_total_votes();
        for (int j = votes.size()-1; j >= 0; j--)
        {
            //dialog->setValue(j);
            Vote *v = votes[j];
            VoteListItem *item = new VoteListItem(v);
            item->setText(QString::number(v->get_id()));
            ui->votes_list->addItem(item);
        }
    }
    else if (index == count->get_candidates().size())
        display_non_transferable_votes();
    else
    {}
    //dialog->setValue(100000);
}

void CountDialog::on_search_by_vote_id_rb_clicked()
{
    ui->search_by_cand_combo->setVisible(false);
    ui->search_by_cand_lbl->setVisible(false);
    ui->search_by_cand_rb->setChecked(false);

    ui->search_by_vote_id_lbl->setVisible(true);
    ui->search_by_vote_id_spinbox->setVisible(true);
    ui->search_by_vote_id_btn->setVisible(true);
    //ui->search_by_vote_id_rb->setChecked(false);
}

void CountDialog::on_search_by_cand_rb_clicked()
{
    ui->search_by_vote_id_lbl->setVisible(false);
    ui->search_by_vote_id_spinbox->setVisible(false);
    ui->search_by_vote_id_btn->setVisible(false);
    ui->search_by_vote_id_rb->setChecked(false);

    ui->search_by_cand_combo->setVisible(true);
    ui->search_by_cand_lbl->setVisible(true);
    //ui->search_by_cand_rb->setChecked(false);
}

void CountDialog::on_search_by_vote_id_btn_clicked()
{
    bool vote_found = false;
    QString id_string = ui->search_by_vote_id_spinbox->text();
    if (id_string != "" && id_string != "0")
    {
        int id = id_string.toInt();
        int size = count->get_candidates().size();
        for (int y = 0; y < size; y++)
        {
            QList<Vote *> votes = count->get_candidates().at(y)->get_total_votes();
            for (int j = votes.size()-1; j >= 0; j--)
            {
                //dialog->setValue(j);
                Vote *v = votes[j];
                if (v->get_id() == id)
                {
                    vote_found = true;
                    ui->votes_list->clear();
                    VoteListItem *item = new VoteListItem(v);
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
    if (!vote_found)
    {
        QMessageBox box;
        box.setText("Invalid vote id");
        box.exec();
    }

}

void CountDialog::enable_final_results_button()
{
    ui->final_results_btn->setEnabled(true);
}

void CountDialog::on_final_results_btn_clicked()
{
    ResultsForm *rf = new ResultsForm();

}

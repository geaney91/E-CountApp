#include "stv.h"
#include <QStringList>
#include <QMessageBox>
#include "countdialog.h"
#include <QSplashScreen>
#include "trackvote.h"
#include <QProgressDialog>
#include <QProgressBar>
#include <QPushButton>

STV::STV()
{

}

STV::STV(FileWork *f)
{
    fileWork = f;
    fileWork->Read();
    seats = fileWork->get_Seats();
    votes = fileWork->get_Votes();
    names = fileWork->get_Candidate_Names();
    parties = fileWork->get_Candidate_Parties();
    countDialog = new CountDialog();
    connect(countDialog->get_button(), &QPushButton::clicked, this, &STV::continue_count);
}

/*STV STV::get_instance()
{
    //return this;
}*/

void STV::start(QProgressDialog *pd)
{
    dialog = pd;
    validate_votes();

    //dialog.setValue(dialog.value() + 1);
    //dialog.close();
    //QPixmap pix(":/background2.png");
    //QSplashScreen *splash = new QSplashScreen(pix);
    //splash->show();
    //splash->showMessage("Validating Votes", Qt::AlignCenter, Qt::blue);
    //splash->showMessage("Creating Candidates", Qt::AlignCenter, Qt::blue);  
    //splash->showMessage("Creating Votes", Qt::AlignCenter, Qt::blue);
}

void STV::validate_votes()
{
    Validate *v = new Validate();
    valids = v->remove_invalids(votes, names);
    invalids = v->get_invalids();
    create_candidates();
    /*QThread* thread = new QThread;
    thread->start();
    thread->wait();
    this->moveToThread(thread);
    connect(thread, SIGNAL (started()), this, SLOT (create_valid_votes()));
    connect(this, SIGNAL (finished()), thread, SLOT (quit()));
    connect(this, SIGNAL (finished()), this, SLOT (deleteLater()));
    connect(thread, SIGNAL (finished()), thread, SLOT (deleteLater()));
    thread->start();*/
}

void STV::create_candidates()
{
    for (int i = 0; i < names.size(); i++)
    {
        candidates.append(new Candidate(names[i], parties[i]));
    }
    create_valid_votes();
}

void STV::create_valid_votes()
{
    dialog->setLabelText("Creating vote objects....");
    for (int i = 0; i < valids.size(); i++)
    {
        dialog->setValue(i);
        QString l = valids[i];
        QStringList prefs = l.split(",");
        QList<QPair<int, bool>> preferences;
        foreach (QString pref, prefs)
        {
            preferences.append(qMakePair(pref.toInt(), false));
        }

        validVotes.append(new Vote(i+1, "", preferences));
    }
    dialog->setValue(valids.size());
    countDialog->set_list(validVotes);
    display_static_count_info();
}

void STV::display_static_count_info()
{
    calculate_quota();
    countDialog->set_static_count_info(votes.size(), valids.size(), invalids.size(), quota, seats);
    start_count();
}

void STV::calculate_quota()
{
    quota = (valids.size()/(seats+1))+1;
}

void STV::start_count()
{
    countNumber = 1;
    distribute_votes();
}

void STV::distribute_votes()
{
    //dialog->setValue(51);
    int count = valids.size();
    dialog->setLabelText("First Count...");
    for (int i = 0; i < validVotes.size(); i++)
    {
        dialog->setValue(count+i);
        Vote *v = validVotes[i];
        //QStringList l = v->get_preferences();
        QList<QPair<int, bool>> l = v->get_preferences();
        int size = l.size();
        for (int j = 0; j < size; j++)
        {
            if (l[j].first == countNumber && !l[j].second)
            {
                candidates[j]->increment_votes(countNumber-1, v);
                v->set_route(TrackVote::add_assignment_route_string(candidates[j]));
                l[j].second = true;
                break;
            }
        }

    }
    dialog->setValue(valids.size()+ validVotes.size());
    display_dynamic_count_info();
}

void STV::display_dynamic_count_info()
{
    Count *count = new Count(candidates, elected, eliminated, active, validVotes, nonTransferableVotes, countNumber);
    countDialog->set_count_info(count);
    dialog->setValue(1000000);
    countDialog->show();
    //continue_count();
}

void STV::continue_count()
{
    //countDialog->display_progress();
    check_for_elected();
}

void STV::check_for_elected()
{
    bool check = false;
    int count = 0;
    //QList<int> surpluses;
    for (int i = 0; i < candidates.size(); i++)
    {
        QList<QList<Vote *>> list = candidates[i]->getVotesPerCount();
        if (list[countNumber-1].size() >= quota)
        {
            check = true;
            elected.append(candidates[i]);
            //active.removeAt(i);
            candidates[i]->set_status(false);
            int surplus = list[countNumber-1].size() - quota;
            candidates[i]->set_surplus(surplus);
            count++;
            //surpluses.append(surplus);
            //surpluses.append(candidates[i]->get_surplus());
        }
        else{}
            //surpluses.append(-1);
    }
    if(check)
        surplus_distribution(count/*surpluses*/);
    else
        excluding_candidates();
}

void STV::surplus_distribution(int count/*QList<int> surpluses*/)
{
    //QList<int> surpluses;
    if (count == 1)
    {
        for (int i = 0; i < candidates.size(); i++)
        {
            if (candidates[i]->get_surplus() != 0)
            {
                check_surplus_type(candidates[i]);
                break;
            }
        }
    }
    else if (count > 1)
    {
        int surplus = 0;
        int total_surplus = 0;
        for (int i = 0; i < candidates.size(); i++)
        {
            if (candidates[i]->get_surplus() != 0)
            {
                surplus = candidates[i]->get_surplus();
            }
        }
    }

}

void STV::check_surplus_type(Candidate *c)
{
    int surplus = c->get_surplus();
    int size = c->getVotesPerCount().at(countNumber-1).size();
    for (int i = 0; i < size; i++)
    {
        bool check = false;
        Vote *v = c->getVotesPerCount().at(countNumber-1)[i];
        //QStringList l = v->get_preferences();
        QList<QPair<int, bool>> list = v->get_preferences();
        int size1 = list.size();
        /*for (int j = 0; j < size1; j++)
        {
            if (list[j] == countNumber+1)// && candidates[j]->get_status())
            {
                check = true;
            }
        }
        if (!check)
        {

        }*/
    }
}

void STV::excluding_candidates()
{
    /*int lowest = 0;
    QList<int> list = c->getVotesPerCount()[countNumber-1];
    int size = candidates.size();
    for (int i = 0; i < size; i++)
    {
        int numOfVotes = c->getVotesPerCount()[countNumber-1].size();
        if (numOfVotes < lowest)
            lowest = numOfVotes;

    }*/
}

QStringList STV::get_valids()
{
    return valids;
}

QList<Vote *> STV::get_votes()
{
    return validVotes;
}


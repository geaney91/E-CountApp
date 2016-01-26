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
}

/*STV STV::get_instance()
{
    return this;
}*/

void STV::start()
{
    validate_votes();
    create_candidates();
    create_valid_votes();
    display_static_count_info();
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
}

void STV::create_valid_votes()
{
    for (int i = 0; i < valids.size(); i++)
    {
        QString l = valids[i];
        QStringList prefs = l.split(",");
        validVotes.append(new Vote(i+1, "", prefs));
    }
    countDialog->set_list(validVotes);
    //emit finished();
}

void STV::run()
{
    //create_valid_votes();
}

void STV::display_static_count_info()
{
    countDialog->show();
    //splash->close();
    calculate_quota();
    countDialog->set_static_count_info(votes.size(), valids.size(), invalids.size(), quota, seats);
    //c->set_list(invalids);
    start_count();
}

void STV::calculate_quota()
{
    quota = (valids.size()/(seats+1))+1;
}

void STV::start_count()
{
    countNumber = 1;
    for (int i = 0; i < validVotes.size(); i++)
    {
        Vote *v = validVotes[i];
        QStringList l = v->get_preferences();
        for (int j = 0; j < l.size(); j++)
        {
            if (l[j] == "1")
            {
                candidates[j]->increment_votes(countNumber-1);
                v->set_route(TrackVote::add_assignment_route_string(candidates[j]));
                break;
            }
        }

    }
    active = candidates;

    while (elected.size() < seats)
    {
        display_dynamic_count_info();

    }
    //connect(countDialog->get_button(), SIGNAL (clicked()), this, SLOT (continue_count()));
}

void STV::display_dynamic_count_info()
{
    Count *count = new Count(candidates, elected, eliminated, active, validVotes, nonTransferableVotes, countNumber);
    countDialog->set_count_info(count);
    continue_count();
}

void STV::continue_count()
{
    check_for_elected();
}

void STV::check_for_elected()
{
    bool check = false;
    QList<int> surpluses;
    for (int i = 0; i < candidates.size(); i++)
    {
        if (candidates[i]->getVotesPerCount().at(countNumber-1) >= quota)
        {
            check = true;
            elected.append(candidates[i]);
            active.removeAt(i);
            int surplus = candidates[i]->getVotesPerCount().at(countNumber-1) - quota;
            candidates[i]->set_surplus(surplus);
            surpluses.append(surplus);
            //surpluses.append(candidates[i]->get_surplus());
        }
        else
            surpluses.append(0);
    }
    if(check)
        surplus_distribution(surpluses);
    else
        excluding_candidates();
}

void STV::surplus_distribution(QList<int> surpluses)
{
    //QList<int> surpluses;
    for (int i = 0; i < surpluses.size(); i++)
    {

    }
}

void STV::excluding_candidates()
{

}

QStringList STV::get_valids()
{
    return valids;
}

QList<Vote *> STV::get_votes()
{
    return validVotes;
}


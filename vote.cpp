#include "vote.h"

Vote::Vote()
{

}

Vote::~Vote()
{
    delete this;
}

Vote::Vote(int id, QString route, QList<int> preferences)
{
    this->id = id;
    this->route = route;
    this->preferences = preferences;
    transferable = true;
    transferable_to = 0;
}

int Vote::get_id()
{
    return id;
}

QString Vote::get_route()
{
    return route;
}

void Vote::set_route(QString r)
{
    route += r;
}

QList<int> Vote::get_preferences()
{
    return preferences;
}

void Vote::set_preferences(QList<int> prefs)
{
    preferences = prefs;
}

void Vote::set_transferable(bool set)
{
    transferable = set;
}

bool Vote::is_transferable()
{
    return transferable;
}

int Vote::get_transferable_to()
{
    return transferable_to;
}

void Vote::set_transferable_to(int t)
{
    transferable_to = t;
}

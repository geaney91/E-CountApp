#include "vote.h"

Vote::Vote()
{

}

Vote::Vote(int id, QString route, QStringList preferences)
{
    this->id = id;
    this->route = route;
    this->preferences = preferences;
}

int Vote::get_id()
{
    return id;
}

QString Vote::get_route()
{
    return route;
}

QStringList Vote::get_preferences()
{
    return preferences;
}

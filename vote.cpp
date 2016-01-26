#include "vote.h"

Vote::Vote()
{

}

Vote::~Vote()
{
    delete this;
}

/*Vote::Vote(const Vote &obj)
{
    ptr = new int;
    *ptr = *obj.ptr;
}*/

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

void Vote::set_route(QString r)
{
    route = r;
}

QStringList Vote::get_preferences()
{
    return preferences;
}

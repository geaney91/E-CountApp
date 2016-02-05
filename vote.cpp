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

Vote::Vote(int id, QString route, QList<QPair<int, bool>> preferences/*QStringList preferences*/)
{
    this->id = id;
    this->route = route;
    this->preferences = preferences;
    transferable = true;
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

QList<QPair<int, bool>> Vote::get_preferences()
{
    return preferences;
}

void Vote::set_transferable(bool set)
{
    transferable = set;
}

bool Vote::is_transferable()
{
    return transferable;
}

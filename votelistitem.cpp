#include "votelistitem.h"

VoteListItem::VoteListItem(Vote *v)
{
    this->v = v;
}

int VoteListItem::getId()
{
    return v->get_id();
}

QString VoteListItem::getRoute()
{
    return v->get_route();
}

QList<int> VoteListItem::getPreferences()
{
    return v->get_preferences();
}

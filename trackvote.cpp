#include "trackvote.h"

TrackVote::TrackVote()
{

}

QString TrackVote::add_assignment_route_string(Candidate *c)
{
    QString route = "- Assigned to " + c->get_Name() + "\n";
    return route;
}

QString TrackVote::add_transferred_in_surplus_route_string(Candidate *c)
{
    QString route = " - Transferred in surplus to " + c->get_Name() + "\n";
    return route;
}

QString TrackVote::add_transferred_in_exclusion_route_string(Candidate *c)
{
    QString route = " - Transferred in exclusion to " + c->get_Name() + "\n";
    return route;
}

QString TrackVote::add_non_transferable_route_string(int countNum)
{
    QString route = " - Became non transferable at count " + QString::number(countNum) + "\n";
    return route;
}

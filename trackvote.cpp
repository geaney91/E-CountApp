#include "trackvote.h"

TrackVote::TrackVote()
{

}

QString TrackVote::add_assignment_route_string(Candidate *c)
{
    QString route = "- Assigned to " + c->get_Name() + "\n";
    return route;
}

QString TrackVote::add_transferred_route_string(Candidate *c)
{
    QString route = "Transferred in surplus to " + c->get_Name() + "\n";
    return route;
}

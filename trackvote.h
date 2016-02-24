#ifndef TRACKVOTE_H
#define TRACKVOTE_H

#include <QMainWindow>
#include "vote.h"
#include "candidate.h"

class TrackVote
{
public:
    TrackVote();
    static QString add_assignment_route_string(Candidate *c);
    static QString add_transferred_in_surplus_route_string(Candidate *c);
    static QString add_transferred_in_exclusion_route_string(Candidate *c);

private:

signals:

public slots:
};

#endif // TRACKVOTE_H

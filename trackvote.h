#ifndef TRACKVOTE_H
#define TRACKVOTE_H

#include <QMainWindow>
#include "vote.h"
#include "candidate.h"

class TrackVote
{
public:
    TrackVote();
    QString add_route_string(Vote *v, Candidate *c);

private:

signals:

public slots:
};

#endif // TRACKVOTE_H

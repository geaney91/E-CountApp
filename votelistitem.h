#ifndef VOTELISTITEM_H
#define VOTELISTITEM_H

#include <QListWidget>
#include "vote.h"

class VoteListItem : public QListWidgetItem
{
public:
    VoteListItem(Vote *v);
    int getId();
    QString getRoute();
    QList<int> getPreferences();

private:
    Vote *v;

signals:

public slots:
};

#endif // VOTELISTITEM_H

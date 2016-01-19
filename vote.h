#ifndef VOTE_H
#define VOTE_H

#include <QMainWindow>
#include <QListWidget>

class Vote
{
public:
    Vote();
    Vote(int id, QString route, QStringList preferences);
    int get_id();
    QString get_route();
    QStringList get_preferences();

private:
    int id;
    QString route;
    QStringList preferences;

signals:

public slots:
};

#endif // VOTE_H

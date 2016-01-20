#ifndef VOTE_H
#define VOTE_H

#include <QMainWindow>
#include <QListWidget>
#include <QMetaType>

class Vote
{

public:
    Vote();
    ~Vote();
    //Vote(const Vote &obj);
    Vote(int id, QString route, QStringList preferences);
    int get_id();
    QString get_route();
    QStringList get_preferences();

private:
    int id;
    QString route;
    QStringList preferences;
    int *ptr;

signals:

public slots:
};

//Q_DECLARE_METATYPE(Vote)

#endif // VOTE_H

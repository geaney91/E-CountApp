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
    Vote(int id, QString route, QList<QPair<int, bool> >);
    int get_id();
    QString get_route();
    void set_route(QString r);
    //QStringList get_preferences();
    QList<QPair<int, bool>> get_preferences();

private:
    int id;
    QString route;
    QList<QPair<int, bool>> preferences;
    //QStringList preferences;
    int *ptr;

signals:

public slots:
};

//Q_DECLARE_METATYPE(Vote)

#endif // VOTE_H

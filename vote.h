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
    void set_preferences(QList<QPair<int, bool>> prefs);
    void set_transferable(bool set);
    bool is_transferable();
    int get_transferable_to();
    void set_transferable_to(int t);

private:
    int id;
    QString route;
    QList<QPair<int, bool>> preferences;
    bool transferable;
    int transferable_to;
    //QStringList preferences;
    int *ptr;

signals:

public slots:
};

//Q_DECLARE_METATYPE(Vote)

#endif // VOTE_H

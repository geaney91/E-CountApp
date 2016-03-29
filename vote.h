#ifndef VOTE_H
#define VOTE_H

#include <QStringList>
#include <QMainWindow>

class Vote
{

public:
    Vote();
    ~Vote();
    Vote(int id, QString route, QList<int> preferences);
    int get_id();
    QString get_route();
    void set_route(QString r);
    QList<int> get_preferences();
    void set_preferences(QList<int> prefs);
    void set_transferable(bool set);
    bool is_transferable();
    int get_transferable_to();
    void set_transferable_to(int t);

private:
    int id;
    QString route;
    QList<int> preferences;
    bool transferable;
    int transferable_to;

signals:

public slots:
};


#endif // VOTE_H

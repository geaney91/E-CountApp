#ifndef VALIDATE_H
#define VALIDATE_H

#include <QStringList>
#include <QMainWindow>

class Validate
{
public:
    Validate();
    QStringList remove_invalids(QStringList votes, QStringList names);
    QStringList get_invalids();

private:
    QStringList invalids;

    bool first_check(QString l);
    bool second_check(QString l);
    bool third_check(QString l, int count);
    bool fourth_check(QString l);
    void add_invalids(QStringList votes, QString l, int i);

signals:

public slots:
};

#endif // VALIDATE_H

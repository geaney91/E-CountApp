#ifndef VALIDATE_H
#define VALIDATE_H

#include <QStringList>
#include <QMainWindow>

class Validate
{
public:
    Validate();
    QStringList remove_invalids(QStringList votes);
    QStringList get_invalids();

private:
    QStringList invalids;

    bool first_check(QString l);

signals:

public slots:
};

#endif // VALIDATE_H

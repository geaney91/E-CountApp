#include "validate.h"
#include <QCoreApplication>

Validate::Validate()
{

}

QStringList Validate::remove_invalids(QStringList votes)
{
    for (int i = 0; i < votes.size();)
    {
        QString l = votes[i];
        if (first_check(l))
        {
            i++;
        }
        else
        {
            votes.removeAt(i);
            invalids.append(l);
        }
    }

    return votes;
}

bool Validate::first_check(QString l)
{
    bool check = true;
    if (!l.contains("1"))
    {
        check = false;
    }

    return check;
}

QStringList Validate::get_invalids()
{
    return invalids;
}


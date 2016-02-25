#include "validate.h"

Validate::Validate()
{

}

QStringList Validate::remove_invalids(QStringList votes, QStringList names)
{
    int count = names.size();
    for (int i = 0; i < votes.size();)
    {
        QString l = votes[i];
        if (first_check(l))
        {
            i++;
            /*if (second_check(l))
            {
                i++;
                if (third_check(l, count))
                {
                    //break;
                }
                else
                {
                    votes.removeAt(i);
                    invalids.append(l);
                    //add_invalids(votes, l, i);
                }
            }
            else
            {
                votes.removeAt(i);
                invalids.append(l);
                //add_invalids(votes, l, i);
            }*/
        }
        else
        {
            votes.removeAt(i);
            invalids.append(l);
            //add_invalids(votes, l, i);
        }
    }

    return votes;
}

void Validate::add_invalids(QStringList votes, QString l, int i)
{
    votes.removeAt(i);
    invalids.append(l);
}

bool Validate::first_check(QString l)
{
    bool check = true;
    if (!l.contains("1"))
    {
        check = false;
    }

    return check;

    //if (!l.contains("1"))
    //    return false;
    //else
    //    return true;
    /*QStringList l1 = l.split(",");
    bool check = true;
    for (int i = 0; i < l1.size() && check; i++)
    {
        QString t = l1[i];
        if (t.isEmpty())
            check = false;
        else
            check = true;
    }*/
    //if(!check)
    //    return false;
    //else
    //    return true;
}

bool Validate::second_check(QString l)
{
    //if(!(std::all_of(l.begin(), l.end(), ::isdigit) || l.contains(",")))
    if (l.contains(QRegExp("[^\\d,.]")))
        return false;
    else
        return true;
}

bool Validate::third_check(QString l, int count)
{
    QList<QString> list = l.split(",");
    bool check = true;
    for (int j = 0; j < list.size() && check; j++)
    {
        int x = list[j].toInt();
        if (x > count)
            check = false;
    }
    return check;
    //if (!check)
    //    return false;
    //else
    //    return true;
}

bool Validate::fourth_check(QString l)
{
    QStringList l1 = l.split(",");
    QList<int> intList;
    int temp = 0;
    bool check = false;
    for (int i = 0; i < l1.size(); i++)
    {
        QString temp = l1[i];
        if (!temp.isEmpty())
            intList.append(temp.toInt());
        else
            intList.append(0);
    }
    for (int j = 0; j < intList.size()-1; j++)
    {
        for (int k = j+1; k < intList.size(); k++)
        {
            if (intList[k] < intList[j])
            {
                temp = intList[k];
                intList[k] = intList[j];
                intList[j] = temp;
            }
        }
    }
    for (int k = 0; k < intList.size() && !check; k++)
    {
        if (intList[k] == 1)
        {
            int t = k;
            while (t < intList.size())
            {
                if (t == intList.size()-1)
                    check = true;
                else if (intList[t+1] == intList[t]+1)
                    check = true;
                else
                {
                    check = false;
                    break;
                }
                t++;
            }
        }
        //if (!check)
        //    break;
    }
    return check;
    //if (!check)
    //    return false;
    //else
    //    return true;
}

QStringList Validate::get_invalids()
{
    return invalids;
}

/*bool Valids(QStringList line)
{
    for (int i = 0; i < line.count(); i++)
    {
        //QString s = line[i];
        //QRegularExpression re("^\\d");

    }
}*/


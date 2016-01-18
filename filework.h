#ifndef FILEWORK_H
#define FILEWORK_H

#include <QMainWindow>

class FileWork
{
public:
    FileWork();
    void Open(QWidget *q);
    void Read();
    int get_Count();
    int get_Seats();
    QStringList get_Votes();
    QStringList get_Candidate_Names();
    QStringList get_Candidate_Parties();
    QString get_FileName();

private:
    int seats;
    QString fileName;
    QStringList votes;
    QString names;
    QString parties;
    int count;

signals:

public slots:
};

#endif // FILEWORK_H

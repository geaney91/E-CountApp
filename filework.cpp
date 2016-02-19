#include "filework.h"

#include <QWidget>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <iostream>
#include <cstdlib>
#include <QString>
#include <string>
#include <fstream>
#include <istream>
#include <QApplication>

FileWork::FileWork()
{

}

void FileWork::Open(QWidget *q)
{
    try{
    fileName = QFileDialog::getOpenFileName(q, QObject::tr("Open File"), ".", QObject::tr("Text Files (*.txt)"));
    }
    catch(std::exception &e)
    {
       QMessageBox::about(q, "title", e.what());
    }

    if (!fileName.isEmpty() && fileName.contains(".txt"))
    {
        QFileInfo file(fileName);
        QString f = file.fileName();
    }
    else
    {
        QMessageBox::information(0, "Error", "File is empty. Exiting application.");
        QApplication::quit();
    }

}

void FileWork::Read()
{
    count = 0;
    int num = 0;
    QString line = "";
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(0, "error", file.errorString());
    }
    QTextStream in(&file);

    while(!in.atEnd())
    {
        if (num == 0)
        {
            line = in.readLine();
            seats = line.toInt();
            num++;
        }
        else if (num == 1)
        {
            line = in.readLine();
            names = line;
            num++;
        }
        else if (num == 2)
        {
            line = in.readLine();
            parties = line;
            num++;
        }
        else
        {
            line = in.readLine();
            votes.append(line);
            count++;
        }
    }
    file.close();

}

int FileWork::get_Count()
{
    return count;
}

int FileWork::get_Seats()
{
    return seats;
}

QStringList FileWork::get_Votes()
{
    return votes;
}

QStringList FileWork::get_Candidate_Names()
{
    QStringList n = names.split(",");
    return n;
}

QStringList FileWork::get_Candidate_Parties()
{
    QStringList p = parties.split(",");
    return p;
}

QString FileWork::get_FileName()
{
    return fileName;
}

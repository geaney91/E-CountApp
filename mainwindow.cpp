#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QWidget>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <iostream>
#include <cstdlib>
#include <QString>
#include <string>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    fileWork = new FileWork();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_chooseFileBtn_clicked()
{
    fileWork->Open(this);
    QFileInfo file(fileWork->get_FileName());
    QString f = file.fileName();
    ui->fileNameLbl->setText(f);
}

void MainWindow::on_countBtn_clicked()
{
    Count *c = new Count(fileWork);
    c->start();
    this->hide();
}



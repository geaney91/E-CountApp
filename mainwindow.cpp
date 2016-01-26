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
#include <QProgressDialog>
#include <QProgressBar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    fileWork = new FileWork();
    ui->counting_lbl->setVisible(false);
    //progressDialog = new QProgressDialog("Counting...", "Abort", 0, INT_MAX, this);
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
    STV *stv = new STV(fileWork);
    /*QProgressDialog dialog("Counting...", "Abort", 0, 100, this);
    QProgressBar *bar = new QProgressBar(&dialog);
    bar->setRange(0, 0);
    bar->setValue(0);
    dialog.setBar(bar);

    dialog.setMinimumWidth(350);
    dialog.setMinimumDuration(1000);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setValue(0);*/
    stv->start();
    this->hide();
    //dialog.setValue(dialog.value() + 1);
    //dialog.close();
}



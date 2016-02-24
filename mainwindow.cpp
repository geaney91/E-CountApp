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
    QProgressDialog *d = new QProgressDialog("Counting..", "Abort", 0, INT_MAX, this);
    d->setValue(0);
    d->setWindowModality(Qt::WindowModal);
    stv->start(d);
    this->hide();
}

void MainWindow::pop_dialog()
{
    //d = new QProgressDialog("Counting..", "Abort", 0, INT_MAX, this);
    //d->show();
}

void MainWindow::close_dialog()
{
    //d->close();
}


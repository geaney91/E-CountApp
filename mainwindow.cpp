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
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    t = new QTimer(this);
    fileWork = new FileWork();
    stv = new STV();
    //QProgressBar bar;// = new QProgressBar(this);
    //connect( stv, &STV::start, this, &MainWindow::start_bar );
    //connect( stv, &STV::display_count_info, this, &MainWindow::stop_bar );
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
    checked = 0;
    if (ui->runFormatCheckbox->isChecked())
    {
        checked = 1;
    }
    counter = 0;
    stv->add_info(fileWork, checked);
    stv->start();
    this->hide();
}

void MainWindow::update_bar()
{
    //connect(t, SIGNAL(timeout()), this, SLOT(update_bar()));
    //t->start(1000);
    counter++;
    ui->progressBar->setValue( counter ); //Should be incremented by one
}

void MainWindow::start_bar()
{
    connect(t, &QTimer::timeout, this, &MainWindow::update_bar);
    t->start(1000);
}

void MainWindow::stop_bar()
{
    ui->progressBar->setValue(100);
    t->stop();
    delete t;
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


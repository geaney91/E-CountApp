#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stv.h"
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
    //t = new QTimer(this);
    ui->progressBar->hide();
    fileWork = new FileWork();
    //stv = new STV();
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
    //FileWork fileWork = new FileWork();
    fileWork->Open(this);
    QFileInfo file(fileWork->get_FileName());
    QString f = file.fileName();
    ui->fileNameLbl->setText(f);
}

void MainWindow::on_countBtn_clicked()
{
    //bar.setMinimum(0);
    //bar.setMaximum(0);
    //bar.show();

    STV *stv = new STV();
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(0);
    ui->progressBar->setRange(0,0);

    //connect(timer, SIGNAL(timeout()), this, SLOT(update_bar()));
    //connect(ui->progressBar, &QProgressBar::valueChanged, this, &MainWindow::on_value_changed);
    //QProgressDialog progress("Copying files...", "Abort Copy", 0, 0);
    //progress.setWindowModality(Qt::WindowModal);
    //progress.setMinimumDuration(0);
    //progress.setRange(0,0);
    //progress.exec();
    ui->progressBar->show();
    timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, this, &MainWindow::on_timeout);
    timer->start(200);
    checked = 0;
    if (ui->runFormatCheckbox->isChecked())
    {
        checked = 1;
    }
    counter = 0;
    stv->add_info(fileWork, checked);
    //progress.hide();
    stv->start();
    //progress.setValue(100);
    this->hide();
}

void MainWindow::on_timeout()
{
    int value = ui->progressBar->value();
    value++;
    ui->progressBar->setValue(value);
}

void MainWindow::on_value_changed()
{

}

void MainWindow::update_bar()
{
    //timer->start(1000);
     if(counter <= 100)
     {
         counter++;
         ui->progressBar->setValue(counter);
     }
     else
     {
         timer->stop();
         delete timer;  //END CODE SNIPPET HERE
         //QString * finalMsg = new QString("Operation completed. All hard drive partitions have been deleted.");
         //ui->msgArea->setText(*finalMsg);
         //delete finalMsg;
     }
    //connect(t, SIGNAL(timeout()), this, SLOT(update_bar()));
    //t->start(1000);
    //counter++;
    //ui->progressBar->setValue( counter ); //Should be incremented by one
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


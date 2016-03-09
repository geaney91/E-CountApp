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
    stv = new STV();
    //QProgressBar bar;// = new QProgressBar(this);
    connect( stv, SIGNAL(start()), this, SLOT(start_bar()) );
    connect( stv, SIGNAL(display_count_info()), this, SLOT(stop_bar()) );
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
    int checked = 0;
    if (ui->runFormatCheckbox->isChecked())
    {
        checked = 1;
    }
    //QProgressBar bar;
    //bar.setMinimum(0);
    //bar.setMaximum(0);
    //bar.show();
    //QProgressDialog *d = new QProgressDialog("Counting..", "Abort", 0, INT_MAX, this);
    //d->setValue(0);
    //d->setWindowModality(Qt::WindowModal);
    stv->add_info(fileWork, checked);
    stv->start();
    if (checked == 0)
        this->close();
}

void MainWindow::start_bar()
{
    bar.setMinimum(0);
    bar.setMaximum(0);
    bar.show();
}

void MainWindow::stop_bar()
{
    bar.setMaximum(10);
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


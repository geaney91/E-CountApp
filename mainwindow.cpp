#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stv.h"
#include <QFile>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->progressBar->hide();
    fileWork = new FileWork();
    ui->counting_lbl->setVisible(false);
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
    STV *stv = new STV();
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(0);
    ui->progressBar->setRange(0,0);
    ui->progressBar->show();
    ui->counting_lbl->setVisible(true);

    checked = 0;
    if (ui->runFormatCheckbox->isChecked())
    {
        checked = 1;
    }

    stv->add_info(fileWork, checked);
    stv->start();

    this->hide();
}


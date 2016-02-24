#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "filework.h"
#include "stv.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void pop_dialog();
    void close_dialog();

private slots:
    void on_chooseFileBtn_clicked();

    void on_countBtn_clicked();

private:
    Ui::MainWindow *ui;
    FileWork *fileWork;
    //QProgressDialog *d;
};

#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "filework.h"
#include "count.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_chooseFileBtn_clicked();

    void on_countBtn_clicked();

private:
    Ui::MainWindow *ui;
    FileWork *fileWork;
};

#endif // MAINWINDOW_H

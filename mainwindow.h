#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressBar>
#include "filework.h"
//#include "stv.h"

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
    void update_bar();
    void on_value_changed();
    void on_timeout();
    void on_countBtn_clicked();
    void start_bar();
    void stop_bar();

private:
    Ui::MainWindow *ui;
    FileWork *fileWork;

    QTimer *t;
    int counter;
    int checked;
    QTimer *timer;
};

#endif // MAINWINDOW_H

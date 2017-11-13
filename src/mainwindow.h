#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

#include "can_config.h"
#include "./j1939/J1939.H"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    ITRFN returnCANinterface(int deviceType);

private:
    Ui::MainWindow *ui;
};

class TSQLThread: public QThread
{
    Q_OBJECT
public:
    TSQLThread(){;}
    void run();
};

#endif // MAINWINDOW_H

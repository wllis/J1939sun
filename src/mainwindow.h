#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

private:
    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H

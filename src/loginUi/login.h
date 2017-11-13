#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include "can_config.h"
#include "./j1939/J1939.H"

namespace Ui {
class login;
}

class login : public QDialog
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = 0);
    ~login();
public:
    int returnDeviceType(int comboBoxDeviceType);
    ITRFN returnCANinterface(int deviceType);
    void setAutoConfig();
    void readAutoConfig();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

private:
    Ui::login *ui;
};

#endif // LOGIN_H

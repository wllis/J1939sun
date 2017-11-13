#include "mainwindow.h"
#include <QApplication>
#include "loginUi/login.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    login _login; //登陆界面
    if(_login.exec()==QDialog::Accepted){
        MainWindow w;
        w.show();
        return a.exec();
    }else{
        return 0;
    }

    return a.exec();
}

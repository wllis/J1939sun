#pragma execution_character_set("utf-8")

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>

TSQLThread *TSQLThread_t;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*测试线程，CAN盒子打开后，定时1S向总线发送J1939数据*/
    TSQLThread_t= new TSQLThread();
    TSQLThread_t->start();
}

MainWindow::~MainWindow()
{
    delete MYCAN;
    delete ui;
}
void TSQLThread::run()
{
    J1939_MESSAGE Msg;

    J1939_Initialization( TRUE );
    //等待地址超时
    while (J1939_Flags.WaitingForAddressClaimContention)
        J1939_Poll(5);

    while(1)
    {
        /********发送数据(参考J1939的ID组成填充下面)***********/
       Msg.Mxe.DataPage = 0;
       Msg.Mxe.Priority = J1939_CONTROL_PRIORITY;
       Msg.Mxe.DestinationAddress = 0x0f;
       Msg.Mxe.DataLength = 8;
       Msg.Mxe.PDUFormat = 0xfe;
       Msg.Mxe.Data[0] = 1;
       Msg.Mxe.Data[1] = 2;
       Msg.Mxe.Data[2] = 3;
       Msg.Mxe.Data[3] = 4;
       Msg.Mxe.Data[4] = 5;
       Msg.Mxe.Data[5] = 6;
       Msg.Mxe.Data[7] = 8;
       Msg.Mxe.Data[6] = 7;

       while (J1939_EnqueueMessage( &Msg ) != RC_SUCCESS)
          J1939_Poll(5);
       /***********************处理接受数据*************************/
       while (RXQueueCount > 0)
       {
           J1939_DequeueMessage( &Msg );
           if (Msg.Mxe.PDUFormat == 0x01)
            while(1); //收到数据 ： 0x xx01f3xx数据后停止
       }

        msleep(100);
        J1939_Poll(5);

    }

}

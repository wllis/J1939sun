#include "login.h"
#include "ui_login.h"

#include "QMessageBox.h"
#include "QFile"
#include <QtDebug>

login::login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);

    /*读取上次启动配置参数，并设置login界面*/
    readAutoConfig();

}

login::~login(){
    delete ui;
}

void login::on_pushButton_clicked(){
    /*读取界面的配置数据*/
    int _DeviceType = returnDeviceType(ui->comboBoxDeviceType->currentIndex()+1);
    int _DeviceInd = ui->comboBoxDeviceInd->currentIndex();
    int _CANInd = ui->comboBoxCANInd->currentIndex();

    /*判断加载CAN盒子类型*/
    ITRFN _interfaceNameStruct = returnCANinterface(ui->comboBoxDeviceType->currentIndex()+1);

    /*开启一个CAN驱动，初始化一个CAN驱动*/
    MYCAN = new USBCAN(_DeviceType,_DeviceInd,_CANInd,_interfaceNameStruct);

    if( !MYCAN->isInitiateSucceed){
        QMessageBox::warning(this,tr("error"),tr("软件加载CAN盒子失败，请检查CAN盒子是否使用正确\n 如果PC机插入多个CAN盒子，请选择正确的索引号"),QMessageBox::Yes);
        return ;
    }

    /*记录本次配置参数*/
    setAutoConfig();

    /*退出登录配置界面*/
    accept();

}

/*
* 说明:带入控件comboBoxDeviceType 当前的值返回设备类型的参数(由驱动提供)
* 输入：comboBoxDeviceType
* 输出：DeviceType
*/
//接口卡类型定义
#define USBCAN1		3
#define USBCAN2		4
#define VCI_USBCAN1		3
#define VCI_USBCAN2		4
#define VCI_USBCAN2A	4
#define VCI_USBCAN_E_U 		20
#define VCI_USBCAN_2E_U 	21

int login::returnDeviceType(int comboBoxDeviceType){

   switch (comboBoxDeviceType){
       case 1:
           return USBCAN1;
       case 2:
           return USBCAN2;
       case 3:
           return VCI_USBCAN1;
       case 4:
           return VCI_USBCAN2;
       case 5:
           return VCI_USBCAN2A;
       case 6:
           return VCI_USBCAN_E_U;
       case 7:
           return VCI_USBCAN_2E_U;
       default :
           QMessageBox::warning(this,tr("警告"),tr("产生一个错误的DeviceType的返回!"),QMessageBox::Yes);
           return -1;
   }
}
ITRFN login::returnCANinterface(int deviceType){
    static ITRFN ECANinterfaceNameStruct{"ECanVci.dll",
                                     "OpenDevice",
                                     "CloseDevice",
                                     "InitCAN",
                                     "ReadBoardInfo",
                                     "SetReference",
                                     "GetReceiveNum",
                                     "ClearBuffer",
                                     "StartCAN",
                                     "ResetCAN",
                                     "Transmit",
                                     "Receive"
                                     };
    static ITRFN ControlCANinterfaceNameStruct{"ControlCAN.dll",
                                     "VCI_OpenDevice",
                                     "VCI_CloseDevice",
                                     "VCI_InitCAN",
                                     "VCI_ReadBoardInfo",
                                     "VCI_SetReference",
                                     "VCI_GetReceiveNum",
                                     "VCI_ClearBuffer",
                                     "VCI_StartCAN",
                                     "VCI_ResetCAN",
                                     "VCI_Transmit",
                                     "VCI_Receive"
                                     };
    if((deviceType == 3) || (deviceType == 4)|| (deviceType == 5)|| (deviceType == 6)|| (deviceType == 7)){
        return ControlCANinterfaceNameStruct;
    }
    if((deviceType == 1)|| (deviceType == 2)){
        return ECANinterfaceNameStruct;
    }
    QMessageBox::warning(this,tr("错误"),tr("没有相应的CAN盒子驱动"),QMessageBox::Yes);
    return ECANinterfaceNameStruct;//随便返回一个类型
}

void login::setAutoConfig(){

    QFile file("initconfig.txt");
    file.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text);
    file.seek(0);  //回到文件开头

    QTextStream out(&file);
    out << "DeviceType:"<< endl;
    out << ui->comboBoxDeviceType->currentIndex()+1 << endl;
    out << "DeviceInd:"<< endl;
    out << ui->comboBoxDeviceInd->currentIndex() << endl;
    out << "CANInd:"<< endl;
    out << ui->comboBoxCANInd->currentIndex() << endl;

    file.close();
}
void login::readAutoConfig(){

    QFile file("initconfig.txt");
    file.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text);
    file.seek(0);  //回到文件开头

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if("DeviceType:" == line){
            line = in.readLine();
            ui->comboBoxDeviceType->setCurrentIndex(line.toInt()-1);
        }
        if("DeviceInd:" == line){
            line = in.readLine();
            ui->comboBoxDeviceInd->setCurrentIndex(line.toInt());
        }
        if("CANInd:" == line){
            line = in.readLine();
            ui->comboBoxCANInd->setCurrentIndex(line.toInt());
        }
    }

    file.close();
}

void login::on_pushButton_2_clicked(){
    this->close();
}

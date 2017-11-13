#pragma execution_character_set("utf-8")

#ifndef CAN_CONFIG_H
#define CAN_CONFIG_H

#include <QtDebug>
#include <stdio.h>
#include "Windows.h"

//#include "windef.h"//MinGW

#include "./ECAN/ECanVci.h"

#include "controlcan/ControlCAN.h"

typedef DWORD (*funOpenDevice) (DWORD, DWORD, DWORD);
typedef DWORD (*funCloseDevice) (DWORD,  DWORD);
typedef DWORD (*funInitCAN) (DWORD, DWORD, DWORD, P_INIT_CONFIG);
typedef DWORD (*funReadBoardInfo) (DWORD,DWORD ,P_BOARD_INFO);
typedef DWORD (*funReadErrInfo) (DWORD ,DWORD ,DWORD ,P_ERR_INFO);
typedef DWORD (*funReadCANStatus)(DWORD ,DWORD ,DWORD ,P_CAN_STATUS);
typedef DWORD (*funGetReference) (DWORD ,DWORD ,DWORD ,DWORD ,PVOID );
typedef DWORD (*funSetReference) (DWORD ,DWORD ,DWORD ,DWORD ,PVOID );
typedef ULONG (*funGetReceiveNum) (DWORD ,DWORD ,DWORD );
typedef DWORD (*funClearBuffer) (DWORD ,DWORD ,DWORD );
typedef DWORD (*funStartCAN) (DWORD ,DWORD ,DWORD );
typedef DWORD (*funResetCAN)(DWORD ,DWORD ,DWORD );
typedef DWORD (*funTransmit)(DWORD ,DWORD ,DWORD ,P_CAN_OBJ ,ULONG );
typedef DWORD (*funReceive)(DWORD ,DWORD ,DWORD ,P_CAN_OBJ ,ULONG,INT);

typedef struct {
    char dllname[40];
    char usbcanOpenDevice[40];
    char usbcanCloseDevice[40];
    char usbcanInitCAN[40];
    char usbcanReadBoardInfo[40];
    char usbcanSetReference[40];
    char usbcanGetReceiveNum[40];
    char usbcanClearBuffer[40];
    char usbcanStartCAN[40];
    char usbcanResetCAN[40];
    char usbcanTransmit[40];
    char usbcanReceive[40];

}ITRFN; //interface name

// 以后在析构函数中解决，动态加载那个can盒子
class USBCAN
{
private:
    int nDeviceType; //设备号
    int nDeviceInd;  //引索号
    int nReserved;   //通常设为0
    int nCANInd;     //can通道号]

    HINSTANCE CANLIB;

    ITRFN INTERFACENAME;

    funOpenDevice CANDriveOpenDevice;
    funCloseDevice CANDriveCloseDevice;
    funInitCAN CANDriveInitCAN;
    funReadBoardInfo CANDriveReadBoardInfo;
    funReadErrInfo CANDriveReadErrInfo;
    funReadCANStatus CANDriveReadCANStatus;
    funGetReference CANDriveGetReference;
    funGetReceiveNum CANDriveGetReceiveNum;
    funClearBuffer CANDriveClearBuffer;
    funStartCAN CANDriveStartCAN;
    funResetCAN CANDriveResetCAN;
    funTransmit CANDriveTransmit;
    funReceive CANDriveReceive;


    INIT_CONFIG vic;//配置初始化结构体
    BOARD_INFO vbi;//USBCAN的信息结构体
    CAN_OBJ nTransmitData;//发送数据结构体
    BYTE nReceiveTime;//接受超时时间,以毫秒为单位
public:
    CAN_OBJ nReceiveData[100];  //用于接受数据的结构体,暂时用100个
public:
    USBCAN(int DeviceType,int DeviceInd,int CANInd,const ITRFN interfaceNameStruct)
    {
        nDeviceType = DeviceType;
        nDeviceInd = DeviceInd;
        nReserved = 0;
        nCANInd = CANInd;
        nReceiveTime = 250;

        qDebug() << nDeviceType;
        qDebug() << nDeviceInd;
        qDebug() << nCANInd;

        INTERFACENAME = interfaceNameStruct;
        qDebug() << interfaceNameStruct.dllname;
        /*加载CAN盒子的动态库*/
        CANLIB = LoadLibraryA(interfaceNameStruct.dllname);
        if(CANLIB == NULL){
            qDebug() << "\nunable to loading dll";
            return ;
        }

        ZeroMemory(&vic, sizeof(INIT_CONFIG));
        ZeroMemory(&vbi, sizeof(BOARD_INFO));
        ZeroMemory(&nTransmitData, sizeof(CAN_OBJ));
        vic.AccMask = 0xffffffff;
        vic.Filter = 0;//不使能滤波
        vic.Mode = 0; //0表示正常模式，1表示只听模式
        /*代表波特率为250Kbps*/
        vic.Timing0 = 0x01;
        vic.Timing1 = 0x1c;

        DWORD dwRel;
        Sleep(100);
        dwRel = ECanOpenDevice();
        if (dwRel != STATUS_OK)
        {
            qDebug() << "打开USBCAN失败\n";
            return ;
        }
        else
        {
            qDebug() << "打开USBCAN成功\n";
        }
        Sleep(100);
        dwRel = ECanInitCAN();
        Sleep(100);

        /*获取函数地址*/
        CANDriveClearBuffer = (funClearBuffer)GetProcAddress(CANLIB,INTERFACENAME.usbcanClearBuffer);
        if(CANDriveClearBuffer == NULL)
        {
            qDebug() << "\nunable to loading dll CANDriveClearBuffer";
            FreeLibrary(CANLIB);
        }
        CANDriveClearBuffer(nDeviceType, nDeviceInd, nCANInd);
        if (dwRel != STATUS_OK)
        {
            qDebug() << "初始化USBCAN失败\n";
            ECanCloseDevice();
            qDebug() << "USBCAN设备已经关闭\n";
            return ;
        }
        else
        {
            qDebug() << "初始化USBCAN成功\n";
        }
        Sleep(100);
        dwRel = ECanReadBoardInfo();
        if(dwRel != STATUS_OK)
        {
            qDebug() << "读取USBCAN的基础信息失败\n";
            ZeroMemory(&vbi, sizeof(BOARD_INFO));
        }
        else
        {
            qDebug() << "读取USBCAN的基础信息成功\n";
        }
        Sleep(100);
        dwRel = ECanStartCAN();
        if (dwRel != STATUS_OK)
        {
            qDebug() << "USBCAN启动失败\n";
            ZeroMemory(&vbi, sizeof(BOARD_INFO));
            ECanCloseDevice();
            qDebug() << "USBCAN设备已经关闭\n";
        }
        else
        {
            qDebug() << "USBCAN启动成功\n";
        }
        Sleep(1000);
    }
    ~USBCAN()
    {
        ECanCloseDevice();
    }
    void ECanReset()
    {
        nReceiveTime = 250;

        ZeroMemory(&vic, sizeof(INIT_CONFIG));
        ZeroMemory(&vbi, sizeof(BOARD_INFO));
        ZeroMemory(&nTransmitData, sizeof(CAN_OBJ));
        vic.AccMask = 0xffffffff;
        vic.Filter = 0;//不使能滤波
        vic.Mode = 0; //0表示正常模式，1表示只听模式
                      /*代表波特率为250Kbps*/
        vic.Timing0 = 0x01;
        vic.Timing1 = 0x1c;

        DWORD dwRel;
        Sleep(100);
        dwRel = ECanOpenDevice();
        if (dwRel != STATUS_OK)
        {
            qDebug() << "打开USBCAN失败\n";
            return;
        }
        else
        {
            qDebug() << "打开USBCAN成功\n";
        }
        Sleep(100);
        dwRel = ECanInitCAN();

        Sleep(100);

        /*获取函数地址*/
        CANDriveClearBuffer = (funClearBuffer)GetProcAddress(CANLIB,INTERFACENAME.usbcanClearBuffer);
        if(CANDriveClearBuffer == NULL)
        {
            qDebug() << "\nunable to loading dll CANDriveClearBuffer";
            FreeLibrary(CANLIB);
        }
        CANDriveClearBuffer(nDeviceType, nDeviceInd, nCANInd);
        if (dwRel != STATUS_OK)
        {
            qDebug() << "初始化USBCAN失败\n";
            ECanCloseDevice();
            qDebug() << "USBCAN设备已经关闭\n";
            return;
        }
        else
        {
            qDebug() << "初始化USBCAN成功\n";
        }
        Sleep(100);
        dwRel = ECanReadBoardInfo();
        if (dwRel != STATUS_OK)
        {
            qDebug() << "读取USBCAN的基础信息失败\n";
            ZeroMemory(&vbi, sizeof(BOARD_INFO));
        }
        else
        {
            qDebug() << "读取USBCAN的基础信息成功\n";
        }
        Sleep(100);
        dwRel = ECanStartCAN();
        if (dwRel != STATUS_OK)
        {
            qDebug() << "USBCAN启动失败\n";
            ZeroMemory(&vbi, sizeof(BOARD_INFO));
            ECanCloseDevice();
            qDebug() << "USBCAN设备已经关闭\n";
        }
        else
        {
            qDebug() << "USBCAN启动成功\n";
        }
        Sleep(1000);
    }
    DWORD ECanOpenDevice()
    {
        /*获取函数地址*/
        CANDriveOpenDevice = (funOpenDevice)GetProcAddress(CANLIB,INTERFACENAME.usbcanOpenDevice);
        if(CANDriveOpenDevice == NULL)
        {
            qDebug() << "\nunable to loading dll CANDriveOpenDevice";
            FreeLibrary(CANLIB);
        }
        return CANDriveOpenDevice(nDeviceType, nDeviceInd, nReserved);
    }
    DWORD ECanCloseDevice()
    {
        /*获取函数地址*/
        CANDriveCloseDevice = (funCloseDevice)GetProcAddress(CANLIB,INTERFACENAME.usbcanCloseDevice);
        if(CANDriveCloseDevice == NULL)
        {
            qDebug() << "\nunable to loading dll CANDriveCloseDevice";
            FreeLibrary(CANLIB);
        }
        return CANDriveCloseDevice(nDeviceType, nDeviceInd);
    }
    DWORD ECanInitCAN()
    {
        /*获取函数地址*/
        CANDriveInitCAN = (funInitCAN)GetProcAddress(CANLIB,INTERFACENAME.usbcanInitCAN);
        if(CANDriveInitCAN == NULL)
        {
            qDebug() << "\nunable to loading dll CANDriveInitCAN";
            FreeLibrary(CANLIB);
        }
        return CANDriveInitCAN(nDeviceType, nDeviceInd, nCANInd,&vic);
    }
    DWORD ECanStartCAN()
    {
        /*获取函数地址*/
        CANDriveStartCAN = (funStartCAN)GetProcAddress(CANLIB,INTERFACENAME.usbcanStartCAN);
        if(CANDriveStartCAN == NULL)
        {
            qDebug() << "\nunable to loading dll CANDriveStartCAN";
            FreeLibrary(CANLIB);
        }
        return CANDriveStartCAN(nDeviceType, nDeviceInd, nCANInd);
    }
    DWORD ECanTransmit(UINT id,BYTE sendtype, BYTE remoteflag, BYTE externflag, BYTE datalen,BYTE data[8])
    {
        nTransmitData.ID = id;
        nTransmitData.SendType = sendtype;        //发送帧类型
        nTransmitData.RemoteFlag = remoteflag;    //是否是远程帧 =0时为数据帧，=1时为远程帧
        nTransmitData.ExternFlag = externflag;    //是否是扩展帧 =0时为标准帧（11位帧ID）=1时为扩展帧（29位帧ID）
        nTransmitData.DataLen = datalen;          //数据长度    <=8
        int i = 0;
        for (i = 0; i < datalen; i++)
        {
            nTransmitData.Data[i] = data[i];
        }
        for ( ; i < 8; i++)
        {
            nTransmitData.Data[i] =0xff;
        }
        /*获取函数地址*/
        CANDriveTransmit = (funTransmit)GetProcAddress(CANLIB,INTERFACENAME.usbcanTransmit);
        if(CANDriveTransmit == NULL)
        {
            qDebug() << "\nunable to loading dll CANDriveTransmit";
            FreeLibrary(CANLIB);
        }
        return CANDriveTransmit(nDeviceType, nDeviceInd, nCANInd,&nTransmitData, 1);
    }
    /*调用函数后，数据将存入nReceiveData[]缓存,返回读取到帧的个数*/
    DWORD ECanReceive()
    {
        /*获取函数地址*/
        CANDriveGetReceiveNum = (funGetReceiveNum)GetProcAddress(CANLIB,INTERFACENAME.usbcanGetReceiveNum);
        if(CANDriveGetReceiveNum == NULL)
        {
            qDebug() << "\nunable to loading dll CANDriveGetReceiveNum";
            FreeLibrary(CANLIB);
        }
        /*缓存清零*/
        unsigned int DwRel = CANDriveGetReceiveNum(nDeviceType, nDeviceInd, nCANInd);
        for (unsigned int i = 0; i < DwRel; i++)
        {
            ZeroMemory((nReceiveData + i), sizeof(CAN_OBJ));
        }
        if(DwRel != 0)
        {
            if(DwRel == 0xFFFFFFFF)
            {
                /*获取函数地址*/
                CANDriveResetCAN = (funResetCAN)GetProcAddress(CANLIB,INTERFACENAME.usbcanResetCAN);
                if(CANDriveResetCAN == NULL)
                {
                    qDebug() << "\nunable to loading dll CANDriveResetCAN";
                    FreeLibrary(CANLIB);
                }
                /*如果返回值为0xFFFFFFFF，则表示读取数据失败，有错误发生，请调用ReadErrInfo函数来获取错误码*/
                CANDriveResetCAN(nDeviceType, nDeviceInd, nCANInd);
                return 0;
            }

            /*获取函数地址*/
            CANDriveReceive = (funReceive)GetProcAddress(CANLIB,INTERFACENAME.usbcanReceive);
            if(CANDriveReceive == NULL)
            {
                qDebug() << "\nunable to loading dll CANDriveReceive";
                FreeLibrary(CANLIB);
            }
            /*返回实际读取到的帧数。如果返回值为0xFFFFFFFF，则表示读取数据失败，有错误发生，请调用ReadErrInfo函数来获取错误码*/
            int _DwRell = CANDriveReceive(nDeviceType, nDeviceInd, nCANInd, nReceiveData, 100, nReceiveTime);

            /*获取函数地址*/
            CANDriveClearBuffer = (funClearBuffer)GetProcAddress(CANLIB,INTERFACENAME.usbcanClearBuffer);
            if(CANDriveClearBuffer == NULL)
            {
                qDebug() << "\nunable to loading dll CANDriveClearBuffer";
                FreeLibrary(CANLIB);
            }
            CANDriveClearBuffer(nDeviceType, nDeviceInd, nCANInd);
            //CANDriveResetCAN(nDeviceType, nDeviceInd, nCANInd);
            if(_DwRell == 0xFFFFFFFF)
            {
                /*获取函数地址*/
                CANDriveResetCAN = (funResetCAN)GetProcAddress(CANLIB,INTERFACENAME.usbcanResetCAN);
                if(CANDriveResetCAN == NULL)
                {
                    qDebug() << "\nunable to loading dll CANDriveResetCAN";
                    FreeLibrary(CANLIB);
                }
                CANDriveResetCAN(nDeviceType, nDeviceInd, nCANInd);
                return 0;
            }
            return  _DwRell;
        }

        return 0;
    }
    DWORD ECanReadBoardInfo()
    {
        /*获取函数地址*/
        CANDriveReadBoardInfo = (funReadBoardInfo)GetProcAddress(CANLIB,INTERFACENAME.usbcanReadBoardInfo);
        if(CANDriveReadBoardInfo == NULL)
        {
            qDebug() << "\nunable to loading dll CANDriveReadBoardInfo";
            FreeLibrary(CANLIB);
        }
        return CANDriveReadBoardInfo(nDeviceType, nDeviceInd, &vbi);
    }
};

extern USBCAN *MYCAN;

#endif

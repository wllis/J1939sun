#pragma execution_character_set("utf-8")

#ifndef CAN_CONFIG_H
#define CAN_CONFIG_H

#include <QtDebug>
#include "windows.h"
//#include "windef.h"//MinGW

// 说明：根据不同的can盒子调用不同的接口，为适配层,

#if USE_ECAN

#include "./ECAN/ECanVci.h"

#define CANDriveOpenDevice(DeviceType, DeviceInd, Reserved)                      OpenDevice( DeviceType, DeviceInd, Reserved)
#define CANDriveCloseDevice( DeviceType, DeviceInd)                              CloseDevice( DeviceType, DeviceInd)

#define CANDriveInitCAN( DeviceType,  DeviceInd,  CANInd,  pInitConfig)          InitCAN( DeviceType,  DeviceInd,  CANInd,  pInitConfig)

#define CANDriveReadBoardInfo( DeviceType, DeviceInd, pInfo)                     ReadBoardInfo( DeviceType, DeviceInd, pInfo)

#define CANDriveSetReference( DeviceType, DeviceInd, CANInd, RefType, pData)     SetReference( DeviceType, DeviceInd, CANInd, RefType, pData)

#define CANDriveGetReceiveNum( DeviceType, DeviceInd, CANInd)                    GetReceiveNum( DeviceType, DeviceInd, CANInd)
#define CANDriveClearBuffer( DeviceType, DeviceInd, CANInd)                      ClearBuffer( DeviceType, DeviceInd, CANInd)

#define CANDriveStartCAN( DeviceType, DeviceInd, CANInd)                         StartCAN( DeviceType, DeviceInd, CANInd)
#define CANDriveResetCAN( DeviceType, DeviceInd, CANInd)                         ResetCAN( DeviceType, DeviceInd, CANInd)

#define CANDriveTransmit( DeviceType, DeviceInd, CANInd, pSend, Len)             Transmit( DeviceType, DeviceInd, CANInd, pSend, Len)
#define CANDriveReceive( DeviceType, DeviceInd, CANInd, pReceive, Len, WaitTime) Receive( DeviceType, DeviceInd, CANInd, pReceive, Len, WaitTime)

#endif


#if USE_CONTROLCAN
#include "controlcan/ControlCAN.h"

#define CAN_OBJ VCI_CAN_OBJ
#define BOARD_INFO VCI_BOARD_INFO
#define INIT_CONFIG VCI_INIT_CONFIG

#define CANDriveOpenDevice(DeviceType, DeviceInd, Reserved)                      VCI_OpenDevice( DeviceType, DeviceInd, Reserved)
#define CANDriveCloseDevice( DeviceType, DeviceInd)                              VCI_CloseDevice( DeviceType, DeviceInd)

#define CANDriveInitCAN( DeviceType,  DeviceInd,  CANInd,  pInitConfig)          VCI_InitCAN( DeviceType,  DeviceInd,  CANInd,  pInitConfig)

#define CANDriveReadBoardInfo( DeviceType, DeviceInd, pInfo)                     VCI_ReadBoardInfo( DeviceType, DeviceInd, pInfo)

#define CANDriveSetReference( DeviceType, DeviceInd, CANInd, RefType, pData)     VCI_SetReference( DeviceType, DeviceInd, CANInd, RefType, pData)

#define CANDriveGetReceiveNum( DeviceType, DeviceInd, CANInd)                    VCI_GetReceiveNum( DeviceType, DeviceInd, CANInd)
#define CANDriveClearBuffer( DeviceType, DeviceInd, CANInd)                      VCI_ClearBuffer( DeviceType, DeviceInd, CANInd)

#define CANDriveStartCAN( DeviceType, DeviceInd, CANInd)                         VCI_StartCAN( DeviceType, DeviceInd, CANInd)
#define CANDriveResetCAN( DeviceType, DeviceInd, CANInd)                         VCI_ResetCAN( DeviceType, DeviceInd, CANInd)

#define CANDriveTransmit( DeviceType, DeviceInd, CANInd, pSend, Len)             VCI_Transmit( DeviceType, DeviceInd, CANInd, pSend, Len)
#define CANDriveReceive( DeviceType, DeviceInd, CANInd, pReceive, Len, WaitTime) VCI_Receive( DeviceType, DeviceInd, CANInd, pReceive, Len, WaitTime)

#endif


// 以后在析构函数中解决，动态加载那个can盒子
class USBCAN
{
private:
    int nDeviceType; //设备号
    int nDeviceInd;  //引索号
    int nReserved;   //通常设为0
    int nCANInd;     //can通道号^can1 = 1,can2 = 2

    INIT_CONFIG vic;//配置初始化结构体
    BOARD_INFO vbi;//USBCAN的信息结构体
    CAN_OBJ nTransmitData;//发送数据结构体
    CAN_OBJ nReceiveData[100];  //用于接受数据的结构体,暂时用100个

    BYTE nReceiveTime;//接受超时时间,以毫秒为单位

public:
    USBCAN()
    {
        nDeviceType = 4; // USBCAN-II
        nDeviceInd = 0;  // 默认为0
        nReserved = 0;   // 默认为0
        nCANInd = 0;
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
            return ;
        }
        else
        {
            qDebug() << "打开USBCAN成功\n";
        }
        Sleep(100);
        dwRel = ECanInitCAN();
        Sleep(100);
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
        nDeviceType = 4; // USBCAN-II
        nDeviceInd = 0;  // 默认为0
        nReserved = 0;   // 默认为0
        nCANInd = 0;
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
        return CANDriveOpenDevice(nDeviceType, nDeviceInd, nReserved);
    }
    DWORD ECanCloseDevice()
    {
        return CANDriveCloseDevice(nDeviceType, nDeviceInd);
    }
    DWORD ECanInitCAN()
    {
        return CANDriveInitCAN(nDeviceType, nDeviceInd, nCANInd,&vic);
    }
    DWORD ECanStartCAN()
    {
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
        return CANDriveTransmit(nDeviceType, nDeviceInd, nCANInd,&nTransmitData, 1);
    }
    /*调用函数后，数据将存入nReceiveData[]缓存*/
    DWORD ECanReceive()
    {
        /*缓存清零*/
        Sleep(100);
    //	ECanStartCAN();
        int DwRel = CANDriveGetReceiveNum(nDeviceType, nDeviceInd, nCANInd);
        for (int i = 0; i < DwRel; i++)
        {
            ZeroMemory((nReceiveData + i), sizeof(CAN_OBJ));
        }
        if(DwRel != 0)
        {
            /*返回实际读取到的帧数。如果返回值为0xFFFFFFFF，则表示读取数据失败，有错误发生，请调用ReadErrInfo函数来获取错误码*/
            int DwRell = CANDriveReceive(nDeviceType, nDeviceInd, nCANInd, nReceiveData, 100, nReceiveTime);
            CANDriveClearBuffer(nDeviceType, nDeviceInd, nCANInd);
        //	CANDriveResetCAN(nDeviceType, nDeviceInd, nCANInd);
            return  DwRell;
        }
        //CANDriveResetCAN(nDeviceType, nDeviceInd, nCANInd);

        return 0;
    }
    DWORD ECanReadBoardInfo()
    {
        return CANDriveReadBoardInfo(nDeviceType, nDeviceInd, &vbi);
    }
};


#endif

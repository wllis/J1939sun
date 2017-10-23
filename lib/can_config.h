#ifndef CAN_CONFIG_H
#define CAN_CONFIG_H

// 说明：根据不同的can盒子调用不同的接口，为适配层,
// 后面来解决CAN盒子的动态适配问题

#if USR_ECAN

#include "./ECAN/ECanVci.h"

#endif


#if USE_CONTROLCAN
#include "controlcan/ControlCAN.h"

#define CAN_OBJ VCI_CAN_OBJ

#define OpenDevice(DeviceType, DeviceInd, Reserved)  VCI_OpenDevice( DeviceType, DeviceInd, Reserved)
#define  CloseDevice( DeviceType, DeviceInd) VCI_CloseDevice( DeviceType, DeviceInd)
#define InitCAN( DeviceType,  DeviceInd,  CANInd,  pInitConfig)  VCI_InitCAN( DeviceType,  DeviceInd,  CANInd,  pInitConfig)

#define ReadBoardInfo( DeviceType, DeviceInd, pInfo)  VCI_ReadBoardInfo( DeviceType, DeviceInd, pInfo)

#define  SetReference( DeviceType, DeviceInd, CANInd, RefType, pData) VCI_SetReference( DeviceType, DeviceInd, CANInd, RefType, pData)

#define  GetReceiveNum( DeviceType, DeviceInd, CANInd) VCI_GetReceiveNum( DeviceType, DeviceInd, CANInd)
#define  ClearBuffer( DeviceType, DeviceInd, CANInd) VCI_ClearBuffer( DeviceType, DeviceInd, CANInd)

#define StartCAN( DeviceType, DeviceInd, CANInd) VCI_StartCAN( DeviceType, DeviceInd, CANInd)
#define  ResetCAN( DeviceType, DeviceInd, CANInd) VCI_ResetCAN( DeviceType, DeviceInd, CANInd)

#define Transmit( DeviceType, DeviceInd, CANInd, pSend, Len) VCI_Transmit( DeviceType, DeviceInd, CANInd, pSend, Len)
#define Receive( DeviceType, DeviceInd, CANInd, pReceive, Len, WaitTime) VCI_Receive( DeviceType, DeviceInd, CANInd, pReceive, Len, WaitTime)

#endif

#endif

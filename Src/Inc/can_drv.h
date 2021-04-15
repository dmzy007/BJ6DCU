#ifndef __CAN_DRV_H
#define __CAN_DRV_H

#include "stm32f10x.h"
#include "bsp.h"

#define CANMAXCONT		48

/**************************************************************/
#define YUEZHANCMDHEAD          0x18C980        //越战
#define LINEINFOCMDHEAD         0x189280        //线路设置
#define ZHUBEICMDHEAD           0x188B80        //主备机
#define STARTSTOPCMDHEAD        0x188980        //开停
#define ALARMCMDHEAD            0x18B180        //报警
#define TALKCMDHEAD             0x188D80        //对讲
#define QUERYVOICECMDHEAD       0x188C80        //查询音量
#define SETVOICECMDHEAD         0x188580        //设置紧急广播
#define SETVOLUMECMDHEAD        0x188C80        //设置音量
#define ERRORCMDHEAD            0x18B280        //回应主机查询
#define ZHUJICMDHEAD            0x18BB80        //DCU主备机
typedef struct{
CanTxMsg       TxMessage[CANMAXCONT];
u8 CANtxSP;
u8 CANtxProcessSP;

CanRxMsg RxMessage[CANMAXCONT];
u8 CANrxSP;
u8 CANrxProcessSP;
} CANTypeDef;

void HexToAscii(uint8_t *InBuffer, vu8 *OutBuffer, unsigned int Num);
void CAN_Configuration(void);
void DecreAlarmID(vu8 Data);
void CAN_SendAlarmCMD(unsigned char CMD, unsigned char Pammater0);
void CAN_SendYueZhanCMD(unsigned char Pammater0, unsigned char Pammater1, unsigned char Pammater2, unsigned char Pammater3, unsigned char Pammater4, unsigned char Pammater5, unsigned char Pammater6, unsigned char CMD);
void CAN_SendLineInfoCMD(unsigned char Pammater0, unsigned char Pammater1, unsigned char Pammater2, unsigned char Pammater3, unsigned char Pammater4);
void CAN_SendFirstZhuBeiCMD(unsigned char CMD, unsigned char Pammater0, unsigned char Pammater1, unsigned char Pammater2, unsigned char Pammater3, unsigned char Pammater4, unsigned char Pammater5);
void CAN_SendZhuBeiCMD(unsigned char CMD, unsigned char Pammater0, unsigned char Pammater1, unsigned char Pammater2, unsigned char Pammater3, unsigned char Pammater4, unsigned char Pammater5);
void CAN_SendZhuJICMD(unsigned char CMD);
void CAN_SendTalkCMD(unsigned char CMD);
void CAN_SendStartStopCMD(unsigned char CMD);
void CAN_SendQueryVoiceCMD(unsigned char CMD);
void CAN_SendSetVoiceCMD(unsigned char CMD,unsigned char VoiceH,unsigned char VoiceL);
void CAN_SendSetVolumeCMD(unsigned char CMD, unsigned char ManVolumeH, unsigned char ManVolumeL, unsigned char MP3VolumeH, unsigned char MP3VolumeL,unsigned char RadioVolumeH, unsigned char RadioVolumeL);
void CAN_Reciver_data_Analyse(void);

void CAN_FirstSendQueryVoiceCMD(unsigned char CMD);
void CAN_FirstSendZhuBeiCMD(unsigned char CMD, unsigned char Pammater0, unsigned char Pammater1, unsigned char Pammater2, unsigned char Pammater3, unsigned char Pammater4, unsigned char Pammater5);
#endif

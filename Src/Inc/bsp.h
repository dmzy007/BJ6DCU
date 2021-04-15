#ifndef __BSP_H_
#define __BSP_H_
#include "3812.h"
#include "Key.h"
#include "LCD.h"
#include "Timer.h"
#include "can_drv.h"
#include "stdbool.h"
#include "stm32f10x.h"
#include "string.h"

//�˹��㲥״̬����
#define MANGB_PIN GPIO_Pin_11
#define MANGB_PORT GPIOA
#define MANGB_in (GPIO_ReadInputDataBit(MANGB_PORT, MANGB_PIN))

//�����AMC1,����˹��㲥
#define AMC1_PIN GPIO_Pin_10
#define AMC1_PORT GPIOC
#define AMC1_OUTPUT_H() GPIO_SetBits(AMC1_PORT, AMC1_PIN)
#define AMC1_OUTPUT_L() GPIO_ResetBits(AMC1_PORT, AMC1_PIN)

//������忴�Ź�
#define WDI_PIN GPIO_Pin_12
#define WDI_PORT GPIOA
#define WDI_CLR() WDI_PORT->ODR ^= WDI_PIN

#define HeadID 0x10
#define TailID 0x28
/*******************************************************
 �����ṹ��
 ***********************************************************/
typedef struct
{
    vu16 Man_Volume;    //�˹�����
    vu16 Talk_Volume;   //�Խ�������
    vu16 MP3_Volume;    // MP3����(�Զ��㲥)
    vu16 Radio_Volume;  //��������
    vu16 JT_Volume;     //�Խ�������
} VolumeTypeDef;

typedef struct
{
    vu8 MainScreen;
    vu8 FuScreen;
} ScreenTypeDef;

typedef struct
{
    vu8  StartStation;           //���վ
    vu8  EndStation;             //�յ�վ
    vu8  NowStation;             //��ǰվ
    vu8  NextStation;            //��һվ
    vu8  SpeciaStation;          //�����յ�
    vu8  LineNo;                 //��·��
    vu8  BroadcastMode;          //�㲥ģʽ   00�޹㲥��01�˹��㲥��02���ߣ�03�Զ���04����
    vu8  BroadcastType;          //�㲥����   00�޹㲥��01�Զ� 02 ���Զ� 03 �ֶ�
    vu8  ErrorCode;              //������Ϣ�豸����
    vu8  DoorStatus;             //��״̬
    vu8  BroadcastStatus;        //�㲥״̬
    vu8  ShangXiaxing;           //�����б�־λ
    vu8  TMStatus;               // TMS״̬
    vu8  LieCheBianZu;           //�г�����
    vu8  ChongLianNum;           //������������
    vu8  YueZhan[64];            //Խս��Ϣ
    vu8  TotalYueZhan;           //Խս����
    vu8  LieCheAddress;          //�г�ͷβID
    vu8  AlarmTotal;             //����������
    vu8  BaoJingCheXiangID[12];  //��������ID�������г��ź�λ��
    vu8  BaoJingCheXiangIDString[12][9];
    vu8  LieCheID;             //��ǰ�г�λ��
    vu16 LieCheHao;            //��ǰ�г���
    vu8  LieCheHaoString[5];   //��ǰ�г��Ŷ�Ӧ���ַ���
    vu16 LieCheHaoA;           // A������
    vu8  LieCheHaoAString[5];  // A�����Ŷ�Ӧ���ַ���
    vu16 LieCheHaoB;           // B������
    vu8  LieCheHaoBString[5];  // B�����Ŷ�Ӧ���ַ���
    vu16 LieCheHaoC;           // C������
    vu8  LieCheHaoCString[5];  // C�����Ŷ�Ӧ���ַ���
    bool SystemStatus;         //ϵͳ����״̬ true:��ʾ�����ɹ�  false����ʾ����ʧ��
    bool CmdStatus;            //����״̬
    bool ListenerFlag;         //����
    vu16 DCUCurrentChe;        // DCU���ڵĵ�ǰ����1����
    vu16 KeyLedStatus;         //���̵�״̬
    vu8  KeyID;                //����ID
    vu8  KeyMan;               //����ID
    vu8  DCUMatser;            // DCU����   1,���أ�2���ӿ�
    vu8  COMMatser;            //��������   1,���أ�2���ӿ�
    vu8  Matser;               //����λ��   1,MC1�� 2��MC2
    vu8  PauseStatus;          //��ͣ״̬   1,����ͣ��2����ͣ
    vu8  MasterMode;           //����ģʽ
    vu8  GBMode;               //��վģʽ
    vu8  WorkMode;             //����ģʽ
    vu8  MonoVolume;           //��������
} SystemInfoTypeDef;

typedef struct
{
    //��ʱ��Ϣ
    vu8  TmpCmd[8];
    vu8  TmpStartStation;   //���վ
    vu8  TmpEndStation;     //�յ�վ
    vu8  TmpNowStation;     //��ǰվ
    vu8  TmpNextStation;    //��һվ
    vu8  TmpSpeciaStation;  //�����յ�
    vu16 TmpLineNo;         //��·��
    vu8  TmpShangXiaxing;   //�����б�־λ
    vu8  TmpYueZhan;        //Խս��Ϣ
    vu8  TmpMan_Volume;     //�˹�����
    vu8  TmpMP3_Volume;     // MP3����(�Զ��㲥)
    vu8  TmpRadio_Volume;   //��������
    vu8  TmpTalk_Volume;    //�Խ�����
    u8   Pages;             //��ʾҳ��
    u8   TmpPages;
    vu8  TmpLieCheID;             //��ǰ�г�λ��
    vu8  TmpMasterMode;           //����ģʽ
    vu8  TmpGBMode;               //��վģʽ
    vu8  TmpWorkMode;             //����ģʽ
    vu8  TmpMonoVolume;           //��������
    u8   LineInfoCmdSta;          //��·���������״̬
    vu8  YunZhanCmdSta;           //Խս�����״̬
    vu8  CurrentHostCmdSta;       // DCU��������״̬
    vu8  FirstCurrentHostCmdSta;  //�ϵ��ѯ��ǰ�г���
    vu8  ZhuBeiCmdSta;            //��������״̬
    vu8  FirstZhuBeiCmdSta;       //�ϵ��ѯ
    vu8  StartStopSta;            //��ͣ�㲥
    vu8  QueryVoiceCmdSta;        //��ѯ����
    vu8  FirstQueryVoiceCmdSta;   //�ϵ��ѯ
    vu8  SetVoiceCmdSta;          //���ý����㲥
    vu8  SetVolumeCmdSta;         //��������
    vu8  KEYCurrentSelect;        // ��ǰkey��ѡ��
    vu8  KEYSelect;               // key��ѡ��
    vu8  DriverTalk;              //˾���Խ� 0:����;1:����;2:��ͨ
    vu8  AlarmTalk;               //�����Խ� 0:����;1:����;2:��ͨ
    bool AlarmStatus;             //��������״̬��false������������  true������������
    vu8  TIM2Flag;                // 1:TIM2���У�0:TIM2ֹͣ
    char passwad[10];
} RunTypeDef;

/**********************
ϵͳ��Ϣ�ṹ��
******************************************/
typedef struct
{
    SystemInfoTypeDef SystemInfo;
    VolumeTypeDef     Volume;
    ScreenTypeDef     Screen;
    RunTypeDef        Run;
} SystemTypeDef;

void Systick_Configuration(void);
void GPIO_Configuration(void);
void Bsp_Init(void);
void ManualChannel(void);
void TalkChannel(void);
void NoChannel(void);
void CLR_MANGB(void);

extern SystemTypeDef        System;
extern const unsigned char *CHzm[];
extern const unsigned char *Number[];
extern const unsigned char *CHinf[];
extern const unsigned char  MAXSTA;
extern const unsigned char *Directioninf[];
#endif

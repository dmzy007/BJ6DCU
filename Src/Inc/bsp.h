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

//人工广播状态输入
#define MANGB_PIN GPIO_Pin_11
#define MANGB_PORT GPIOA
#define MANGB_in (GPIO_ReadInputDataBit(MANGB_PORT, MANGB_PIN))

//输出：AMC1,清除人工广播
#define AMC1_PIN GPIO_Pin_10
#define AMC1_PORT GPIOC
#define AMC1_OUTPUT_H() GPIO_SetBits(AMC1_PORT, AMC1_PIN)
#define AMC1_OUTPUT_L() GPIO_ResetBits(AMC1_PORT, AMC1_PIN)

//输出：清看门狗
#define WDI_PIN GPIO_Pin_12
#define WDI_PORT GPIOA
#define WDI_CLR() WDI_PORT->ODR ^= WDI_PIN

#define HeadID 0x10
#define TailID 0x28
/*******************************************************
 音量结构体
 ***********************************************************/
typedef struct
{
    vu16 Man_Volume;    //人工音量
    vu16 Talk_Volume;   //对讲讲音量
    vu16 MP3_Volume;    // MP3音量(自动广播)
    vu16 Radio_Volume;  //无线音量
    vu16 JT_Volume;     //对讲听音量
} VolumeTypeDef;

typedef struct
{
    vu8 MainScreen;
    vu8 FuScreen;
} ScreenTypeDef;

typedef struct
{
    vu8  StartStation;           //起点站
    vu8  EndStation;             //终点站
    vu8  NowStation;             //当前站
    vu8  NextStation;            //下一站
    vu8  SpeciaStation;          //特殊终点
    vu8  LineNo;                 //线路号
    vu8  BroadcastMode;          //广播模式   00无广播，01人工广播，02无线，03自动，04紧急
    vu8  BroadcastType;          //广播类型   00无广播，01自动 02 半自动 03 手动
    vu8  ErrorCode;              //故障信息设备编码
    vu8  DoorStatus;             //门状态
    vu8  BroadcastStatus;        //广播状态
    vu8  ShangXiaxing;           //上下行标志位
    vu8  TMStatus;               // TMS状态
    vu8  LieCheBianZu;           //列车编组
    vu8  ChongLianNum;           //重连车的数量
    vu8  YueZhan[64];            //越战信息
    vu8  TotalYueZhan;           //越战总数
    vu8  LieCheAddress;          //列车头尾ID
    vu8  AlarmTotal;             //报警器总数
    vu8  BaoJingCheXiangID[12];  //报警车厢ID，包含列车号和位置
    vu8  BaoJingCheXiangIDString[12][9];
    vu8  LieCheID;             //当前列车位置
    vu16 LieCheHao;            //当前列车号
    vu8  LieCheHaoString[5];   //当前列车号对应的字符串
    vu16 LieCheHaoA;           // A车车号
    vu8  LieCheHaoAString[5];  // A车车号对应的字符串
    vu16 LieCheHaoB;           // B车车号
    vu8  LieCheHaoBString[5];  // B车车号对应的字符串
    vu16 LieCheHaoC;           // C车车号
    vu8  LieCheHaoCString[5];  // C车车号对应的字符串
    bool SystemStatus;         //系统启动状态 true:表示启动成功  false：表示启动失败
    bool CmdStatus;            //命令状态
    bool ListenerFlag;         //监听
    vu16 DCUCurrentChe;        // DCU所在的当前车的1车号
    vu16 KeyLedStatus;         //键盘灯状态
    vu8  KeyID;                //按键ID
    vu8  KeyMan;               //按键ID
    vu8  DCUMatser;            // DCU主控   1,主控；2，从控
    vu8  COMMatser;            //主机主控   1,主控；2，从控
    vu8  Matser;               //主机位置   1,MC1； 2，MC2
    vu8  PauseStatus;          //暂停状态   1,非暂停；2，暂停
    vu8  MasterMode;           //主控模式
    vu8  GBMode;               //报站模式
    vu8  WorkMode;             //工作模式
    vu8  MonoVolume;           //监听音量
} SystemInfoTypeDef;

typedef struct
{
    //临时信息
    vu8  TmpCmd[8];
    vu8  TmpStartStation;   //起点站
    vu8  TmpEndStation;     //终点站
    vu8  TmpNowStation;     //当前站
    vu8  TmpNextStation;    //下一站
    vu8  TmpSpeciaStation;  //特殊终点
    vu16 TmpLineNo;         //线路号
    vu8  TmpShangXiaxing;   //上下行标志位
    vu8  TmpYueZhan;        //越战信息
    vu8  TmpMan_Volume;     //人工音量
    vu8  TmpMP3_Volume;     // MP3音量(自动广播)
    vu8  TmpRadio_Volume;   //无线音量
    vu8  TmpTalk_Volume;    //对讲音量
    u8   Pages;             //显示页数
    u8   TmpPages;
    vu8  TmpLieCheID;             //当前列车位置
    vu8  TmpMasterMode;           //主控模式
    vu8  TmpGBMode;               //报站模式
    vu8  TmpWorkMode;             //工作模式
    vu8  TmpMonoVolume;           //监听音量
    u8   LineInfoCmdSta;          //线路设置命令发送状态
    vu8  YunZhanCmdSta;           //越战命令发送状态
    vu8  CurrentHostCmdSta;       // DCU主机命令状态
    vu8  FirstCurrentHostCmdSta;  //上电查询当前列车号
    vu8  ZhuBeiCmdSta;            //主备命令状态
    vu8  FirstZhuBeiCmdSta;       //上电查询
    vu8  StartStopSta;            //关停广播
    vu8  QueryVoiceCmdSta;        //查询音量
    vu8  FirstQueryVoiceCmdSta;   //上电查询
    vu8  SetVoiceCmdSta;          //设置紧急广播
    vu8  SetVolumeCmdSta;         //设置音量
    vu8  KEYCurrentSelect;        // 当前key键选择
    vu8  KEYSelect;               // key键选择
    vu8  DriverTalk;              //司机对讲 0:空闲;1:呼叫;2:接通
    vu8  AlarmTalk;               //报警对讲 0:空闲;1:呼叫;2:接通
    bool AlarmStatus;             //报警接听状态，false：表明呼叫中  true：表明接听中
    vu8  TIM2Flag;                // 1:TIM2运行，0:TIM2停止
    char passwad[10];
} RunTypeDef;

/**********************
系统信息结构体
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

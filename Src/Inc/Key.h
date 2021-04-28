#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

#if 1
//有键按下输入（PC7）
#define ZLG7289A_KEY_PIN GPIO_Pin_7
#define ZLG7289A_KEY_PORT GPIOC
#else
//有键按下输入（PC7）
#define ZLG7289A_KEY_PIN GPIO_Pin_11
#define ZLG7289A_KEY_PORT GPIOD
#endif
#define ZLG7289A_KEYin GPIO_ReadInputDataBit(ZLG7289A_KEY_PORT, ZLG7289A_KEY_PIN)

#if 1
//模拟SPI数据输入输出口为：ZLG7289A_DIO（PC6）
#define ZLG7289A_DIO_DIR_OUT() \
    GPIOC->CRL &= 0xf0ffffff;  \
    GPIOC->CRL |= 0x03000000;  //清掉这原来的设置，设置为推挽输出

#define ZLG7289A_DIO_DIR_IN() \
    GPIOC->CRL &= 0xf0fffFff; \
    GPIOC->CRL |= 0x08000000;  //清掉这原来的设置，设置为输入上拉
#else
//模拟SPI数据输入输出口为：ZLG7289A_DIO（PB15）
#define ZLG7289A_DIO_DIR_OUT() \
    GPIOB->CRH &= 0x0FFFFFFF;  \
    GPIOB->CRH |=              \
        0x30000000;  //													//清掉这原来的设置，设置为推挽输出

#define ZLG7289A_DIO_DIR_IN() \
    GPIOB->CRH &= 0x0FFFFFFF; \
    GPIOB->CRH |= 0x40000000;
#endif

#if 1
#define ZLG7289A_DIO_PIN GPIO_Pin_6
#define ZLG7289A_DIO_PORT GPIOC
#else
#define ZLG7289A_DIO_PIN GPIO_Pin_15
#define ZLG7289A_DIO_PORT GPIOB
#endif
#define ZLG7289A_DIOin GPIO_ReadInputDataBit(ZLG7289A_DIO_PORT, ZLG7289A_DIO_PIN)
#define ZLG7289A_DIO_OUTPUT_H() GPIO_SetBits(ZLG7289A_DIO_PORT, ZLG7289A_DIO_PIN)
#define ZLG7289A_DIO_OUTPUT_L() GPIO_ResetBits(ZLG7289A_DIO_PORT, ZLG7289A_DIO_PIN)

#if 1
#define ZLG7289A_RST_PIN GPIO_Pin_10
#define ZLG7289A_RST_PORT GPIOD
#else
#define ZLG7289A_RST_PIN GPIO_Pin_13
#define ZLG7289A_RST_PORT GPIOD
#endif
#define ZLG7289A_RST_OUTPUT_H() GPIO_SetBits(ZLG7289A_RST_PORT, ZLG7289A_RST_PIN)
#define ZLG7289A_RST_OUTPUT_L() GPIO_ResetBits(ZLG7289A_RST_PORT, ZLG7289A_RST_PIN)

#if 1
#define ZLG7289A_CS_PIN GPIO_Pin_14
#define ZLG7289A_CS_PORT GPIOD
#else
#define ZLG7289A_CS_PIN GPIO_Pin_12
#define ZLG7289A_CS_PORT GPIOB
#endif
#define ZLG7289A_CS_OUTPUT_H() GPIO_SetBits(ZLG7289A_CS_PORT, ZLG7289A_CS_PIN)
#define ZLG7289A_CS_OUTPUT_L() GPIO_ResetBits(ZLG7289A_CS_PORT, ZLG7289A_CS_PIN)

#if 1
#define ZLG7289A_CLK_PIN GPIO_Pin_12
#define ZLG7289A_CLK_PORT GPIOD
#else
#define ZLG7289A_CLK_PIN GPIO_Pin_13
#define ZLG7289A_CLK_PORT GPIOB
#endif
#define ZLG7289A_CLK_OUTPUT_H() GPIO_SetBits(ZLG7289A_CLK_PORT, ZLG7289A_CLK_PIN)
#define ZLG7289A_CLK_OUTPUT_L() GPIO_ResetBits(ZLG7289A_CLK_PORT, ZLG7289A_CLK_PIN)

#define KEY1LED (1 << 0)  //自动按键灯
#define KEY2LED (1 << 1)  //开始按键灯
#define KEY3LED (1 << 2)  //对讲按键灯
#define KEY4LED (1 << 3)  //停止按键灯
#define KEY5LED (1 << 4)  //报警按键灯
#define KEY6LED (1 << 5)  //暂停按键灯
#define KEY7LED (1 << 6)  //复位按键灯
#define KEY8LED (1 << 7)  //主机按键灯
#define KEY9LED (1 << 8)  //主控按键灯
#define KEY10LED (1 << 9)
#define KEY11LED (1 << 10)  //通讯LED
#define KEY12LED (1 << 11)  //广播LED
#define KEY13LED (1 << 12)  // OCC LED
#define KEY14LED (1 << 13)
#define KEY15LED (1 << 14)
#define KEY16LED (1 << 15)
#define ALLKEYLEDOFF 0x0000

void KeyLed_IO_Config(void);
void ZLG7289_SPI_Write(char dat);
void ScanKey(void);
void KeyRefeshLed(vu16 Status);
void Keyfunction(u8 KeyID);
void KeepInterCom(void);
void CancelInterCom(void);

void ShowNum(u8 Num);
void Up(void);
void Down(void);
void Cancel(void);
void OK(void);
#endif

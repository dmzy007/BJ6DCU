#ifndef __3812_H_
#define __3812_H_
#include "stm32f10x.h"

#define DATA_3812_H    GPIO_SetBits(GPIOC, GPIO_Pin_12)
#define DATA_3812_L    GPIO_ResetBits(GPIOC, GPIO_Pin_12)

#define CLK_3812_H    GPIO_SetBits(GPIOC, GPIO_Pin_11)
#define CLK_3812_L    GPIO_ResetBits(GPIOC, GPIO_Pin_11)

#define  ChxVolume     0
#define  ChxGain       1
#define  Gain          0

void delay_us(long data);
void delay_ms(u32 data);
void DB3812WriteVolume(unsigned char ch1volume, unsigned char ch2volume, unsigned char adrs);
void initbd3812(void );

#endif

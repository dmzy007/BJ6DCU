#ifndef __LCD_H
#define __LCD_H

#include "stm32f10x.h"

#define Dis_X_MAX (128 - 1)
#define Dis_Y_MAX (64 - 1)

#define SPI_SS_H() GPIO_SetBits(GPIOA, GPIO_Pin_4)
#define SPI_SS_L() GPIO_ResetBits(GPIOA, GPIO_Pin_4)

#define RESET_H() GPIO_SetBits(GPIOC, GPIO_Pin_4)
#define RESET_L() GPIO_ResetBits(GPIOC, GPIO_Pin_4)

#define Busy() GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5)

void _delay_us(u32 us);
void _delay_ms(u32 ms);
void LCD_IO_Config(void);
void LCD_init(void);
void Images(unsigned char s_x, unsigned char s_p, unsigned char x_width, unsigned char p_width, const unsigned char *p, unsigned int length);
void LCD_PutChar(unsigned short x, unsigned short y, char c);
void LCD_PutCHString(unsigned short x, unsigned short y, const unsigned char *s, unsigned char Mode);

void clear(unsigned short sx, unsigned short sy, unsigned short ex, unsigned short ey);

void LCD_DIS_MAIN(void);
void LCD_DIS_EmerGB(u8 index);
void LCD_DIS_Setting(u8 index);
#endif

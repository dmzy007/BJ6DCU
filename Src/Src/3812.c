
/****************************************Copyright (c)****************************************************
**
**
**
****--------------File Info---------------------------------------------------------------------------------
** File name:
** Descriptions:
**
****--------------------------------------------------------------------------------------------------------
** Created by:              wkl
** Created date:            2013-05-20
** Version:                 v1.0
** Descriptions:            The original version
**
****--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Descriptions:
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "3812.h"
extern void _delay_ms(u32 us);
/**************************************************************/
char Mp3Volume          = 0;
u8   Mp3Volume_Tabl[10] = {36, 20, 17, 14, 13, 11, 7, 5, 3, 1};

/*****************************
 * Function Name  : bd3812writevolume
 * Description    : 写数据
 * Input          : ch1volume  ch2volume  音量值大小调整
 * Output         : None
 * Return         : None
 * Attention		 : None

 ***************************/
void DB3812WriteVolume(unsigned char ch1volume, unsigned char ch2volume, unsigned char adrs)
{
    unsigned char i, temp, ch1volumetemp = 0, ch2volumetemp = 0;
    if ((adrs & 0x01) == 0)  //写音量(先查表)
    {
        ch1volumetemp = (10 - ch1volume);
        ch2volumetemp = (10 - ch2volume);
    }
    else  //写增益
    {
        ch1volumetemp = ch1volume;
        ch2volumetemp = ch2volume;
    }

    temp = ch2volumetemp << 1;
    for (i = 0; i < 7; i++)
    {
        DATA_3812_L;
        delay_us(10);
        CLK_3812_L;
        delay_us(10);
        if (temp & 0x80)
        {
            DATA_3812_H;
        }
        else
        {
            DATA_3812_L;
        }
        delay_us(10);
        CLK_3812_H;
        temp = (temp << 1);
        delay_us(10);
    }

    temp = ch1volumetemp << 1;
    for (i = 0; i < 7; i++)
    {
        DATA_3812_L;
        delay_us(10);
        CLK_3812_L;
        delay_us(10);
        if (temp & 0x80)
        {
            DATA_3812_H;
        }
        else
        {
            DATA_3812_L;
        }
        delay_us(10);
        CLK_3812_H;
        temp = (temp << 1);
        delay_us(10);
    }

    temp = (adrs << 5);
    for (i = 0; i < 3; i++)
    {
        DATA_3812_L;
        delay_us(10);
        CLK_3812_L;
        delay_us(10);
        if (temp & 0x80)
        {
            DATA_3812_H;
        }
        else
        {
            DATA_3812_L;
        }
        delay_us(10);
        CLK_3812_H;
        temp = (temp << 1);
        delay_us(10);
    }

    DATA_3812_H;
    delay_us(10);
    CLK_3812_L;
    delay_us(10);
    DATA_3812_L;
    delay_us(10);
}

void initbd3812(void)
{
    DATA_3812_L;
    CLK_3812_L;
    _delay_ms(20);

    DB3812WriteVolume(Mp3Volume, Mp3Volume, ChxVolume);  //写通
    _delay_ms(20);
    DB3812WriteVolume(Gain, 0, ChxGain);  //写增益
    _delay_ms(20);
}

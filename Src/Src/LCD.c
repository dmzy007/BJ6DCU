#include "LCD.h"
#include "LCDDATA.h"
#include "bsp.h"
#include "string.h"

extern SystemTypeDef System;

unsigned char X_Witch    = 6;
unsigned char Y_Witch    = 10;
unsigned char X_Witch_cn = 16;
unsigned char Y_Witch_cn = 16;
unsigned char Dis_Zero   = 0;

void _delay_us(u32 us)
{
    unsigned char i, j;
    for (i = 0; i < us; i++)
    {
        for (j = 0; j < 8; j++)
        {
            ;
        }
    }
}

void _delay_ms(u32 ms)
{
    u16 j, k;
    while (ms--)
    {
        for (j = 99; j > 0; j--)
        {
            for (k = 720; k > 0; k--)
                ;
        }
    }
}

//引脚配置
void LCD_IO_Config(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 配置LCD SPI的 CS引脚，普通IO即可 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 配置LCD SPI的 SCK引脚*/
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 配置LCD SPI的 MISO引脚*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 配置LCD SPI的 MOSI引脚*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 配置LCD SPI的 RST引脚，普通IO即可 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* 配置LCD SPI的 BUSY引脚，普通IO即可 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* 停止信号 FLASH: CS引脚高电平*/
    SPI_SS_H();

    /* SPI 模式配置 */
    SPI_InitStructure.SPI_Direction         = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial     = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    /* 使能 SPI  */
    SPI_Cmd(SPI1, ENABLE);
}

//========================================================================
// 函数: void SPI_SSSet(unsigned char Status)
// 描述: 置SS线状态
// 参数: Status   =1则置高电平，=0则置低电平
// 返回: 无
// 版本:
//      2007/07/17      First version
//		2007/07/24		V1.2 for MCS51 Keil C
//========================================================================
void SPI_SSSet(unsigned int Status)
{
    while (Busy())
        ;  //稍稍延一点时间
    _delay_us(5);
    if (Status)  //判断是要置SS为低还是高电平？
    {            // SS置高电平
        SPI_SS_H();
    }
    else  // SS置低电平
    {
        SPI_SS_L();
    }
}

//========================================================================
// 函数: void SPI_Send(unsigned char Data)
// 描述: 通过串行SPI口输送一个byte的数据置模组
// 参数: Data 要传送的数据
// 返回: 无
// 版本:
//      2007/07/17      First version
//		2007/07/24		V1.2 for MCS51 Keil C
//========================================================================
void SPI_Send(unsigned int Data)
{
    /* 等待发送缓冲区为空，TXE事件 */
    while (Busy())
        ;
    _delay_us(5);

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
        ;

    /* 写入数据寄存器，把要写入的数据写入发送缓冲区 */
    SPI_I2S_SendData(SPI1, Data);
}

//========================================================================
// 函数: void FontSet(unsigned char Font_NUM,unsigned char Color)
// 描述: ASCII字符字体设置
// 参数: Font_NUM 字体选择,以驱动所带的字库为准
//		 Color  文本颜色,仅作用于ASCII字库
// 返回: 无
// 备注:
// 版本:
//      2007/07/19      First version
//========================================================================
void FontSet(unsigned char Font_NUM, unsigned char Color)
{
    unsigned char ucTemp = 0;
    if (Font_NUM)
    {
        X_Witch = 6;
        Y_Witch = 10;
    }
    else
    {
        X_Witch = 8;
        Y_Witch = 16;
    }
    ucTemp = (Font_NUM << 4) | Color;
    //设置ASCII字符的字型
    SPI_SSSet(0);      // SS置低电平
    SPI_Send(0x81);    //传送指令0x81
    SPI_Send(ucTemp);  //选择8X16的ASCII字体,字符色为黑色
    SPI_SSSet(1);      //完成操作置SS高电平
}

//========================================================================
// 函数: void FontSet_cn(unsigned char Font_NUM,unsigned char Color)
// 描述: 汉字库字符字体设置
// 参数: Font_NUM 字体选择,以驱动所带的字库为准
//		 Color  文本颜色,仅作用于汉字库
// 返回: 无
// 备注:
// 版本:
//      2007/07/19      First version
//========================================================================
void FontSet_cn(unsigned char Font_NUM, unsigned char Color)
{
    unsigned char ucTemp = 0;
    if (Font_NUM)
    {
        X_Witch_cn = 12;
        Y_Witch_cn = 12;
    }
    else
    {
        X_Witch_cn = 16;
        Y_Witch_cn = 16;
    }
    ucTemp = (Font_NUM << 4) | Color;
    //设置ASCII字符的字型
    SPI_SSSet(0);      // SS置低电平
    SPI_Send(0x82);    //传送指令0x81
    SPI_Send(ucTemp);  //选择8X16的ASCII字体,字符色为黑色
    SPI_SSSet(1);      //完成操作置SS高电平
}

//========================================================================
// 函数: void PutChar(unsigned char x,unsigned char y,unsigned char a)
// 描述: 写入一个标准ASCII字符
// 参数: x  X轴坐标     y  Y轴坐标
//		 a  要显示的字符在字库中的偏移量
// 返回: 无
// 备注: ASCII字符可直接输入ASCII码即可
// 版本:
//      2007/07/19      First version
//========================================================================
void PutChar(unsigned char x, unsigned char y, unsigned char a)
{
    //显示ASCII字符
    SPI_SSSet(0);  // SS置低电平
    SPI_Send(7);   //传送指令0x07
    SPI_Send(x);   //要显示字符的左上角的X轴位置
    SPI_Send(y);   //要显示字符的左上角的Y轴位置
    SPI_Send(a);   //要显示字符ASCII字符的ASCII码值
    SPI_SSSet(1);  //完成操作置SS高电平
}

//========================================================================
// 函数: void PutString(int x,int y,char *p)
// 描述: 在x、y为起始坐标处写入一串标准ASCII字符
// 参数: x  X轴坐标     y  Y轴坐标
//		 p  要显示的字符串
// 返回: 无
// 备注: 仅能用于自带的ASCII字符串显示
// 版本:
//      2007/07/19      First version
//========================================================================
void PutString(unsigned char x, unsigned char y, unsigned char *p)
{
    while (*p != 0)
    {
        PutChar(x, y, *p);
        x += X_Witch;
        if ((x + X_Witch) > Dis_X_MAX)
        {
            x = Dis_Zero;
            if ((Dis_Y_MAX - y) < Y_Witch)
                break;
            else
                y += Y_Witch;
        }
        p++;
    }
}

//========================================================================
// 函数: void PutChar_cn(unsigned char x,unsigned char y,unsigned short * GB)
// 描述: 写入一个二级汉字库汉字
// 参数: x  X轴坐标     y  Y轴坐标
//		 a  GB码
// 返回: 无
// 备注:
// 版本:
//      2007/07/19      First version
//		2007/07/24		V1.2 for MCS51 Keil C
//========================================================================
void PutChar_cn(unsigned char x, unsigned char y, const unsigned char *GB)
{
    //显示ASCII字符
    SPI_SSSet(0);  // SS置低电平
    SPI_Send(8);   //传送指令0x08
    SPI_Send(x);   //要显示字符的左上角的X轴位置
    SPI_Send(y);   //要显示字符的左上角的Y轴位置

    SPI_Send(*(GB++));  //传送二级字库中汉字GB码的高八位值
    SPI_Send(*GB);      //传送二级字库中汉字GB码的低八位值
    SPI_SSSet(1);       //完成操作置SS高电平
}

//========================================================================
// 函数: void PutString_cn(unsigned char x,unsigned char y,unsigned char *p)
// 描述: 在x、y为起始坐标处写入一串汉字字符
// 参数: x  X轴坐标     y  Y轴坐标
//		 p  要显示的字符串
// 返回: 无
// 备注: 同PutChar_cn中的解释
// 版本:
//      2007/07/19      First version
//		2007/07/24		V1.2 for MCS51 Keil C
//========================================================================
void PutString_cn(unsigned char x, unsigned char y, const unsigned char *p)
{
    while (*p != 0)
    {
        if (*p < 128)
        {
            PutChar(x, y, *p);
            x += X_Witch;
            if ((x /* + X_Witch*/) > Dis_X_MAX)
            {
                x = Dis_Zero;
                if ((Dis_Y_MAX - y) < Y_Witch)
                    break;
                else
                    y += Y_Witch_cn;
            }
            p += 1;
        }
        else
        {
            PutChar_cn(x, y, p);
            //			x += X_Witch_cn+1;						//字符间隔加1
            x += X_Witch_cn;
            if ((x /* + X_Witch_cn*/) > Dis_X_MAX)
            {
                x = Dis_Zero;
                if ((Dis_Y_MAX - y) < Y_Witch_cn)
                    break;
                else
                    y += Y_Witch_cn;
            }
            p += 2;
        }
    }
}

//========================================================================
// 函数: void SetPaintMode(unsigned char Mode,unsigned char Color)
// 描述: 绘图模式设置
// 参数: Mode 绘图模式    Color  像素点的颜色,相当于前景色
// 返回: 无
// 备注: Mode无效
// 版本:
//      2007/07/19      First version
//========================================================================
void SetPaintMode(unsigned char Mode, unsigned char Color)
{
    unsigned char ucTemp = 0;
    ucTemp               = (Mode << 4) | Color;
    //设置绘图模式
    SPI_SSSet(0);      // SS置低电平
    SPI_Send(0x83);    //传送指令0x83
    SPI_Send(ucTemp);  //选择8X16的ASCII字体,字符色为黑色
    SPI_SSSet(1);      //完成操作置SS高电平
}

//========================================================================
// 函数: void PutPixel(unsigned char x,unsigned char y)
// 描述: 在x、y点上绘制一个前景色的点
// 参数: x  X轴坐标     y  Y轴坐标
// 返回: 无
// 备注: 使用前景色
// 版本:
//      2007/07/19      First version
//========================================================================
void PutPixel(unsigned char x, unsigned char y)
{
    //绘点操作
    SPI_SSSet(0);  // SS置低电平
    SPI_Send(1);   //送指令0x01
    SPI_Send(x);   //送第一个数据,即设置点的X轴位置
    SPI_Send(y);   //点的Y轴位置
    SPI_SSSet(1);  //完成操作置SS高电平
}

//========================================================================
// 函数: void Line(unsigned char s_x,unsigned char  s_y,
//					unsigned char  e_x,unsigned char  e_y)
// 描述: 在s_x、s_y为起始坐标，e_x、e_y为结束坐标绘制一条直线
// 参数: x  X轴坐标     y  Y轴坐标
// 返回: 无
// 备注: 使用前景色
// 版本:
//      2007/07/19      First version
//========================================================================
void Line(unsigned char s_x, unsigned char s_y, unsigned char e_x, unsigned char e_y)
{
    //绘制直线
    SPI_SSSet(0);   // SS置低电平
    SPI_Send(2);    //送指令0x02
    SPI_Send(s_x);  //起点X轴坐标
    SPI_Send(s_y);  //起点Y轴坐标
    SPI_Send(e_x);  //终点X轴坐标
    SPI_Send(e_y);  //终点Y轴坐标
    SPI_SSSet(1);   //完成操作置SS高电平
}

//========================================================================
// 函数: void Circle(unsigned char x,unsigned char y,
//					unsigned char r,unsigned char mode)
// 描述: 以x,y为圆心R为半径画一个圆(mode = 0) or 圆面(mode = 1)
// 参数:
// 返回: 无
// 备注: 画圆函数执行较慢，如果MCU有看门狗，请作好清狗的操作
// 版本:
//      2007/07/19      First version
//========================================================================
void Circle(unsigned char x, unsigned char y, unsigned char r, unsigned char mode)
{
    SPI_SSSet(0);
    if (mode)
        SPI_Send(6);
    else
        SPI_Send(5);
    SPI_Send(x);
    SPI_Send(y);
    SPI_Send(r);
    SPI_SSSet(1);
}

//========================================================================
// 函数: void Rectangle(unsigned char left, unsigned char top, unsigned char right,
//				 unsigned char bottom, unsigned char mode)
// 描述: 以x,y为圆心R为半径画一个圆(mode = 0) or 圆面(mode = 1)
// 参数: left - 矩形的左上角横坐标，范围0到126
//		 top - 矩形的左上角纵坐标，范围0到62
//		 right - 矩形的右下角横坐标，范围1到127
//		 bottom - 矩形的右下角纵坐标，范围1到63
//		 Mode - 绘制模式，可以是下列数值之一：
//				0:	矩形框（空心矩形）
//				1:	矩形面（实心矩形）
// 返回: 无
// 备注: 画圆函数执行较慢，如果MCU有看门狗，请作好清狗的操作
// 版本:
//      2007/07/19      First version
//========================================================================
void Rectangle(unsigned char left, unsigned char top, unsigned char right, unsigned char bottom, unsigned char mode)
{
    SPI_SSSet(0);
    if (mode)
        SPI_Send(4);
    else
        SPI_Send(3);
    SPI_Send(left);
    SPI_Send(top);
    SPI_Send(right);
    SPI_Send(bottom);
    SPI_SSSet(1);
}

//========================================================================
// 函数: void Images(unsigned char s_x, unsigned char s_p, unsigned char x_width,
//                  unsigned char p_width, unsigned char *p, unsigned int length)
// 描述: 以图片的方式显示中文字体
// 参数: s_x - 要填充的起始位置的 X 轴坐标
//       s_p - 要填充的起始位置的页数(Page) ，范围0到7
//		 x_width - 矩形的右下角横坐标，范围1到127
//		 p_width - 位图图像 Y 轴方向的页数(Page)
//		 p - 显示数据数组
//		 length - 显示数据数组长度，不得超过512
// 返回: 无
// 版本:
//      2007/07/19      First version
//========================================================================
void Images(unsigned char s_x, unsigned char s_p, unsigned char x_width, unsigned char p_width, const unsigned char *p, unsigned int length)
{
    unsigned int  i = 0, j = 0;
    unsigned char array[480] = {0};

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 120; j++)
        {
            if (i == 0)
            {
                array[j] = (p[j] & 0x0F) * 16;
            }
            else if ((i > 0) && (i < 3))
            {
                array[i * 120 + j] = (p[i * 120 + j] & 0x0F) * 16 + (p[(i - 1) * 120 + j] & 0xF0) / 16;
            }
            else
            {
                array[i * 120 + j] = (p[(i - 1) * 120 + j] & 0xF0) / 16;
            }
        }
    }

    length = 480;

    SPI_SSSet(0);
    SPI_Send(9);
    SPI_Send(s_x);
    SPI_Send(s_p);
    SPI_Send(x_width);
    SPI_Send(p_width);
    for (i = 0; i < ((length > 512) ? 512 : length); i++)
    {
        // SPI_Send(p[i]);
        SPI_Send(array[i]);
    }

    SPI_SSSet(1);
}
//========================================================================
// 函数: void ClrScreen(void)
// 描述: 清屏函数，执行全屏幕清除或填充前景色
// 参数:
// 返回: 无
// 备注:
// 版本:
//      2007/07/19      First version
//========================================================================
void ClrScreen(void)
{
    //清屏操作
    SPI_SSSet(0);    // SS置低电平
    SPI_Send(0x80);  //送指令0x80
    SPI_Send(0);     //指令数据
    SPI_SSSet(1);    //完成操作置SS高电平
}

//=====================================================================
// 函数: void FontMode(unsigned char Cover,unsigned char Color)
// 描述: 字符覆盖模式设置
// 参数:  Cover 0 字符显示时不将字符区域的背景擦除
//			    1 字符显示时将要显示的字符区域的背景擦除
//		 Color  字符覆盖模式(Cover)为1时有效，即使用黑或白进行字符区域背景填充
//				0/1
// 返回: 无
// 备注:
// 版本:
//      2009/01/19      First version
//=====================================================================
void FontMode(unsigned char Cover, unsigned char Color)
{
    unsigned char ucTemp = 0;
    ucTemp               = (Cover << 4) | Color;
    SPI_SSSet(0);      // SS置低电平
    SPI_Send(0x89);    //传送指令0x89
    SPI_Send(ucTemp);  //传送指令数据
    SPI_SSSet(1);      //完成操作置SS高电平
}

//====================================================================
// 函数: void Show_Char(unsigned char x,unsigned char y,unsigned char a,unsigned char type)
// 描述: 在x、y的位置为显示区域的左上角，显示一个unsigned char型数
// 参数: x X轴坐标  y Y轴坐标  a 要显示的8位宽度的数据  type 显示特性(0,1,2)
// 返回: 无
// 备注:
// 版本:
//      2008/11/27      First version
//====================================================================
void Show_Char(unsigned char x, unsigned char y, unsigned char a, unsigned char type)
{
    SPI_SSSet(0);    // SS置低电平
    SPI_Send(11);    //传送指令11
    SPI_Send(x);     //要显示数字的左上角的X轴位置
    SPI_Send(y);     //要显示数字的左上角的Y轴位置
    SPI_Send(type);  //要显示数字的显示特性
    SPI_Send(a);
    SPI_SSSet(1);  //完成操作置SS高电平
}

//====================================================================
// 函数: void Show_Short(unsigned char x,unsigned char y,unsigned short a,unsigned char type)
// 描述: 在x、y的位置为显示区域的左上角，显示一个unsigned short型数（16位宽度）
// 参数: x X轴坐标    y Y轴坐标   a 要显示的16位宽度的数据  type 显示特性(0,1,2)
// 返回: 无
// 备注:
// 版本:
//      2008/11/27      First version
//====================================================================
void Show_Short(unsigned char x, unsigned char y, unsigned short a, unsigned char type)
{
    SPI_SSSet(0);    // SS置低电平
    SPI_Send(12);    //传送指令12
    SPI_Send(x);     //要显示数字的左上角的X轴位置
    SPI_Send(y);     //要显示数字的左上角的Y轴位置
    SPI_Send(type);  //要显示数字的显示特性
    SPI_Send((unsigned char)(a >> 8));
    SPI_Send((unsigned char)a);
    SPI_SSSet(1);  //完成操作置SS高电平
}

//=====================================================================
// 函数: void SetLCDGra(unsigned char Dat)
// 描述: LCD整屏显示灰度调节
// 参数:  Dat   要调成的灰度值,0~0x3f
//
// 返回: 无
// 备注: 调节后的灰度将会保持到下一将从 LCD复位之前,或者下一次重新调整灰度之前
// 版本:
//      2009/05/10      First version
//=====================================================================
void SetLCDGra(unsigned char Dat)
{
    //显示ASCII字符
    SPI_SSSet(0);    // SS置低电平
    SPI_Send(0x8a);  //传送指令0x8a
    SPI_Send(Dat);   //传送数值
    SPI_SSSet(1);    //完成操作置SS高电平
}

//=====================================================================
// 函数: void SetLCDSleep(void)
// 描述: 设置模块进入低功耗模式
// 参数:  Dat   0xaa 进入低功耗模式后,屏上保留显示
//				0x55 进入低功耗模式后,屏上关闭显示
//
// 返回: 无
// 备注: 模块将会在进入低功耗模式后,SS引脚上的电平变化后恢复正常工作模式
// 版本:
//      2009/05/10      First version
//=====================================================================
void SetLCDSleep(unsigned char Dat)
{
    SPI_SSSet(0);    // SS置低电平
    SPI_Send(0x8b);  //传送指令0x8b
    SPI_Send(Dat);   //传送数值
    SPI_SSSet(1);    //完成操作置SS高电平
    _delay_us(30);
}

void CloseLCD(unsigned char Dat)
{
    SPI_SSSet(0);    // SS置低电平
    SPI_Send(0x88);  //传送指令0x8b
    SPI_Send(Dat);   //传送数值
    SPI_SSSet(1);    //完成操作置SS高电平
    _delay_us(30);
}

//=====================================================================
// 函数: void SetLCDWakeup(void)
// 描述: 设置模块退出低功耗模式
// 参数:  无
//
// 返回: 无
// 备注: 模块将会在进入低功耗模式后,SS引脚上的电平变化后恢复正常工作模式
// 版本:
//      2009/05/10      First version
//=====================================================================
void SetLCDWakeup(void)
{
    SPI_SSSet(0);  // SS置低电平
    _delay_us(100);
    SPI_SSSet(1);  //完成操作置SS高电平
}

/**** 清除 ****/
// X[0,192],Y[0,8]
void clear(unsigned short sx, unsigned short sy, unsigned short ex, unsigned short ey)
{
    FontMode(1, 0);  //设置字符覆盖模式为使能，字符背景覆盖色为0
    FontSet_cn(0, 1);
    FontSet(0, 1);
    SetPaintMode(1, 0);

    Rectangle(sx, sy, ex, ey, 1);
}

//液晶屏初始化
void LCD_init(void)
{
    RESET_L();
    _delay_ms(10);
    RESET_H();

    ClrScreen();
    SetLCDGra(30);

    FontMode(1, 0);  //设置字符覆盖模式为使能，字符背景覆盖色为0
    FontSet_cn(0, 1);
    FontSet(0, 1);
    SetPaintMode(1, 0);

    Rectangle(0, 0, 127, 63, 1);
}

//写入一字符
void LCD_PutChar(unsigned short x, unsigned short y, char c)
{
    FontMode(1, 0);  //设置字符覆盖模式为使能，字符背景覆盖色为0
    FontSet_cn(0, 1);
    FontSet(0, 1);
    SetPaintMode(1, 0);

    PutChar(x, y, c);
}

//静态显示中文字符串
void LCD_PutCHString(unsigned short x, unsigned short y, const unsigned char *s, unsigned char Mode)
{
    if (Mode)
    {
        FontMode(1, 1);  //设置字符覆盖模式为使能，字符背景覆盖色为0
        FontSet_cn(0, 0);
        FontSet(0, 0);
    }
    else
    {
        FontMode(1, 0);  //设置字符覆盖模式为使能，字符背景覆盖色为0
        FontSet_cn(0, 1);
        FontSet(0, 1);
    }
    SetPaintMode(1, 0);

    PutString_cn(x, y, s);
}

void LCD_DIS_MAIN(void)
{
    /*第一行显示*/
    LCD_PutCHString(0, 0, Masterinf[System.SystemInfo.COMMatser], 0);

    LCD_PutCHString(108, 0, " ", 1);
    LCD_PutCHString(112, 0, Addressinf[(System.SystemInfo.LieCheAddress == HeadID) ? 1 : 2], 1);
    /*第二行显示*/
    Images(4, 2, 120, 4, CHzm[System.SystemInfo.NowStation], 360);

    /*第三行显示*/
    LCD_PutCHString(0, 48, Directioninf[System.SystemInfo.ShangXiaxing], 0);
    LCD_PutCHString(64, 48, Number[System.SystemInfo.StartStation], 0);
    LCD_PutCHString(88, 48, Number[System.SystemInfo.NowStation], 0);
    LCD_PutCHString(112, 48, Number[System.SystemInfo.EndStation], 0);
}

void LCD_DIS_EmerGB(u8 index)
{
    index = (1 << (index - 1));

    LCD_PutCHString(8, 0, "01", 0);
    LCD_PutCHString(56, 0, "临停1分", (index & 0x01) ? 1 : 0);
    LCD_PutCHString(8, 16, "02", 0);
    LCD_PutCHString(56, 16, "临停3分", (index & 0x02) ? 1 : 0);
    LCD_PutCHString(8, 32, "03", 0);
    LCD_PutCHString(56, 32, "故障临停", (index & 0x04) ? 1 : 0);
    LCD_PutCHString(8, 48, "04", 0);
    LCD_PutCHString(56, 48, "故障清人", (index & 0x08) ? 1 : 0);
}

void LCD_DIS_Setting(u8 index)
{
    index = (1 << (index - 1));

    LCD_PutCHString(8, 0, "主控模式", (index & 0x01) ? 1 : 0);
    LCD_PutCHString(104, 0, Number[System.Run.TmpMasterMode], 0);
    LCD_PutCHString(8, 16, "报站模式", (index & 0x02) ? 1 : 0);
    LCD_PutCHString(104, 16, Number[System.Run.TmpGBMode], 0);
    LCD_PutCHString(8, 32, "工作模式", (index & 0x04) ? 1 : 0);
    LCD_PutCHString(104, 32, Number[System.Run.TmpWorkMode], 0);
    LCD_PutCHString(8, 48, "监听音量", (index & 0x08) ? 1 : 0);
    LCD_PutCHString(104, 48, Number[System.Run.TmpMonoVolume], 0);
}

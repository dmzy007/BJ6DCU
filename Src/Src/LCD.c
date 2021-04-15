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

//��������
void LCD_IO_Config(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* ����LCD SPI�� CS���ţ���ͨIO���� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* ����LCD SPI�� SCK����*/
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* ����LCD SPI�� MISO����*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* ����LCD SPI�� MOSI����*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* ����LCD SPI�� RST���ţ���ͨIO���� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* ����LCD SPI�� BUSY���ţ���ͨIO���� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* ֹͣ�ź� FLASH: CS���Ÿߵ�ƽ*/
    SPI_SS_H();

    /* SPI ģʽ���� */
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

    /* ʹ�� SPI  */
    SPI_Cmd(SPI1, ENABLE);
}

//========================================================================
// ����: void SPI_SSSet(unsigned char Status)
// ����: ��SS��״̬
// ����: Status   =1���øߵ�ƽ��=0���õ͵�ƽ
// ����: ��
// �汾:
//      2007/07/17      First version
//		2007/07/24		V1.2 for MCS51 Keil C
//========================================================================
void SPI_SSSet(unsigned int Status)
{
    while (Busy())
        ;  //������һ��ʱ��
    _delay_us(5);
    if (Status)  //�ж���Ҫ��SSΪ�ͻ��Ǹߵ�ƽ��
    {            // SS�øߵ�ƽ
        SPI_SS_H();
    }
    else  // SS�õ͵�ƽ
    {
        SPI_SS_L();
    }
}

//========================================================================
// ����: void SPI_Send(unsigned char Data)
// ����: ͨ������SPI������һ��byte��������ģ��
// ����: Data Ҫ���͵�����
// ����: ��
// �汾:
//      2007/07/17      First version
//		2007/07/24		V1.2 for MCS51 Keil C
//========================================================================
void SPI_Send(unsigned int Data)
{
    /* �ȴ����ͻ�����Ϊ�գ�TXE�¼� */
    while (Busy())
        ;
    _delay_us(5);

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
        ;

    /* д�����ݼĴ�������Ҫд�������д�뷢�ͻ����� */
    SPI_I2S_SendData(SPI1, Data);
}

//========================================================================
// ����: void FontSet(unsigned char Font_NUM,unsigned char Color)
// ����: ASCII�ַ���������
// ����: Font_NUM ����ѡ��,�������������ֿ�Ϊ׼
//		 Color  �ı���ɫ,��������ASCII�ֿ�
// ����: ��
// ��ע:
// �汾:
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
    //����ASCII�ַ�������
    SPI_SSSet(0);      // SS�õ͵�ƽ
    SPI_Send(0x81);    //����ָ��0x81
    SPI_Send(ucTemp);  //ѡ��8X16��ASCII����,�ַ�ɫΪ��ɫ
    SPI_SSSet(1);      //��ɲ�����SS�ߵ�ƽ
}

//========================================================================
// ����: void FontSet_cn(unsigned char Font_NUM,unsigned char Color)
// ����: ���ֿ��ַ���������
// ����: Font_NUM ����ѡ��,�������������ֿ�Ϊ׼
//		 Color  �ı���ɫ,�������ں��ֿ�
// ����: ��
// ��ע:
// �汾:
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
    //����ASCII�ַ�������
    SPI_SSSet(0);      // SS�õ͵�ƽ
    SPI_Send(0x82);    //����ָ��0x81
    SPI_Send(ucTemp);  //ѡ��8X16��ASCII����,�ַ�ɫΪ��ɫ
    SPI_SSSet(1);      //��ɲ�����SS�ߵ�ƽ
}

//========================================================================
// ����: void PutChar(unsigned char x,unsigned char y,unsigned char a)
// ����: д��һ����׼ASCII�ַ�
// ����: x  X������     y  Y������
//		 a  Ҫ��ʾ���ַ����ֿ��е�ƫ����
// ����: ��
// ��ע: ASCII�ַ���ֱ������ASCII�뼴��
// �汾:
//      2007/07/19      First version
//========================================================================
void PutChar(unsigned char x, unsigned char y, unsigned char a)
{
    //��ʾASCII�ַ�
    SPI_SSSet(0);  // SS�õ͵�ƽ
    SPI_Send(7);   //����ָ��0x07
    SPI_Send(x);   //Ҫ��ʾ�ַ������Ͻǵ�X��λ��
    SPI_Send(y);   //Ҫ��ʾ�ַ������Ͻǵ�Y��λ��
    SPI_Send(a);   //Ҫ��ʾ�ַ�ASCII�ַ���ASCII��ֵ
    SPI_SSSet(1);  //��ɲ�����SS�ߵ�ƽ
}

//========================================================================
// ����: void PutString(int x,int y,char *p)
// ����: ��x��yΪ��ʼ���괦д��һ����׼ASCII�ַ�
// ����: x  X������     y  Y������
//		 p  Ҫ��ʾ���ַ���
// ����: ��
// ��ע: ���������Դ���ASCII�ַ�����ʾ
// �汾:
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
// ����: void PutChar_cn(unsigned char x,unsigned char y,unsigned short * GB)
// ����: д��һ���������ֿ⺺��
// ����: x  X������     y  Y������
//		 a  GB��
// ����: ��
// ��ע:
// �汾:
//      2007/07/19      First version
//		2007/07/24		V1.2 for MCS51 Keil C
//========================================================================
void PutChar_cn(unsigned char x, unsigned char y, const unsigned char *GB)
{
    //��ʾASCII�ַ�
    SPI_SSSet(0);  // SS�õ͵�ƽ
    SPI_Send(8);   //����ָ��0x08
    SPI_Send(x);   //Ҫ��ʾ�ַ������Ͻǵ�X��λ��
    SPI_Send(y);   //Ҫ��ʾ�ַ������Ͻǵ�Y��λ��

    SPI_Send(*(GB++));  //���Ͷ����ֿ��к���GB��ĸ߰�λֵ
    SPI_Send(*GB);      //���Ͷ����ֿ��к���GB��ĵͰ�λֵ
    SPI_SSSet(1);       //��ɲ�����SS�ߵ�ƽ
}

//========================================================================
// ����: void PutString_cn(unsigned char x,unsigned char y,unsigned char *p)
// ����: ��x��yΪ��ʼ���괦д��һ�������ַ�
// ����: x  X������     y  Y������
//		 p  Ҫ��ʾ���ַ���
// ����: ��
// ��ע: ͬPutChar_cn�еĽ���
// �汾:
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
            //			x += X_Witch_cn+1;						//�ַ������1
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
// ����: void SetPaintMode(unsigned char Mode,unsigned char Color)
// ����: ��ͼģʽ����
// ����: Mode ��ͼģʽ    Color  ���ص����ɫ,�൱��ǰ��ɫ
// ����: ��
// ��ע: Mode��Ч
// �汾:
//      2007/07/19      First version
//========================================================================
void SetPaintMode(unsigned char Mode, unsigned char Color)
{
    unsigned char ucTemp = 0;
    ucTemp               = (Mode << 4) | Color;
    //���û�ͼģʽ
    SPI_SSSet(0);      // SS�õ͵�ƽ
    SPI_Send(0x83);    //����ָ��0x83
    SPI_Send(ucTemp);  //ѡ��8X16��ASCII����,�ַ�ɫΪ��ɫ
    SPI_SSSet(1);      //��ɲ�����SS�ߵ�ƽ
}

//========================================================================
// ����: void PutPixel(unsigned char x,unsigned char y)
// ����: ��x��y���ϻ���һ��ǰ��ɫ�ĵ�
// ����: x  X������     y  Y������
// ����: ��
// ��ע: ʹ��ǰ��ɫ
// �汾:
//      2007/07/19      First version
//========================================================================
void PutPixel(unsigned char x, unsigned char y)
{
    //������
    SPI_SSSet(0);  // SS�õ͵�ƽ
    SPI_Send(1);   //��ָ��0x01
    SPI_Send(x);   //�͵�һ������,�����õ��X��λ��
    SPI_Send(y);   //���Y��λ��
    SPI_SSSet(1);  //��ɲ�����SS�ߵ�ƽ
}

//========================================================================
// ����: void Line(unsigned char s_x,unsigned char  s_y,
//					unsigned char  e_x,unsigned char  e_y)
// ����: ��s_x��s_yΪ��ʼ���꣬e_x��e_yΪ�����������һ��ֱ��
// ����: x  X������     y  Y������
// ����: ��
// ��ע: ʹ��ǰ��ɫ
// �汾:
//      2007/07/19      First version
//========================================================================
void Line(unsigned char s_x, unsigned char s_y, unsigned char e_x, unsigned char e_y)
{
    //����ֱ��
    SPI_SSSet(0);   // SS�õ͵�ƽ
    SPI_Send(2);    //��ָ��0x02
    SPI_Send(s_x);  //���X������
    SPI_Send(s_y);  //���Y������
    SPI_Send(e_x);  //�յ�X������
    SPI_Send(e_y);  //�յ�Y������
    SPI_SSSet(1);   //��ɲ�����SS�ߵ�ƽ
}

//========================================================================
// ����: void Circle(unsigned char x,unsigned char y,
//					unsigned char r,unsigned char mode)
// ����: ��x,yΪԲ��RΪ�뾶��һ��Բ(mode = 0) or Բ��(mode = 1)
// ����:
// ����: ��
// ��ע: ��Բ����ִ�н��������MCU�п��Ź����������幷�Ĳ���
// �汾:
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
// ����: void Rectangle(unsigned char left, unsigned char top, unsigned char right,
//				 unsigned char bottom, unsigned char mode)
// ����: ��x,yΪԲ��RΪ�뾶��һ��Բ(mode = 0) or Բ��(mode = 1)
// ����: left - ���ε����ϽǺ����꣬��Χ0��126
//		 top - ���ε����Ͻ������꣬��Χ0��62
//		 right - ���ε����½Ǻ����꣬��Χ1��127
//		 bottom - ���ε����½������꣬��Χ1��63
//		 Mode - ����ģʽ��������������ֵ֮һ��
//				0:	���ο򣨿��ľ��Σ�
//				1:	�����棨ʵ�ľ��Σ�
// ����: ��
// ��ע: ��Բ����ִ�н��������MCU�п��Ź����������幷�Ĳ���
// �汾:
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
// ����: void Images(unsigned char s_x, unsigned char s_p, unsigned char x_width,
//                  unsigned char p_width, unsigned char *p, unsigned int length)
// ����: ��ͼƬ�ķ�ʽ��ʾ��������
// ����: s_x - Ҫ������ʼλ�õ� X ������
//       s_p - Ҫ������ʼλ�õ�ҳ��(Page) ����Χ0��7
//		 x_width - ���ε����½Ǻ����꣬��Χ1��127
//		 p_width - λͼͼ�� Y �᷽���ҳ��(Page)
//		 p - ��ʾ��������
//		 length - ��ʾ�������鳤�ȣ����ó���512
// ����: ��
// �汾:
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
// ����: void ClrScreen(void)
// ����: ����������ִ��ȫ��Ļ��������ǰ��ɫ
// ����:
// ����: ��
// ��ע:
// �汾:
//      2007/07/19      First version
//========================================================================
void ClrScreen(void)
{
    //��������
    SPI_SSSet(0);    // SS�õ͵�ƽ
    SPI_Send(0x80);  //��ָ��0x80
    SPI_Send(0);     //ָ������
    SPI_SSSet(1);    //��ɲ�����SS�ߵ�ƽ
}

//=====================================================================
// ����: void FontMode(unsigned char Cover,unsigned char Color)
// ����: �ַ�����ģʽ����
// ����:  Cover 0 �ַ���ʾʱ�����ַ�����ı�������
//			    1 �ַ���ʾʱ��Ҫ��ʾ���ַ�����ı�������
//		 Color  �ַ�����ģʽ(Cover)Ϊ1ʱ��Ч����ʹ�úڻ�׽����ַ����򱳾����
//				0/1
// ����: ��
// ��ע:
// �汾:
//      2009/01/19      First version
//=====================================================================
void FontMode(unsigned char Cover, unsigned char Color)
{
    unsigned char ucTemp = 0;
    ucTemp               = (Cover << 4) | Color;
    SPI_SSSet(0);      // SS�õ͵�ƽ
    SPI_Send(0x89);    //����ָ��0x89
    SPI_Send(ucTemp);  //����ָ������
    SPI_SSSet(1);      //��ɲ�����SS�ߵ�ƽ
}

//====================================================================
// ����: void Show_Char(unsigned char x,unsigned char y,unsigned char a,unsigned char type)
// ����: ��x��y��λ��Ϊ��ʾ��������Ͻǣ���ʾһ��unsigned char����
// ����: x X������  y Y������  a Ҫ��ʾ��8λ��ȵ�����  type ��ʾ����(0,1,2)
// ����: ��
// ��ע:
// �汾:
//      2008/11/27      First version
//====================================================================
void Show_Char(unsigned char x, unsigned char y, unsigned char a, unsigned char type)
{
    SPI_SSSet(0);    // SS�õ͵�ƽ
    SPI_Send(11);    //����ָ��11
    SPI_Send(x);     //Ҫ��ʾ���ֵ����Ͻǵ�X��λ��
    SPI_Send(y);     //Ҫ��ʾ���ֵ����Ͻǵ�Y��λ��
    SPI_Send(type);  //Ҫ��ʾ���ֵ���ʾ����
    SPI_Send(a);
    SPI_SSSet(1);  //��ɲ�����SS�ߵ�ƽ
}

//====================================================================
// ����: void Show_Short(unsigned char x,unsigned char y,unsigned short a,unsigned char type)
// ����: ��x��y��λ��Ϊ��ʾ��������Ͻǣ���ʾһ��unsigned short������16λ��ȣ�
// ����: x X������    y Y������   a Ҫ��ʾ��16λ��ȵ�����  type ��ʾ����(0,1,2)
// ����: ��
// ��ע:
// �汾:
//      2008/11/27      First version
//====================================================================
void Show_Short(unsigned char x, unsigned char y, unsigned short a, unsigned char type)
{
    SPI_SSSet(0);    // SS�õ͵�ƽ
    SPI_Send(12);    //����ָ��12
    SPI_Send(x);     //Ҫ��ʾ���ֵ����Ͻǵ�X��λ��
    SPI_Send(y);     //Ҫ��ʾ���ֵ����Ͻǵ�Y��λ��
    SPI_Send(type);  //Ҫ��ʾ���ֵ���ʾ����
    SPI_Send((unsigned char)(a >> 8));
    SPI_Send((unsigned char)a);
    SPI_SSSet(1);  //��ɲ�����SS�ߵ�ƽ
}

//=====================================================================
// ����: void SetLCDGra(unsigned char Dat)
// ����: LCD������ʾ�Ҷȵ���
// ����:  Dat   Ҫ���ɵĻҶ�ֵ,0~0x3f
//
// ����: ��
// ��ע: ���ں�ĻҶȽ��ᱣ�ֵ���һ���� LCD��λ֮ǰ,������һ�����µ����Ҷ�֮ǰ
// �汾:
//      2009/05/10      First version
//=====================================================================
void SetLCDGra(unsigned char Dat)
{
    //��ʾASCII�ַ�
    SPI_SSSet(0);    // SS�õ͵�ƽ
    SPI_Send(0x8a);  //����ָ��0x8a
    SPI_Send(Dat);   //������ֵ
    SPI_SSSet(1);    //��ɲ�����SS�ߵ�ƽ
}

//=====================================================================
// ����: void SetLCDSleep(void)
// ����: ����ģ�����͹���ģʽ
// ����:  Dat   0xaa ����͹���ģʽ��,���ϱ�����ʾ
//				0x55 ����͹���ģʽ��,���Ϲر���ʾ
//
// ����: ��
// ��ע: ģ�齫���ڽ���͹���ģʽ��,SS�����ϵĵ�ƽ�仯��ָ���������ģʽ
// �汾:
//      2009/05/10      First version
//=====================================================================
void SetLCDSleep(unsigned char Dat)
{
    SPI_SSSet(0);    // SS�õ͵�ƽ
    SPI_Send(0x8b);  //����ָ��0x8b
    SPI_Send(Dat);   //������ֵ
    SPI_SSSet(1);    //��ɲ�����SS�ߵ�ƽ
    _delay_us(30);
}

void CloseLCD(unsigned char Dat)
{
    SPI_SSSet(0);    // SS�õ͵�ƽ
    SPI_Send(0x88);  //����ָ��0x8b
    SPI_Send(Dat);   //������ֵ
    SPI_SSSet(1);    //��ɲ�����SS�ߵ�ƽ
    _delay_us(30);
}

//=====================================================================
// ����: void SetLCDWakeup(void)
// ����: ����ģ���˳��͹���ģʽ
// ����:  ��
//
// ����: ��
// ��ע: ģ�齫���ڽ���͹���ģʽ��,SS�����ϵĵ�ƽ�仯��ָ���������ģʽ
// �汾:
//      2009/05/10      First version
//=====================================================================
void SetLCDWakeup(void)
{
    SPI_SSSet(0);  // SS�õ͵�ƽ
    _delay_us(100);
    SPI_SSSet(1);  //��ɲ�����SS�ߵ�ƽ
}

/**** ��� ****/
// X[0,192],Y[0,8]
void clear(unsigned short sx, unsigned short sy, unsigned short ex, unsigned short ey)
{
    FontMode(1, 0);  //�����ַ�����ģʽΪʹ�ܣ��ַ���������ɫΪ0
    FontSet_cn(0, 1);
    FontSet(0, 1);
    SetPaintMode(1, 0);

    Rectangle(sx, sy, ex, ey, 1);
}

//Һ������ʼ��
void LCD_init(void)
{
    RESET_L();
    _delay_ms(10);
    RESET_H();

    ClrScreen();
    SetLCDGra(30);

    FontMode(1, 0);  //�����ַ�����ģʽΪʹ�ܣ��ַ���������ɫΪ0
    FontSet_cn(0, 1);
    FontSet(0, 1);
    SetPaintMode(1, 0);

    Rectangle(0, 0, 127, 63, 1);
}

//д��һ�ַ�
void LCD_PutChar(unsigned short x, unsigned short y, char c)
{
    FontMode(1, 0);  //�����ַ�����ģʽΪʹ�ܣ��ַ���������ɫΪ0
    FontSet_cn(0, 1);
    FontSet(0, 1);
    SetPaintMode(1, 0);

    PutChar(x, y, c);
}

//��̬��ʾ�����ַ���
void LCD_PutCHString(unsigned short x, unsigned short y, const unsigned char *s, unsigned char Mode)
{
    if (Mode)
    {
        FontMode(1, 1);  //�����ַ�����ģʽΪʹ�ܣ��ַ���������ɫΪ0
        FontSet_cn(0, 0);
        FontSet(0, 0);
    }
    else
    {
        FontMode(1, 0);  //�����ַ�����ģʽΪʹ�ܣ��ַ���������ɫΪ0
        FontSet_cn(0, 1);
        FontSet(0, 1);
    }
    SetPaintMode(1, 0);

    PutString_cn(x, y, s);
}

void LCD_DIS_MAIN(void)
{
    /*��һ����ʾ*/
    LCD_PutCHString(0, 0, Masterinf[System.SystemInfo.COMMatser], 0);

    LCD_PutCHString(108, 0, " ", 1);
    LCD_PutCHString(112, 0, Addressinf[(System.SystemInfo.LieCheAddress == HeadID) ? 1 : 2], 1);
    /*�ڶ�����ʾ*/
    Images(4, 2, 120, 4, CHzm[System.SystemInfo.NowStation], 360);

    /*��������ʾ*/
    LCD_PutCHString(0, 48, Directioninf[System.SystemInfo.ShangXiaxing], 0);
    LCD_PutCHString(64, 48, Number[System.SystemInfo.StartStation], 0);
    LCD_PutCHString(88, 48, Number[System.SystemInfo.NowStation], 0);
    LCD_PutCHString(112, 48, Number[System.SystemInfo.EndStation], 0);
}

void LCD_DIS_EmerGB(u8 index)
{
    index = (1 << (index - 1));

    LCD_PutCHString(8, 0, "01", 0);
    LCD_PutCHString(56, 0, "��ͣ1��", (index & 0x01) ? 1 : 0);
    LCD_PutCHString(8, 16, "02", 0);
    LCD_PutCHString(56, 16, "��ͣ3��", (index & 0x02) ? 1 : 0);
    LCD_PutCHString(8, 32, "03", 0);
    LCD_PutCHString(56, 32, "������ͣ", (index & 0x04) ? 1 : 0);
    LCD_PutCHString(8, 48, "04", 0);
    LCD_PutCHString(56, 48, "��������", (index & 0x08) ? 1 : 0);
}

void LCD_DIS_Setting(u8 index)
{
    index = (1 << (index - 1));

    LCD_PutCHString(8, 0, "����ģʽ", (index & 0x01) ? 1 : 0);
    LCD_PutCHString(104, 0, Number[System.Run.TmpMasterMode], 0);
    LCD_PutCHString(8, 16, "��վģʽ", (index & 0x02) ? 1 : 0);
    LCD_PutCHString(104, 16, Number[System.Run.TmpGBMode], 0);
    LCD_PutCHString(8, 32, "����ģʽ", (index & 0x04) ? 1 : 0);
    LCD_PutCHString(104, 32, Number[System.Run.TmpWorkMode], 0);
    LCD_PutCHString(8, 48, "��������", (index & 0x08) ? 1 : 0);
    LCD_PutCHString(104, 48, Number[System.Run.TmpMonoVolume], 0);
}

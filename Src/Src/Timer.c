#include "Timer.h"

void Timer2_Configuration(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  //ʹ��ʱ��

    TIM_DeInit(TIM2);
    TIM_TimeBaseStructure.TIM_Prescaler     = 3600;   //��Ƶ
    TIM_TimeBaseStructure.TIM_Period        = 10000;  //�����Զ�װ�ؼĴ���
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;  //ѡ�����ϼ���
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_ClearFlag(TIM2, TIM_FLAG_Update);

    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);  //ʹ���ж�

    TIM_Cmd(TIM2, ENABLE);                                 //ʹ��
                                                           //
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE);  //�ȹرյȴ�ʹ��
    TIM_SetCounter(TIM2, 0);
}

void TIM2_Open(void)
{
    TIM_SetCounter(TIM2, 0);
    TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

void TIM2_Close(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE);
    TIM_Cmd(TIM2, DISABLE);
}

void Timer3_Configuration(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  //ʹ��ʱ��

    TIM_DeInit(TIM3);
    TIM_TimeBaseStructure.TIM_Prescaler     = 23999;  //��Ƶ
    TIM_TimeBaseStructure.TIM_Period        = 30000;  //�����Զ�װ�ؼĴ���
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;  //ѡ�����ϼ���
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    TIM_ClearFlag(TIM3, TIM_FLAG_Update);

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);  //ʹ���ж�

    TIM_Cmd(TIM3, ENABLE);                                 //ʹ��
                                                           //
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE);  //�ȹرյȴ�ʹ��
    TIM_SetCounter(TIM3, 0);
}

void TIM3_Open(void)
{
    TIM_SetCounter(TIM3, 0);
    TIM_ClearITPendingBit(TIM3, TIM_FLAG_Update);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
}

void TIM3_Close(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE);
    TIM_Cmd(TIM3, DISABLE);
}

void Timer4_Configuration(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);  //ʹ��ʱ��

    TIM_DeInit(TIM4);
    TIM_TimeBaseStructure.TIM_Prescaler     = 7199;  //��Ƶ
    TIM_TimeBaseStructure.TIM_Period        = 5000;  //�����Զ�װ�ؼĴ���
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;  //ѡ�����ϼ���
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    TIM_ClearFlag(TIM4, TIM_FLAG_Update);

    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);  //ʹ���ж�

    TIM_Cmd(TIM4, ENABLE);                                 //ʹ��
                                                           //
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, DISABLE);  //�ȹرյȴ�ʹ��
    TIM_SetCounter(TIM4, 0);
}

void TIM4_Open(void)
{
    TIM_SetCounter(TIM4, 0);
    TIM_ClearITPendingBit(TIM4, TIM_FLAG_Update);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    TIM_Cmd(TIM4, ENABLE);
}

void TIM4_Close(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, DISABLE);
    TIM_Cmd(TIM4, DISABLE);
}

void Timer5_Configuration(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);  //ʹ��ʱ��

    TIM_DeInit(TIM5);
    TIM_TimeBaseStructure.TIM_Prescaler     = 7200;  //��Ƶ
    TIM_TimeBaseStructure.TIM_Period        = 1000;  //�����Զ�װ�ؼĴ���
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;  //ѡ�����ϼ���
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

    TIM_ClearFlag(TIM5, TIM_FLAG_Update);

    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);  //ʹ���ж�

    TIM_Cmd(TIM5, ENABLE);                                 //ʹ��
                                                           //
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, DISABLE);  //�ȹرյȴ�ʹ��
    TIM_SetCounter(TIM5, 0);
}

void TIM5_Open(void)
{
    TIM_SetCounter(TIM5, 0);
    TIM_ClearITPendingBit(TIM5, TIM_FLAG_Update);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
    TIM_Cmd(TIM5, ENABLE);
}

void TIM5_Close(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, DISABLE);
    TIM_Cmd(TIM5, DISABLE);
}

// void Timer6_Configuration(void)
// {
//    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
//
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);         //ʹ��ʱ��
//
//    TIM_DeInit(TIM6);
//    TIM_TimeBaseStructure.TIM_Prescaler = 720;                 //��Ƶ
//    TIM_TimeBaseStructure.TIM_Period = 1000;                     //�����Զ�װ�ؼĴ���
//    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
//    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //ѡ�����ϼ���
//    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
//
//    TIM_ClearFlag(TIM6, TIM_FLAG_Update);
//
//    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);                   //ʹ���ж�
//
//    TIM_Cmd(TIM6, ENABLE);                                       //ʹ��
//                                                                 //
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, DISABLE);        //�ȹرյȴ�ʹ��
//    TIM_SetCounter(TIM6, 0);
// }
//
// void TIM6_Open(void)
// {
//   TIM_SetCounter(TIM6, 0);
//   TIM_ClearITPendingBit(TIM6, TIM_FLAG_Update);
//   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
//   TIM_Cmd(TIM6, ENABLE);
// }
//
// void TIM6_Close(void)
// {
//   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, DISABLE);
//   TIM_Cmd(TIM6, DISABLE);
// }

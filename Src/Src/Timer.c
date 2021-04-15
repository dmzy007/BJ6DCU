#include "Timer.h"

void Timer2_Configuration(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  //使能时钟

    TIM_DeInit(TIM2);
    TIM_TimeBaseStructure.TIM_Prescaler     = 3600;   //分频
    TIM_TimeBaseStructure.TIM_Period        = 10000;  //设置自动装载寄存器
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;  //选择向上计数
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_ClearFlag(TIM2, TIM_FLAG_Update);

    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);  //使能中断

    TIM_Cmd(TIM2, ENABLE);                                 //使能
                                                           //
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE);  //先关闭等待使用
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

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  //使能时钟

    TIM_DeInit(TIM3);
    TIM_TimeBaseStructure.TIM_Prescaler     = 23999;  //分频
    TIM_TimeBaseStructure.TIM_Period        = 30000;  //设置自动装载寄存器
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;  //选择向上计数
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    TIM_ClearFlag(TIM3, TIM_FLAG_Update);

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);  //使能中断

    TIM_Cmd(TIM3, ENABLE);                                 //使能
                                                           //
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE);  //先关闭等待使用
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

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);  //使能时钟

    TIM_DeInit(TIM4);
    TIM_TimeBaseStructure.TIM_Prescaler     = 7199;  //分频
    TIM_TimeBaseStructure.TIM_Period        = 5000;  //设置自动装载寄存器
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;  //选择向上计数
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    TIM_ClearFlag(TIM4, TIM_FLAG_Update);

    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);  //使能中断

    TIM_Cmd(TIM4, ENABLE);                                 //使能
                                                           //
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, DISABLE);  //先关闭等待使用
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

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);  //使能时钟

    TIM_DeInit(TIM5);
    TIM_TimeBaseStructure.TIM_Prescaler     = 7200;  //分频
    TIM_TimeBaseStructure.TIM_Period        = 1000;  //设置自动装载寄存器
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;  //选择向上计数
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

    TIM_ClearFlag(TIM5, TIM_FLAG_Update);

    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);  //使能中断

    TIM_Cmd(TIM5, ENABLE);                                 //使能
                                                           //
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, DISABLE);  //先关闭等待使用
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
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);         //使能时钟
//
//    TIM_DeInit(TIM6);
//    TIM_TimeBaseStructure.TIM_Prescaler = 720;                 //分频
//    TIM_TimeBaseStructure.TIM_Period = 1000;                     //设置自动装载寄存器
//    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
//    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //选择向上计数
//    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
//
//    TIM_ClearFlag(TIM6, TIM_FLAG_Update);
//
//    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);                   //使能中断
//
//    TIM_Cmd(TIM6, ENABLE);                                       //使能
//                                                                 //
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, DISABLE);        //先关闭等待使用
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

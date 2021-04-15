/**
 ******************************************************************************
 * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c
 * @author  MCD Application Team
 * @version V3.5.0
 * @date    08-April-2011
 * @brief   Main Interrupt Service Routines.
 *          This file provides template for all exceptions handler and
 *          peripherals interrupt service routine.
 ******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "Timer.h"
#include "bsp.h"
#include "can_drv.h"
extern SystemTypeDef System;
extern CANTypeDef    CAN;
extern u32           DispDelay;
/** @addtogroup STM32F10x_StdPeriph_Template
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief  This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler(void)
{
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval None
 */
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval None
 */
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles SVCall exception.
 * @param  None
 * @retval None
 */
void SVC_Handler(void)
{
}

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler(void)
{
}

/**
 * @brief  This function handles PendSVC exception.
 * @param  None
 * @retval None
 */
void PendSV_Handler(void)
{
}

/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 */
void SysTick_Handler(void)
{
    static u16 cnt  = 0;
    static u8  flag = 0;

    if (DispDelay < 1000)
    {
        DispDelay++;
    }

    // if (cnt < 400)
    // {
    //     cnt++;
    // }
    // else
    // {
    //     cnt = 0;

    //     if (flag)
    //     {
    //         flag = 0;

    //         Images(4, 2, 120, 4, CHzm[1], 360);
    //     }
    //     else
    //     {
    //         flag = 1;
    //         clear(24, 24, 104, 47);
    //     }
    // }
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
/**
 * @brief  This function handles CAN1 Receive interrupt request.
 * @param  None
 * @retval None
 */
void CAN1_RX0_IRQHandler(void)
{
    CAN_Receive(CAN1, CAN_FIFO0, &CAN.RxMessage[CAN.CANrxSP]);
    CAN.CANrxSP++;

    if (CAN.CANrxSP == CANMAXCONT)
    {
        CAN.CANrxSP = 0;
    }
}
/**
 * @brief  This function handles TIM2 interrupt request.
 * @param  None
 * @retval None
 */
void TIM2_IRQHandler(void)
{
    static u8 flag = 0;
    if (TIM_GetITStatus(TIM2, TIM_FLAG_Update) == SET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update);

        if (flag)
        {
            flag = 0;

            if (System.Run.TIM2Flag == 1)
            {
                Images(4, 2, 120, 4, CHzm[System.Run.TmpNowStation], 360);
                LCD_PutCHString(88, 48, Number[System.Run.TmpNowStation], 0);
            }
            else if (System.Run.TIM2Flag == 2)
            {
                Images(4, 2, 120, 4, CHzm[System.Run.KEYSelect], 360);
                LCD_PutCHString(64, 48, Number[System.Run.KEYSelect], 0);
            }
            else if (System.Run.TIM2Flag == 3)
            {
                Images(4, 2, 120, 4, CHzm[System.Run.KEYSelect], 360);
                LCD_PutCHString(64, 48, Number[System.Run.KEYSelect], 0);
            }
            else if (System.Run.TIM2Flag == 4)
            {
                Images(4, 2, 120, 4, CHzm[System.Run.KEYSelect], 360);
                LCD_PutCHString(64, 48, Number[System.Run.KEYSelect], 0);
            }
            else if (System.Run.TIM2Flag == 5)
            {
                Images(4, 2, 120, 4, CHzm[System.SystemInfo.YueZhan[System.Run.KEYSelect]], 360);
                LCD_PutCHString(64, 48, Number[System.SystemInfo.YueZhan[System.Run.KEYSelect]], 0);
            }
        }
        else
        {
            flag = 1;

            if (System.Run.TIM2Flag == 1)
            {
                clear(4, 16, 124, 47);
                clear(88, 48, 104, 63);
            }
            else if ((System.Run.TIM2Flag == 2) || (System.Run.TIM2Flag == 3) || (System.Run.TIM2Flag == 4))
            {
                clear(4, 16, 124, 47);
            }
        }
    }
}

/**
 * @brief  This function handles TIM3 interrupt request.
 * @param  None
 * @retval None
 */
void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_FLAG_Update) == SET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_FLAG_Update);
        System.SystemInfo.SystemStatus = false;
    }
}

/**
 * @brief  This function handles TIM4 interrupt request.
 * @param  None
 * @retval None
 */
void TIM4_IRQHandler(void)
{
    static u8 i = 0;
    if (TIM_GetITStatus(TIM4, TIM_FLAG_Update) == SET)
    {
        TIM_ClearITPendingBit(TIM4, TIM_FLAG_Update);
        if (i)
        {
            i = 0;

            GPIO_SetBits(GPIOD, GPIO_Pin_3);
            System.SystemInfo.KeyLedStatus |= KEY5LED;
        }
        else
        {
            i = 1;

            GPIO_ResetBits(GPIOD, GPIO_Pin_3);
            System.SystemInfo.KeyLedStatus &= ~KEY5LED;
        }
    }
}

/**
 * @brief  This function handles TIM5 interrupt request.
 * @param  None
 * @retval None
 */
void TIM5_IRQHandler(void)
{
    static u8 gpio[3] = {1};
    static u8 cnt     = 0;
    if (TIM_GetITStatus(TIM5, TIM_FLAG_Update) == SET)
    {
        TIM_ClearITPendingBit(TIM5, TIM_FLAG_Update);

        gpio[cnt] = MANGB_in;
        cnt++;

        if (cnt >= 3)
        {
            cnt = 0;

            if ((gpio[0] == gpio[1]) && (gpio[1] == gpio[2]) && (gpio[2] == 0x00))
            {
                if ((System.SystemInfo.KeyMan == 0) && (System.SystemInfo.DCUMatser == 1))
                {
                    DB3812WriteVolume(System.Volume.Man_Volume, System.Volume.JT_Volume, ChxVolume);
                    DB3812WriteVolume(Gain, 0, ChxGain);  //写增益
                    CAN_SendSetVoiceCMD(0x01, 0x00, 0x00);

                    if (System.Run.AlarmTalk == 2)
                    {
                        System.Run.AlarmTalk = 0;
                        CAN_SendAlarmCMD(0x03, System.SystemInfo.BaoJingCheXiangID[0]);
                        DecreAlarmID(System.SystemInfo.BaoJingCheXiangID[0]);
                        System.SystemInfo.KeyLedStatus &= ~KEY5LED;

                        GPIO_ResetBits(GPIOB, GPIO_Pin_8 | GPIO_Pin_9);
                    }
                    CancelInterCom();
                }
                System.SystemInfo.KeyMan = 1;
                //                else
                //                {
                //                    System.SystemInfo.KeyMan = 0;
                //                    WDI_CLR();  //清MAX813；使MCU控制AMC有效;
                //                    CLR_MANGB();
                //                }
            }
            else
            {
                if (System.SystemInfo.KeyMan == 1)
                {
                    System.SystemInfo.KeyMan = 0;
                    CAN_SendSetVoiceCMD(0x03, 0x00, 0x00);
                    KeepInterCom();
                }
            }
        }
    }
}

/**
 * @brief  This function handles TIM6 interrupt request.
 * @param  None
 * @retval None
 */
// void TIM6_IRQHandler(void)
// {
//    if (TIM_GetITStatus(TIM6, TIM_FLAG_Update) == SET)
//    {
//       TIM_ClearITPendingBit(TIM6, TIM_FLAG_Update);
//    }
// }
/**
 * @brief  This function handles PPP interrupt request.
 * @param  None
 * @retval None
 */
/*void PPP_IRQHandler(void)
{
}*/

/**
 * @}
 */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

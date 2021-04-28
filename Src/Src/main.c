#include "bsp.h"

extern SystemTypeDef System;
extern u32           DispDelay;

int main(void)
{
    u8 ClearCnt = 0;
    Bsp_Init();

    while (1)
    {
        IWDG_ReloadCounter();

        if ((GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4) == 0) || (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) == 0))
        {
            if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4) == 0)
            {
                System.SystemInfo.LieCheAddress = HeadID;  // 10��ͷ
            }
            if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) == 0)
            {
                System.SystemInfo.LieCheAddress = TailID;  // 28��β
            }
        }
        else
        {
            System.SystemInfo.LieCheAddress = TailID;  // 28��β
        }

        if ((System.SystemInfo.SystemStatus && System.Run.ZhuBeiCmdSta))
        {
            if (ClearCnt)
            {
                ClearCnt = 0;
                clear(0, 0, 127, 63);
                LCD_DIS_MAIN();
            }
            CAN_Reciver_data_Analyse();
            ScanKey();
        }
        else
        {
            ScanKey();
            // if (!ClearCnt)
            // {
            //     ClearCnt = 1;
            //     clear(0, 0, 127, 63);
            // }
            // CAN_Reciver_data_Analyse();
            // LCD_PutCHString(16, 24, "ϵͳͨѶ�ж�", 0);
            // if (DispDelay == 1000)
            // {
            //     CAN_SendZhuBeiCMD(0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
            // }
        }

        // if (System.SystemInfo.DCUMatser == 2)
        // {
        //     if (System.SystemInfo.KeyMan == 1)
        //     {
        //         System.SystemInfo.KeyMan = 0;
        //         WDI_CLR();  //��MAX813��ʹMCU����AMC��Ч;
        //         CLR_MANGB();
        //         CAN_SendSetVoiceCMD(0x03, 0x00, 0x00);
        //         KeepInterCom();
        //     }
        // }
    }
}

#include "can_drv.h"
#include "stdio.h"

CANTypeDef CAN;

void CAN_Configuration(void)
{
    GPIO_InitTypeDef      GPIO_InitStructure;
    CAN_InitTypeDef       CAN_InitStructure;
    CAN_FilterInitTypeDef CAN_FilterInitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap2_CAN1, ENABLE);
    /* Configure CAN pin: RX */
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* Configure CAN pin: TX */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* CAN register init */
    CAN_DeInit(CAN1);

    /* CAN cell init */
    CAN_InitStructure.CAN_TTCM = DISABLE;
    // CAN_InitStructure.CAN_ABOM = DISABLE;
    CAN_InitStructure.CAN_ABOM = ENABLE;
    CAN_InitStructure.CAN_AWUM = DISABLE;
    CAN_InitStructure.CAN_NART = DISABLE;
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = DISABLE;
    ;
    CAN_InitStructure.CAN_Mode      = CAN_Mode_Normal;
    CAN_InitStructure.CAN_SJW       = CAN_SJW_1tq;
    CAN_InitStructure.CAN_BS1       = CAN_BS1_8tq;
    CAN_InitStructure.CAN_BS2       = CAN_BS2_7tq;
    CAN_InitStructure.CAN_Prescaler = 45;
    //  CAN_Init(&CAN_InitStructure);

    if (CAN_Init(CAN1, &CAN_InitStructure) == CANINITFAILED)
    {
        /* 初始化时先设置CAN_MCR的初始化位 */
        /* 然后查看硬件是否真的设置了CAN_MSR的初始化位来确认是否进入了初始化模式  */
    }

    /* CAN filter init */
    CAN_FilterInitStructure.CAN_FilterNumber         = 0;
    CAN_FilterInitStructure.CAN_FilterMode           = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale          = CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh         = 0x0000;
    CAN_FilterInitStructure.CAN_FilterIdLow          = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh     = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow      = 0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
    CAN_FilterInitStructure.CAN_FilterActivation     = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);

    /* IT Configuration for CAN */
    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
}

/**************************************************************************
 ** 描述：把二进制进制转换成ascii格式的表示--非等值转换
 ** 输入：*InBuffer	--	待转换的16进制数组		二进制形式
 **	    *OutBuffer	--	转换后的ASCII格式字符串  TEXT
 **	    Num				--	转换的InBuffer的长度
 ** 输出：无
 ** 说明：不检查数据长度是否越界
 ** 如  :   InBuffer  --  0xFF, 0xAB
 **     :   OutBuffer --  FFAB
 **************************************************************************/
void HexToAscii(uint8_t *InBuffer, vu8 *OutBuffer, unsigned int Num)
{
    unsigned char Loop = 0;

    for (Loop = 0; Loop < Num; Loop++)
    {
        if (InBuffer[Loop] / 16 >= 10)
        {
            OutBuffer[Loop * 2] = InBuffer[Loop] / 16 + 55;
        }
        else
            OutBuffer[Loop * 2] = InBuffer[Loop] / 16 + 48;

        if (InBuffer[Loop] % 16 >= 10)
        {
            OutBuffer[Loop * 2 + 1] = InBuffer[Loop] % 16 + 55;
        }
        else
            OutBuffer[Loop * 2 + 1] = InBuffer[Loop] % 16 + 48;
    }
}

void SaveAlarmID(vu8 Data)
{
    u8 i;

    for (i = 0; i < 12; i++)
    {
        if (System.SystemInfo.BaoJingCheXiangID[i] != Data)
        {
            if (System.SystemInfo.BaoJingCheXiangID[i] == 0x00)
            {
                System.SystemInfo.BaoJingCheXiangID[i] = Data;

                //                sprintf((char *)System.SystemInfo.BaoJingCheXiangIDString[i], "%d-%d", ((Data & 0xF0) >> 4), (Data & 0x0F));
                System.SystemInfo.BaoJingCheXiangIDString[i][0] = ((Data & 0xF0) >> 4) + 0x30;
                System.SystemInfo.BaoJingCheXiangIDString[i][1] = '-';
                System.SystemInfo.BaoJingCheXiangIDString[i][2] = (Data & 0x0F) + 0x30;

                System.SystemInfo.AlarmTotal++;

                if (System.Run.AlarmTalk == 0)
                {
                    TIM4_Open();
                    System.Run.AlarmTalk = 1;
                }

                break;
            }
        }
        else
        {
            return;
        }
    }
}

void DecreAlarmID(vu8 Data)
{
    u8 i;
    for (i = 0; i < 12; i++)
    {
        if (System.SystemInfo.BaoJingCheXiangID[i] == Data)
        {
            System.SystemInfo.BaoJingCheXiangID[i] = 0x00;
            System.SystemInfo.AlarmTotal--;
            memcpy((void *)(System.SystemInfo.BaoJingCheXiangID + i), (void *)(System.SystemInfo.BaoJingCheXiangID + i + 1),
                   (12 - i - 1) * sizeof(vu8));
            memcpy((void *)(System.SystemInfo.BaoJingCheXiangIDString + i), (void *)(System.SystemInfo.BaoJingCheXiangIDString + i + 1),
                   (12 - i - 1) * sizeof(System.SystemInfo.BaoJingCheXiangIDString[0]));

            if (System.SystemInfo.AlarmTotal)
            {
                if (System.Run.AlarmTalk != 2)
                {
                    TIM4_Open();
                    System.Run.AlarmTalk = 1;
                    CancelInterCom();
                }
            }
            else
            {
                TIM4_Close();
                System.Run.AlarmTalk = 0;
                GPIO_ResetBits(GPIOD, GPIO_Pin_3);
                System.SystemInfo.KeyLedStatus &= ~KEY5LED;

                GPIO_ResetBits(GPIOB, GPIO_Pin_8 | GPIO_Pin_9);
                KeepInterCom();
            }
            break;
        }
    }
}

void ClearSendDataBuffer(void)
{
    u8 i;
    for (i = 0; i < 8; i++)
    {
        System.Run.TmpCmd[i] = 0;
    }
}

void CAN_SendErrorCMD(void)
{
    CAN.TxMessage[CAN.CANtxSP].IDE     = CAN_ID_EXT;
    CAN.TxMessage[CAN.CANtxSP].DLC     = 0x08;
    CAN.TxMessage[CAN.CANtxSP].ExtId   = (ERRORCMDHEAD << 8) | (System.SystemInfo.LieCheAddress);
    CAN.TxMessage[CAN.CANtxSP].RTR     = CAN_RTR_DATA;
    CAN.TxMessage[CAN.CANtxSP].Data[0] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[1] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[2] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[3] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[4] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[4] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[5] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[6] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[7] = 0x00;

    CAN_Transmit(CAN1, &CAN.TxMessage[CAN.CANtxSP]);

    CAN.CANtxSP++;

    if (CAN.CANtxSP == CANMAXCONT)
    {
        CAN.CANtxSP = 0;
    }
}

void CAN_SendAlarmCMD(unsigned char CMD, unsigned char Pammater0)
{
    CAN.TxMessage[CAN.CANtxSP].IDE     = CAN_ID_EXT;
    CAN.TxMessage[CAN.CANtxSP].DLC     = 0x08;
    CAN.TxMessage[CAN.CANtxSP].ExtId   = (ALARMCMDHEAD << 8) | (System.SystemInfo.LieCheAddress);
    CAN.TxMessage[CAN.CANtxSP].RTR     = CAN_RTR_DATA;
    CAN.TxMessage[CAN.CANtxSP].Data[0] = CMD;
    CAN.TxMessage[CAN.CANtxSP].Data[1] = Pammater0;
    CAN.TxMessage[CAN.CANtxSP].Data[2] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[3] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[4] = 0x00;

    CAN.TxMessage[CAN.CANtxSP].Data[5] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[6] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[7] = 0x00;

    System.Run.TmpCmd[0] = CMD;
    System.Run.TmpCmd[1] = Pammater0;
    System.Run.TmpCmd[2] = 0x00;
    System.Run.TmpCmd[3] = 0x00;

    CAN_Transmit(CAN1, &CAN.TxMessage[CAN.CANtxSP]);

    if (!System.SystemInfo.CmdStatus)
    {
        TIM2_Open();
    }

    CAN.CANtxSP++;

    if (CAN.CANtxSP == CANMAXCONT)
    {
        CAN.CANtxSP = 0;
    }
}

/*接收报警信息*/
void CAN_ReciverAlarmCMD(uint8_t *Data)
{
    if (System.SystemInfo.DCUMatser == 1)
    {
        if (Data[0] == 0x01)
        {
            SaveAlarmID(Data[1]);
        }
        else if (Data[0] == 0x03)
        {
            DecreAlarmID(Data[1]);
        }
    }
}

/*
 * 设置起点、终点、当前、线路ID、上下行
 */
void CAN_SendLineInfoCMD(unsigned char Pammater0, unsigned char Pammater1, unsigned char Pammater2, unsigned char Pammater3, unsigned char Pammater4)
{
    System.Run.LineInfoCmdSta          = 0;
    CAN.TxMessage[CAN.CANtxSP].IDE     = CAN_ID_EXT;
    CAN.TxMessage[CAN.CANtxSP].DLC     = 0x08;
    CAN.TxMessage[CAN.CANtxSP].ExtId   = (LINEINFOCMDHEAD << 8) | (System.SystemInfo.LieCheAddress);
    CAN.TxMessage[CAN.CANtxSP].RTR     = CAN_RTR_DATA;
    CAN.TxMessage[CAN.CANtxSP].Data[0] = Pammater0;
    CAN.TxMessage[CAN.CANtxSP].Data[1] = Pammater1;
    CAN.TxMessage[CAN.CANtxSP].Data[2] = Pammater2;
    CAN.TxMessage[CAN.CANtxSP].Data[3] = Pammater3;
    CAN.TxMessage[CAN.CANtxSP].Data[4] = Pammater4;
    CAN.TxMessage[CAN.CANtxSP].Data[5] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[6] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[7] = 0x00;

    System.Run.TmpCmd[0] = Pammater0;
    System.Run.TmpCmd[1] = Pammater1;
    System.Run.TmpCmd[2] = Pammater2;
    System.Run.TmpCmd[3] = Pammater3;
    System.Run.TmpCmd[4] = Pammater4;

    CAN_Transmit(CAN1, &CAN.TxMessage[CAN.CANtxSP]);

    // if (!System.SystemInfo.CmdStatus)
    // {
    //    TIM2_Open();
    // }

    CAN.CANtxSP++;

    if (CAN.CANtxSP == CANMAXCONT)
    {
        CAN.CANtxSP = 0;
    }
}

void CAN_ReciverLineInfoCMD(uint8_t *Data)
{
    if (!Data[3])
    {
        System.Run.LineInfoCmdSta = 1;
        if ((System.Screen.MainScreen == 0) && (System.Screen.FuScreen == 1))
        {
            //       KeyLed(ALLKEYLEDOFF);
            System.SystemInfo.KeyLedStatus &= ~KEY1LED;
            System.Screen.MainScreen = 0;
            System.Screen.FuScreen   = 0;
            clear(0, 0, 192, 8);
            LCD_DIS_MAIN();
        }
        else if ((System.Screen.MainScreen == 4) && (System.Screen.FuScreen == 1))
        {
            //       KeyLed(ALLKEYLEDOFF);
            System.SystemInfo.KeyLedStatus &= ~KEY14LED;
            System.Screen.MainScreen = 0;
            System.Screen.FuScreen   = 0;
            clear(0, 0, 192, 8);
            LCD_DIS_MAIN();
        }
    }
}
/*
 * 保存各种系统信息
 */
void CAN_ReciverSysInfoCMD(uint8_t *Data)
{
    static u8 ComLED      = 0;
    static u8 KeyPosition = 0xFF;

    System.SystemInfo.SystemStatus = true;

    if (ComLED)
    {
        ComLED = 0;
        System.SystemInfo.KeyLedStatus &= ~KEY11LED;
    }
    else
    {
        ComLED = 1;
        System.SystemInfo.KeyLedStatus |= KEY11LED;
    }

    /***************************
    设置当前站
    ***********************/
    if ((Data[2] > 0x00) && (Data[2] <= MAXSTA))
    {
        if (System.SystemInfo.NowStation != Data[2])
        {
            System.SystemInfo.NowStation = Data[2];
            System.Run.TmpNowStation     = System.SystemInfo.NowStation;

            if ((System.Screen.MainScreen == 0) && (System.Screen.FuScreen == 0) && (System.SystemInfo.PauseStatus == 1))
            {
                LCD_PutCHString(24, 24, CHzm[System.SystemInfo.NowStation], 0);
                LCD_PutCHString(88, 48, Number[System.SystemInfo.NowStation], 0);
            }
        }
    }
    /********************
    设置起点站
    **********************/
    if ((Data[0] > 0x00) && (Data[0] <= MAXSTA))
    {
        if (System.SystemInfo.StartStation != Data[0])
        {
            System.SystemInfo.StartStation = Data[0];
            System.Run.TmpStartStation     = System.SystemInfo.StartStation;

            if ((System.Screen.MainScreen == 0) && (System.Screen.FuScreen == 0) && (System.SystemInfo.PauseStatus == 1))
            {
                LCD_PutCHString(64, 48, Number[System.SystemInfo.StartStation], 0);
            }
        }
    }

    /*************************
    设置终点站
    **************************/
    if ((Data[1] > 0x00) && (Data[1] <= MAXSTA))
    {
        if (System.SystemInfo.EndStation != Data[1])
        {
            System.SystemInfo.EndStation = Data[1];
            System.Run.TmpEndStation     = System.SystemInfo.EndStation;

            if ((System.Screen.MainScreen == 0) && (System.Screen.FuScreen == 0) && (System.SystemInfo.PauseStatus == 1))
            {
                LCD_PutCHString(112, 48, Number[System.SystemInfo.EndStation], 0);
            }
        }
    }

    if ((Data[3] > 0x00) && (Data[3] <= MAXSTA))
    {
        if (System.SystemInfo.NextStation != Data[3])
        {
            System.SystemInfo.NextStation = Data[3];
            System.Run.TmpNextStation     = System.SystemInfo.NextStation;
        }
    }

    /********************
    设置线路号
    ***********************/
    if (System.SystemInfo.LineNo != (Data[4] & 0x0F))
    {
        System.SystemInfo.LineNo = (Data[4] & 0x0F);
    }

    if (System.SystemInfo.BroadcastType != ((Data[4] & 0xF0) >> 4))
    {
        System.SystemInfo.BroadcastType = ((Data[4] & 0xF0) >> 4);
        if (System.SystemInfo.BroadcastType != 1)
        {
            // System.SystemInfo.KeyLedStatus &= ~KEY1LED;
            WriteLED(1, 0);
        }
        else
        {
            // System.SystemInfo.KeyLedStatus |= KEY1LED;
            WriteLED(1, 1);
        }
    }

    if (System.SystemInfo.ShangXiaxing != ((Data[5] & 0x80) >> 7))
    {
        System.SystemInfo.ShangXiaxing = ((Data[5] & 0x80) >> 7);
        System.Run.TmpShangXiaxing     = System.SystemInfo.ShangXiaxing;

        if ((System.Screen.MainScreen == 0) && (System.Screen.FuScreen == 0) && (System.SystemInfo.PauseStatus == 1))
        {
            LCD_PutCHString(0, 48, Directioninf[System.SystemInfo.ShangXiaxing], 0);
        }
    }

    if (System.SystemInfo.LieCheAddress == Data[6])
    {
        CAN_SendErrorCMD();
    }
    else if (Data[6] >= 0xA0 && Data[6] <= 0xA5)
    {
        if (System.SystemInfo.BroadcastMode != Data[6])
        {
            if (Data[6] == 0xA0)
            {
                Data[6] = 0xA3;
            }
            System.SystemInfo.BroadcastMode = Data[6];
            if (System.SystemInfo.BroadcastMode == 0xA2)  //无线电
            {
                if (System.SystemInfo.KeyMan == 1)
                {
                    WDI_CLR();  //清MAX813；使MCU控制AMC有效;
                    CLR_MANGB();
                }
            }
        }
    }

    if (System.SystemInfo.DoorStatus != ((Data[7] & (1 << 1)) >> 1))
    {
        System.SystemInfo.DoorStatus = ((Data[7] & (1 << 1)) >> 1);
    }

    if (((Data[7] & (1 << 4)) >> 4) <= 0x01)
    {
        System.SystemInfo.BroadcastStatus = (Data[7] & (1 << 4)) >> 4;

        if (System.SystemInfo.BroadcastStatus)
        {
            System.SystemInfo.KeyLedStatus |= KEY12LED;
        }
        else
        {
            System.SystemInfo.KeyLedStatus &= ~KEY12LED;
        }
    }

    if (KeyPosition != ((Data[7] & (1 << 6)) >> 6))
    {
        KeyPosition = ((Data[7] & (1 << 6)) >> 6);
        if (((Data[7] & (1 << 6)) >> 6) == 0x00)
        {
            if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4) == 0)
            {
                System.SystemInfo.KeyLedStatus &= ~KEY8LED;
                System.SystemInfo.DCUMatser = 2;
            }
            if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) == 0)
            {
                System.SystemInfo.KeyLedStatus |= KEY8LED;
                System.SystemInfo.DCUMatser = 1;
            }
        }
        else if (((Data[7] & (1 << 6)) >> 6) == 0x01)
        {
            if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4) == 0)
            {
                System.SystemInfo.KeyLedStatus |= KEY8LED;
                System.SystemInfo.DCUMatser = 1;
            }
            if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) == 0)
            {
                System.SystemInfo.KeyLedStatus &= ~KEY8LED;
                System.SystemInfo.DCUMatser = 2;
            }
        }
    }

    if (Data[7] & 0x10)
    {
        System.SystemInfo.KeyLedStatus |= KEY2LED;
    }
    else
    {
        System.SystemInfo.KeyLedStatus &= ~KEY2LED;
    }
}
/*
 * 设置越战信息
 */
void CAN_SendYueZhanCMD(unsigned char Pammater0, unsigned char Pammater1, unsigned char Pammater2, unsigned char Pammater3, unsigned char Pammater4,
                        unsigned char Pammater5, unsigned char Pammater6, unsigned char CMD)
{
    CAN.TxMessage[CAN.CANtxSP].IDE     = CAN_ID_EXT;
    CAN.TxMessage[CAN.CANtxSP].DLC     = 0x08;
    CAN.TxMessage[CAN.CANtxSP].ExtId   = (YUEZHANCMDHEAD << 8) | (System.SystemInfo.LieCheAddress);
    CAN.TxMessage[CAN.CANtxSP].RTR     = CAN_RTR_DATA;
    CAN.TxMessage[CAN.CANtxSP].Data[0] = Pammater0;
    CAN.TxMessage[CAN.CANtxSP].Data[1] = Pammater1;
    CAN.TxMessage[CAN.CANtxSP].Data[2] = Pammater2;
    CAN.TxMessage[CAN.CANtxSP].Data[3] = Pammater3;
    CAN.TxMessage[CAN.CANtxSP].Data[4] = Pammater4;
    CAN.TxMessage[CAN.CANtxSP].Data[5] = Pammater5;
    CAN.TxMessage[CAN.CANtxSP].Data[6] = Pammater6;
    CAN.TxMessage[CAN.CANtxSP].Data[7] = CMD;

    System.Run.TmpCmd[0] = Pammater0;
    System.Run.TmpCmd[1] = Pammater1;
    System.Run.TmpCmd[2] = Pammater2;
    System.Run.TmpCmd[3] = Pammater3;
    System.Run.TmpCmd[4] = Pammater4;
    System.Run.TmpCmd[5] = Pammater5;
    System.Run.TmpCmd[6] = Pammater6;
    System.Run.TmpCmd[7] = CMD;

    CAN_Transmit(CAN1, &CAN.TxMessage[CAN.CANtxSP]);

    if (!System.SystemInfo.CmdStatus)
    {
        TIM2_Open();
    }

    CAN.CANtxSP++;

    if (CAN.CANtxSP == CANMAXCONT)
    {
        CAN.CANtxSP = 0;
    }
}
/*
 * 保存越战信息
 */
void CAN_ReciverYueZhanCMD(uint8_t *Data)
{
    u8 i, j;
    u8 TmpData[8];
    for (i = 0; i < 8; i++)
    {
        TmpData[i] = Data[i];
    }

    if ((TmpData[7] == 0x03) || (TmpData[7] == 0x04) || (TmpData[7] == 0x05))
    {
        System.SystemInfo.TotalYueZhan = 0;
        System.Run.YunZhanCmdSta       = 1;
        for (i = 0; i < 7; i++)
        {
            for (j = 0; j < 8; j++)
            {
                if (TmpData[i] & 0x01)
                {
                    System.SystemInfo.YueZhan[System.SystemInfo.TotalYueZhan] = i * 8 + j - 2;
                    System.SystemInfo.TotalYueZhan++;
                }
                TmpData[i] >>= 1;
            }
        }
    }
    else if (Data[3] == 0x00)
    {
        System.Run.YunZhanCmdSta = 1;
        if ((System.Screen.MainScreen == 1) && (System.Screen.FuScreen == 4))
        {
            clear(0, 0, 192, 8);
            LCD_PutCHString(32, 3, CHinf[24], 0);
        }
    }
}

/*
 * 设置和查询主备机信息(上电的时候发一次)
 */
/*
 * 设置和查询主备机信息
 */
void CAN_FirstSendZhuBeiCMD(unsigned char CMD, unsigned char Pammater0, unsigned char Pammater1, unsigned char Pammater2, unsigned char Pammater3,
                            unsigned char Pammater4, unsigned char Pammater5)
{
    if (CMD == 0x01)
    {
        System.Run.FirstZhuBeiCmdSta = 0;
    }
    else if (CMD == 0x03)
    {
        System.Run.FirstCurrentHostCmdSta = 0;
    }
    CAN.TxMessage[CAN.CANtxSP].IDE     = CAN_ID_EXT;
    CAN.TxMessage[CAN.CANtxSP].DLC     = 0x08;
    CAN.TxMessage[CAN.CANtxSP].ExtId   = (ZHUBEICMDHEAD << 8) | (System.SystemInfo.LieCheAddress);
    CAN.TxMessage[CAN.CANtxSP].RTR     = CAN_RTR_DATA;
    CAN.TxMessage[CAN.CANtxSP].Data[0] = CMD;
    CAN.TxMessage[CAN.CANtxSP].Data[1] = Pammater0;
    CAN.TxMessage[CAN.CANtxSP].Data[2] = Pammater1;
    CAN.TxMessage[CAN.CANtxSP].Data[3] = Pammater2;
    CAN.TxMessage[CAN.CANtxSP].Data[4] = Pammater3;
    CAN.TxMessage[CAN.CANtxSP].Data[5] = Pammater4;
    CAN.TxMessage[CAN.CANtxSP].Data[6] = Pammater5;
    CAN.TxMessage[CAN.CANtxSP].Data[7] = 0x00;

    System.Run.TmpCmd[0] = CMD;
    System.Run.TmpCmd[1] = Pammater0;
    System.Run.TmpCmd[2] = Pammater1;
    System.Run.TmpCmd[3] = Pammater2;
    System.Run.TmpCmd[4] = Pammater3;
    System.Run.TmpCmd[5] = Pammater4;
    System.Run.TmpCmd[6] = Pammater5;

    CAN_Transmit(CAN1, &CAN.TxMessage[CAN.CANtxSP]);

    CAN.CANtxSP++;

    if (CAN.CANtxSP == CANMAXCONT)
    {
        CAN.CANtxSP = 0;
    }
}

/*
 * 设置和查询主备机信息
 */
void CAN_SendZhuBeiCMD(unsigned char CMD, unsigned char Pammater0, unsigned char Pammater1, unsigned char Pammater2, unsigned char Pammater3,
                       unsigned char Pammater4, unsigned char Pammater5)
{
    if (CMD == 0x01)
    {
        System.Run.ZhuBeiCmdSta = 0;
    }
    else if (CMD == 0x03)
    {
        System.Run.CurrentHostCmdSta = 0;
    }
    CAN.TxMessage[CAN.CANtxSP].IDE     = CAN_ID_EXT;
    CAN.TxMessage[CAN.CANtxSP].DLC     = 0x08;
    CAN.TxMessage[CAN.CANtxSP].ExtId   = (ZHUBEICMDHEAD << 8) | (System.SystemInfo.LieCheAddress);
    CAN.TxMessage[CAN.CANtxSP].RTR     = CAN_RTR_DATA;
    CAN.TxMessage[CAN.CANtxSP].Data[0] = CMD;
    CAN.TxMessage[CAN.CANtxSP].Data[1] = Pammater0;
    CAN.TxMessage[CAN.CANtxSP].Data[2] = Pammater1;
    CAN.TxMessage[CAN.CANtxSP].Data[3] = Pammater2;
    CAN.TxMessage[CAN.CANtxSP].Data[4] = Pammater3;
    CAN.TxMessage[CAN.CANtxSP].Data[5] = Pammater4;
    CAN.TxMessage[CAN.CANtxSP].Data[6] = Pammater5;
    CAN.TxMessage[CAN.CANtxSP].Data[7] = 0x00;

    System.Run.TmpCmd[0] = CMD;
    System.Run.TmpCmd[1] = Pammater0;
    System.Run.TmpCmd[2] = Pammater1;
    System.Run.TmpCmd[3] = Pammater2;
    System.Run.TmpCmd[4] = Pammater3;
    System.Run.TmpCmd[5] = Pammater4;
    System.Run.TmpCmd[6] = Pammater5;

    CAN_Transmit(CAN1, &CAN.TxMessage[CAN.CANtxSP]);

    if (!System.SystemInfo.CmdStatus)
    {
        TIM2_Open();
    }

    CAN.CANtxSP++;

    if (CAN.CANtxSP == CANMAXCONT)
    {
        CAN.CANtxSP = 0;
    }
}

void CAN_SendZhuJICMD(unsigned char CMD)
{
    CAN.TxMessage[CAN.CANtxSP].IDE     = CAN_ID_EXT;
    CAN.TxMessage[CAN.CANtxSP].DLC     = 0x08;
    CAN.TxMessage[CAN.CANtxSP].ExtId   = (ZHUJICMDHEAD << 8) | (System.SystemInfo.LieCheAddress);
    CAN.TxMessage[CAN.CANtxSP].RTR     = CAN_RTR_DATA;
    CAN.TxMessage[CAN.CANtxSP].Data[0] = CMD;
    CAN.TxMessage[CAN.CANtxSP].Data[1] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[2] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[3] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[4] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[5] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[6] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[7] = 0x00;

    System.Run.TmpCmd[0] = CMD;
    System.Run.TmpCmd[1] = 0x00;
    System.Run.TmpCmd[2] = 0x00;
    System.Run.TmpCmd[3] = 0x00;
    System.Run.TmpCmd[4] = 0x00;
    System.Run.TmpCmd[5] = 0x00;
    System.Run.TmpCmd[6] = 0x00;

    CAN_Transmit(CAN1, &CAN.TxMessage[CAN.CANtxSP]);

    if (!System.SystemInfo.CmdStatus)
    {
        TIM2_Open();
    }

    CAN.CANtxSP++;

    if (CAN.CANtxSP == CANMAXCONT)
    {
        CAN.CANtxSP = 0;
    }
}
/*
 * 保存主备机信息
 */
void CAN_ReciverQueryZhuBeiCMD(uint8_t *Data)
{
    if (Data[0] == 0x02)
    {
        if (!System.Run.FirstZhuBeiCmdSta)
        {
            System.Run.FirstZhuBeiCmdSta = 1;
        }
        System.Run.ZhuBeiCmdSta = 1;

        if (System.SystemInfo.LieCheHao != ((Data[2] * 100) + Data[3]))
        {
            System.SystemInfo.LieCheHao = ((Data[2] * 100) + Data[3]);
        }
    }
    else if (Data[0] == 0x04)
    {
        if (!System.Run.FirstCurrentHostCmdSta)
        {
            System.Run.FirstCurrentHostCmdSta = 1;
        }
        System.Run.CurrentHostCmdSta    = 1;
        System.SystemInfo.DCUCurrentChe = ((Data[1] * 100) + Data[2]);
    }
}

/*
 * 保存DCU主备机信息
 */
void CAN_ReciverZhuJICMD(uint8_t *Data)
{
    if (Data[0] == 0x01)
    {
        System.SystemInfo.KeyLedStatus |= KEY8LED;
        System.SystemInfo.DCUMatser = 1;
    }
    else if (Data[0] == 0x02)
    {
        System.SystemInfo.KeyLedStatus &= ~KEY8LED;
        System.SystemInfo.DCUMatser = 2;
    }
}

/*
 * 更新主备机信息
 */
void CAN_ReciverSetZhuBeiCMD(uint8_t *Data)
{
    if (Data[3] == 0x00)
    {
        System.Run.ZhuBeiCmdSta = 1;
    }
}

void CAN_SendStartStopCMD(unsigned char CMD)
{
    System.Run.StartStopSta            = 0;
    CAN.TxMessage[CAN.CANtxSP].IDE     = CAN_ID_EXT;
    CAN.TxMessage[CAN.CANtxSP].DLC     = 0x08;
    CAN.TxMessage[CAN.CANtxSP].ExtId   = (STARTSTOPCMDHEAD << 8) | (System.SystemInfo.LieCheAddress);
    CAN.TxMessage[CAN.CANtxSP].RTR     = CAN_RTR_DATA;
    CAN.TxMessage[CAN.CANtxSP].Data[0] = CMD;
    CAN.TxMessage[CAN.CANtxSP].Data[1] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[2] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[3] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[4] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[5] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[6] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[7] = 0x00;

    System.Run.TmpCmd[0] = CMD;

    CAN_Transmit(CAN1, &CAN.TxMessage[CAN.CANtxSP]);
    if (!System.SystemInfo.CmdStatus)
    {
        TIM2_Open();
    }

    CAN.CANtxSP++;

    if (CAN.CANtxSP == CANMAXCONT)
    {
        CAN.CANtxSP = 0;
    }
}

/*
 * 接受对讲信息
 */
void CAN_SendTalkCMD(unsigned char CMD)
{
    CAN.TxMessage[CAN.CANtxSP].IDE     = CAN_ID_EXT;
    CAN.TxMessage[CAN.CANtxSP].DLC     = 0x08;
    CAN.TxMessage[CAN.CANtxSP].ExtId   = (TALKCMDHEAD << 8) | (System.SystemInfo.LieCheAddress);
    CAN.TxMessage[CAN.CANtxSP].RTR     = CAN_RTR_DATA;
    CAN.TxMessage[CAN.CANtxSP].Data[0] = CMD;
    CAN.TxMessage[CAN.CANtxSP].Data[1] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[2] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[3] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[4] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[5] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[6] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[7] = 0x00;

    CAN_Transmit(CAN1, &CAN.TxMessage[CAN.CANtxSP]);

    if (!System.SystemInfo.CmdStatus)
    {
        TIM2_Open();
    }

    CAN.CANtxSP++;

    if (CAN.CANtxSP == CANMAXCONT)
    {
        CAN.CANtxSP = 0;
    }
}

/*
 * 接受对讲信息
 */
void CAN_ReciverTalkCMD(uint8_t *Data)
{
    if (Data[0] == 0x02)
    {
        System.SystemInfo.KeyLedStatus |= KEY3LED;
        if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4) == 0)
        {
            GPIO_SetBits(GPIOB, GPIO_Pin_8 | GPIO_Pin_9);
        }
        if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) == 0)
        {
            GPIO_SetBits(GPIOB, GPIO_Pin_8);
        }
    }
    else if (Data[0] == 0x03)
    {
        System.SystemInfo.KeyLedStatus &= ~KEY3LED;
        GPIO_ResetBits(GPIOB, GPIO_Pin_8 | GPIO_Pin_9);
    }
}
/*
 * 开机查询音量大小
 */
void CAN_FirstSendQueryVoiceCMD(unsigned char CMD)
{
    System.Run.FirstQueryVoiceCmdSta   = 0;
    CAN.TxMessage[CAN.CANtxSP].IDE     = CAN_ID_EXT;
    CAN.TxMessage[CAN.CANtxSP].DLC     = 0x08;
    CAN.TxMessage[CAN.CANtxSP].ExtId   = (QUERYVOICECMDHEAD << 8) | (System.SystemInfo.LieCheAddress);
    CAN.TxMessage[CAN.CANtxSP].RTR     = CAN_RTR_DATA;
    CAN.TxMessage[CAN.CANtxSP].Data[0] = CMD;
    CAN.TxMessage[CAN.CANtxSP].Data[1] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[2] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[3] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[4] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[5] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[6] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[7] = 0x00;

    System.Run.TmpCmd[0] = CMD;

    CAN_Transmit(CAN1, &CAN.TxMessage[CAN.CANtxSP]);

    CAN.CANtxSP++;

    if (CAN.CANtxSP == CANMAXCONT)
    {
        CAN.CANtxSP = 0;
    }
}
/*
 * 查询音量大小
 */
void CAN_SendQueryVoiceCMD(unsigned char CMD)
{
    System.Run.QueryVoiceCmdSta        = 0;
    CAN.TxMessage[CAN.CANtxSP].IDE     = CAN_ID_EXT;
    CAN.TxMessage[CAN.CANtxSP].DLC     = 0x08;
    CAN.TxMessage[CAN.CANtxSP].ExtId   = (QUERYVOICECMDHEAD << 8) | (System.SystemInfo.LieCheAddress);
    CAN.TxMessage[CAN.CANtxSP].RTR     = CAN_RTR_DATA;
    CAN.TxMessage[CAN.CANtxSP].Data[0] = CMD;
    CAN.TxMessage[CAN.CANtxSP].Data[1] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[2] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[3] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[4] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[5] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[6] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[7] = 0x00;

    System.Run.TmpCmd[0] = CMD;

    CAN_Transmit(CAN1, &CAN.TxMessage[CAN.CANtxSP]);

    if (!System.SystemInfo.CmdStatus)
    {
        TIM2_Open();
    }

    CAN.CANtxSP++;

    if (CAN.CANtxSP == CANMAXCONT)
    {
        CAN.CANtxSP = 0;
    }
}

/*
 * 查询音量大小
 */
void CAN_ReciverQueryVoiceCMD(uint8_t *Data)
{
    if (Data[0] == 0x01)
    {
        if (!System.Run.FirstQueryVoiceCmdSta)
        {
            System.Run.FirstQueryVoiceCmdSta = 1;
        }
        System.Run.QueryVoiceCmdSta = 1;
        System.Volume.Man_Volume    = Data[1];
        System.Volume.MP3_Volume    = Data[2];
        System.Volume.Radio_Volume  = Data[3];
        System.Volume.JT_Volume     = Data[5];
        System.Volume.Talk_Volume   = Data[6];
    }
}

/*
 * 设置紧急广播编号
 */
void CAN_SendSetVoiceCMD(unsigned char CMD, unsigned char VoiceH, unsigned char VoiceL)
{
    System.Run.SetVoiceCmdSta          = 0;
    CAN.TxMessage[CAN.CANtxSP].IDE     = CAN_ID_EXT;
    CAN.TxMessage[CAN.CANtxSP].DLC     = 0x08;
    CAN.TxMessage[CAN.CANtxSP].ExtId   = (SETVOICECMDHEAD << 8) | (System.SystemInfo.LieCheAddress);
    CAN.TxMessage[CAN.CANtxSP].RTR     = CAN_RTR_DATA;
    CAN.TxMessage[CAN.CANtxSP].Data[0] = CMD;
    CAN.TxMessage[CAN.CANtxSP].Data[1] = VoiceH;
    CAN.TxMessage[CAN.CANtxSP].Data[2] = VoiceL;
    CAN.TxMessage[CAN.CANtxSP].Data[3] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[4] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[5] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[6] = 0x00;
    CAN.TxMessage[CAN.CANtxSP].Data[7] = 0x00;

    System.Run.TmpCmd[0] = CMD;
    System.Run.TmpCmd[1] = VoiceH;
    System.Run.TmpCmd[2] = VoiceL;

    CAN_Transmit(CAN1, &CAN.TxMessage[CAN.CANtxSP]);

    if (!System.SystemInfo.CmdStatus)
    {
        TIM2_Open();
    }

    CAN.CANtxSP++;

    if (CAN.CANtxSP == CANMAXCONT)
    {
        CAN.CANtxSP = 0;
    }
}

void CAN_SendSetVolumeCMD(unsigned char CMD, unsigned char ManVolumeH, unsigned char ManVolumeL, unsigned char MP3VolumeH, unsigned char MP3VolumeL,
                          unsigned char RadioVolumeH, unsigned char RadioVolumeL)
{
    System.Run.SetVolumeCmdSta         = 0;
    CAN.TxMessage[CAN.CANtxSP].IDE     = CAN_ID_EXT;
    CAN.TxMessage[CAN.CANtxSP].DLC     = 0x08;
    CAN.TxMessage[CAN.CANtxSP].ExtId   = (SETVOLUMECMDHEAD << 8) | (System.SystemInfo.LieCheAddress);
    CAN.TxMessage[CAN.CANtxSP].RTR     = CAN_RTR_DATA;
    CAN.TxMessage[CAN.CANtxSP].Data[0] = CMD;
    CAN.TxMessage[CAN.CANtxSP].Data[1] = ManVolumeH;
    CAN.TxMessage[CAN.CANtxSP].Data[2] = ManVolumeL;
    CAN.TxMessage[CAN.CANtxSP].Data[3] = MP3VolumeH;
    CAN.TxMessage[CAN.CANtxSP].Data[4] = MP3VolumeL;
    CAN.TxMessage[CAN.CANtxSP].Data[5] = RadioVolumeH;
    CAN.TxMessage[CAN.CANtxSP].Data[6] = RadioVolumeL;
    CAN.TxMessage[CAN.CANtxSP].Data[7] = 0x00;

    System.Run.TmpCmd[0] = CMD;
    System.Run.TmpCmd[1] = ManVolumeH;
    System.Run.TmpCmd[2] = ManVolumeL;
    System.Run.TmpCmd[3] = MP3VolumeH;
    System.Run.TmpCmd[4] = MP3VolumeL;
    System.Run.TmpCmd[5] = RadioVolumeH;
    System.Run.TmpCmd[6] = RadioVolumeL;

    CAN_Transmit(CAN1, &CAN.TxMessage[CAN.CANtxSP]);

    if (!System.SystemInfo.CmdStatus)
    {
        TIM2_Open();
    }

    CAN.CANtxSP++;

    if (CAN.CANtxSP == CANMAXCONT)
    {
        CAN.CANtxSP = 0;
    }
}

void CAN_ReciverSetVoiceCMD(uint8_t *Data)
{
    if (Data[0] == 0x01)
    {
        System.Run.SetVoiceCmdSta = 1;

        WDI_CLR();  //清MAX813；使MCU控制AMC有效;
        CLR_MANGB();
    }
}

void CAN_ReciverSetVolumeCMD(uint8_t *Data)
{
    if (Data[3] == 0x00)
    {
        System.Run.SetVolumeCmdSta = 1;

        if ((System.Screen.MainScreen == 4) && (System.Screen.FuScreen == 3))
        {
            System.Volume.Man_Volume   = System.Run.TmpMan_Volume;
            System.Volume.MP3_Volume   = System.Run.TmpMP3_Volume;
            System.Volume.Radio_Volume = System.Run.TmpRadio_Volume;

            System.Screen.MainScreen = 0;
            System.Screen.FuScreen   = 0;
            DB3812WriteVolume(System.Volume.Talk_Volume, System.Volume.Man_Volume, ChxVolume);
            //       KeyLed(ALLKEYLEDOFF);
            System.SystemInfo.KeyLedStatus &= ~KEY14LED;
            clear(0, 0, 192, 8);
            LCD_DIS_MAIN();
        }
    }
}

/*
 * 查询关停是否成功
 */
void CAN_ReciverStartStopCMD(uint8_t *Data)
{
    if (Data[3] == 0x00)
    {
        System.Run.StartStopSta = 1;
    }

    if (Data[0] == 0x03)
    {
        System.SystemInfo.KeyLedStatus |= KEY6LED;
        System.SystemInfo.PauseStatus = 2;

        clear(0, 0, 192, 8);
        LCD_PutCHString(72, 4, CHinf[28], 0);
    }
    else if (Data[0] == 0x04)
    {
        System.SystemInfo.KeyLedStatus &= ~KEY6LED;
        System.SystemInfo.PauseStatus = 1;

        clear(0, 0, 192, 8);
        LCD_DIS_MAIN();
    }
}

void CAN_ReciverConnectInfoCMD(uint8_t *Data)
{
    u16 TmpCheHao[3] = {0};
    u8  i;
    if (Data[0] == 0x01)
    {
        System.SystemInfo.ChongLianNum = 1;
        System.SystemInfo.LieCheHaoA   = 0;
        System.SystemInfo.LieCheHaoB   = 0;
        System.SystemInfo.LieCheHaoC   = 0;
        //    memset((void *)System.SystemInfo.LieCheHaoAString, 0, 5);
        //    memset((void *)System.SystemInfo.LieCheHaoBString, 0, 5);
        //    memset((void *)System.SystemInfo.LieCheHaoCString, 0, 5);
    }
    else if (Data[0] == 0x02)
    {
        System.SystemInfo.ChongLianNum = 2;

        TmpCheHao[0] = Data[2] * 100 + Data[3];
        TmpCheHao[1] = Data[4] * 100 + Data[5];

        for (i = 0; i < 2; i++)
        {
            if (System.SystemInfo.LieCheHao % 2)
            {
                if (System.SystemInfo.LieCheHao == TmpCheHao[i])
                {
                    memcpy((void *)&TmpCheHao[i], (void *)&TmpCheHao[i + 1], 2 - i);
                }
            }
            else
            {
                if ((System.SystemInfo.LieCheHao - 1) == TmpCheHao[i])
                {
                    memcpy((void *)&TmpCheHao[i], (void *)&TmpCheHao[i + 1], 2 - i);
                }
            }
        }

        System.SystemInfo.LieCheHaoA = TmpCheHao[0];

        //    System.SystemInfo.LieCheHaoAString[0] = (System.SystemInfo.LieCheHaoA >> 12) + 0x30;
        //    System.SystemInfo.LieCheHaoAString[1] = ((System.SystemInfo.LieCheHaoA & 0xF00) >> 8) + 0x30;
        //    System.SystemInfo.LieCheHaoAString[2] = (((System.SystemInfo.LieCheHaoA & 0xF0) >> 4)) + 0x30;
        //    System.SystemInfo.LieCheHaoAString[3] = ((System.SystemInfo.LieCheHaoA & 0x0F)) + 0x30;
    }
    else if (Data[0] == 0x03)
    {
        System.SystemInfo.ChongLianNum = 3;
        TmpCheHao[0]                   = Data[2] * 100 + Data[3];
        TmpCheHao[1]                   = Data[4] * 100 + Data[5];
        TmpCheHao[2]                   = Data[6] * 100 + Data[7];

        for (i = 0; i < 3; i++)
        {
            if (System.SystemInfo.LieCheHao % 2)
            {
                if (System.SystemInfo.LieCheHao == TmpCheHao[i])
                {
                    memcpy((void *)&TmpCheHao[i], (void *)&TmpCheHao[i + 1], 3 - i);
                }
            }
            else
            {
                if ((System.SystemInfo.LieCheHao - 1) == TmpCheHao[i])
                {
                    memcpy((void *)&TmpCheHao[i], (void *)&TmpCheHao[i + 1], 3 - i);
                }
            }
        }

        System.SystemInfo.LieCheHaoA = TmpCheHao[0];
        System.SystemInfo.LieCheHaoB = TmpCheHao[1];
    }
}

void CAN_Reciver_data_Analyse(void)
{
    if (CAN.CANrxSP != CAN.CANrxProcessSP)
    {
        switch ((u16)(CAN.RxMessage[CAN.CANrxProcessSP].ExtId >> 16))
        {
            case 0x188F:  //系统信息
                TIM_SetCounter(TIM3, 0);
                CAN_ReciverSysInfoCMD(CAN.RxMessage[CAN.CANrxProcessSP].Data);

                if ((CAN.RxMessage[CAN.CANrxProcessSP].ExtId & 0xFF) == 0x30)  // MC1
                {
                    System.SystemInfo.Matser = 1;
                    if (System.SystemInfo.LieCheAddress == HeadID)
                    {
                        if (System.SystemInfo.COMMatser != 1)
                        {
                            System.SystemInfo.COMMatser = 1;
                            if ((System.Screen.MainScreen == 0) && (System.Screen.FuScreen == 0))
                            {
                                LCD_PutCHString(0, 4, CHinf[System.SystemInfo.COMMatser - 1], 0);
                            }
                        }
                        System.SystemInfo.KeyLedStatus |= KEY9LED;
                    }
                    else
                    {
                        if (System.SystemInfo.COMMatser != 2)
                        {
                            System.SystemInfo.COMMatser = 2;
                            if ((System.Screen.MainScreen == 0) && (System.Screen.FuScreen == 0))
                            {
                                LCD_PutCHString(0, 4, CHinf[System.SystemInfo.COMMatser - 1], 0);
                            }
                        }
                        System.SystemInfo.KeyLedStatus &= ~KEY9LED;
                    }
                }
                else  // MC2
                {
                    System.SystemInfo.Matser = 2;
                    if (System.SystemInfo.LieCheAddress == TailID)
                    {
                        if (System.SystemInfo.COMMatser != 1)
                        {
                            System.SystemInfo.COMMatser = 1;
                            if ((System.Screen.MainScreen == 0) && (System.Screen.FuScreen == 0))
                            {
                                LCD_PutCHString(0, 4, CHinf[System.SystemInfo.COMMatser - 1], 0);
                            }
                        }
                        System.SystemInfo.KeyLedStatus |= KEY9LED;
                    }
                    else
                    {
                        if (System.SystemInfo.COMMatser != 2)
                        {
                            System.SystemInfo.COMMatser = 2;
                            if ((System.Screen.MainScreen == 0) && (System.Screen.FuScreen == 0))
                            {
                                LCD_PutCHString(0, 4, CHinf[System.SystemInfo.COMMatser - 1], 0);
                            }
                        }
                        System.SystemInfo.KeyLedStatus &= ~KEY9LED;
                    }
                }
                break;
            case 0x18C9:  //越战信息
                CAN_ReciverYueZhanCMD(CAN.RxMessage[CAN.CANrxProcessSP].Data);
                break;
            case 0x188B:  //主备机
                CAN_ReciverQueryZhuBeiCMD(CAN.RxMessage[CAN.CANrxProcessSP].Data);
                break;
            case 0x18BB:  // DCU主备机
                CAN_ReciverZhuJICMD(CAN.RxMessage[CAN.CANrxProcessSP].Data);
                break;
            case 0x188D:  //对讲
                CAN_ReciverTalkCMD(CAN.RxMessage[CAN.CANrxProcessSP].Data);
                break;
            case 0x188C:  //音量查询
                CAN_ReciverQueryVoiceCMD(CAN.RxMessage[CAN.CANrxProcessSP].Data);
                break;
            case 0x18B1:  //报警
                CAN_ReciverAlarmCMD(CAN.RxMessage[CAN.CANrxProcessSP].Data);
                break;
            case 0x1889:
                CAN_ReciverStartStopCMD(CAN.RxMessage[CAN.CANrxProcessSP].Data);
                break;
            case 0x1864:
                CAN_ReciverConnectInfoCMD(CAN.RxMessage[CAN.CANrxProcessSP].Data);
                break;
            case 0x1885:
                CAN_ReciverSetVoiceCMD(CAN.RxMessage[CAN.CANrxProcessSP].Data);
                break;
            case 0x178B:
                CAN_ReciverSetZhuBeiCMD(CAN.RxMessage[CAN.CANrxProcessSP].Data);
                break;
                //            case 0x1785:
                //                CAN_ReciverSetVoiceCMD(CAN.RxMessage[CAN.CANrxProcessSP].Data);
                //                break;
            case 0x178C:
                CAN_ReciverSetVolumeCMD(CAN.RxMessage[CAN.CANrxProcessSP].Data);
                break;
                //            case 0x1789:
                //                CAN_ReciverStartStopCMD(CAN.RxMessage[CAN.CANrxProcessSP].Data);
                //                break;
            case 0x1792:
                CAN_ReciverLineInfoCMD(CAN.RxMessage[CAN.CANrxProcessSP].Data);
                break;
            case 0x17C9:
                CAN_ReciverYueZhanCMD(CAN.RxMessage[CAN.CANrxProcessSP].Data);
            default:
                break;
        }

        CAN.CANrxProcessSP++;
        if (CAN.CANrxProcessSP == CANMAXCONT)
        {
            CAN.CANrxProcessSP = 0;
        }
    }
}

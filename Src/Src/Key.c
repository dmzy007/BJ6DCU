#include "Key.h"
#include "bsp.h"
#include "can_drv.h"
#include "mcp23s17.h"

u8 Station         = 0;
u8 DefaultValue[5] = {0, 0, 1, 0, 13};

void ZLG7289_init(void)
{
    ZLG7289A_DIO_DIR_OUT();

    ZLG7289A_RST_OUTPUT_L();
    delay_us(50);
    delay_us(50);
    delay_us(50);
    ZLG7289A_RST_OUTPUT_H();

    ZLG7289A_CS_OUTPUT_H();
    ZLG7289A_CLK_OUTPUT_L();
    ZLG7289A_DIO_OUTPUT_L();

    ZLG7289A_CS_OUTPUT_L();
    delay_us(50);
    ZLG7289_SPI_Write(0xa4);

    ZLG7289A_CS_OUTPUT_H();
    ZLG7289A_CLK_OUTPUT_L();
    ZLG7289A_DIO_OUTPUT_H();
}

/*
函数：ZLG7289_SPI_Write()
功能：向SPI 总线写入1 个字节的数据
参数：
dat：要写入的数据
*/
void ZLG7289_SPI_Write(char dat)
{
    unsigned char t = 8;
    ZLG7289A_DIO_DIR_OUT();
    do
    {
        if ((dat & 0x80) == 0x80)
        {
            ZLG7289A_DIO_OUTPUT_H();
        }
        else
        {
            ZLG7289A_DIO_OUTPUT_L();
        }
        dat = dat << 1;
        ZLG7289A_CLK_OUTPUT_H();
        delay_us(20);
        delay_us(20);
        ZLG7289A_CLK_OUTPUT_L();
        delay_us(20);
    } while (--t);
}

/*
函数：ZLG7289_SPI_Read()
功能：从SPI 总线读取1 个字节的数据
返回：
读取到的数据
*/
unsigned char ZLG7289_SPI_Read(void)  // reentrant
{
    unsigned char t = 8, dat = 0;
    ZLG7289A_DIO_DIR_IN();
    delay_us(30);
    do
    {
        ZLG7289A_CLK_OUTPUT_H();
        delay_us(30);
        dat = dat << 1;
        if (ZLG7289A_DIOin)
        {
            dat |= 0x01;
        }
        ZLG7289A_CLK_OUTPUT_L();
        delay_us(30);
    } while (--t);

    ZLG7289A_DIO_DIR_OUT();
    return (dat);
}

unsigned char zlg7289_key(void)
{
    unsigned char KEY;
    ZLG7289A_CS_OUTPUT_L();  //片选有效

    delay_us(50);
    ZLG7289_SPI_Write(0x15);

    delay_us(500);
    KEY = ZLG7289_SPI_Read();
    ZLG7289A_CS_OUTPUT_H();  //片选

    delay_us(50);
    return (KEY);
}
/*
 * 按键IO初始化
 */
void KeyLed_IO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
#if 1
    /* 设置7289*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
#else
    /* 设置7289*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
#endif
    ZLG7289_init();
}

/*
 * 按键消抖
 */
void Key_Delay(int t)
{
    int i, j;

    for (i = 0; i < t; i++)
    {
        for (j = 0; j < 100; j++)
        {
        }
    }
}

/*
 * 扫描按键
 */
void ScanKey(void)
{
    static unsigned char Key_complete = 0;
    static unsigned char NEW_Key      = 0;

    if (Key_complete == 0)
    {
        if (ZLG7289A_KEYin == 0)
        {
            NEW_Key      = zlg7289_key();
            Key_complete = 1;
        }
    }
    else
    {
        if (ZLG7289A_KEYin)
        {
            Key_complete = 0;
            if (NEW_Key > 45)  //最大键值不能大于62，否则视为无效
            {
                NEW_Key      = 0;
                Key_complete = 0;
                return;
            }
            else
            {
                Keyfunction(NEW_Key);  //键值转换
                Key_complete = 0;
                NEW_Key      = 0;  //键值处理完,清键值和标志
            }
        }
    }
}

// 74HC595  PD11--STCP   PD15--SHCP   PD13--DATA
void KeyRefeshLed(vu16 Status)
{
    vu8 i;

    for (i = 0; i < 16; i++)
    {
        GPIOD->ODR &= ~(1 << 15);
        if ((Status & 0x8000) == 0x8000)
        {
            GPIOD->ODR &= ~(1 << 13);
        }
        else
        {
            GPIOD->ODR |= (1 << 13);
        }
        Status = (Status << 1);
        GPIOD->ODR |= (1 << 15);
    }
    GPIO_ResetBits(GPIOD, GPIO_Pin_11);
    Key_Delay(10);
    GPIO_SetBits(GPIOD, GPIO_Pin_11);
}

void KeepInterCom(void)
{
    System.Run.DriverTalk = 2;
    System.SystemInfo.KeyLedStatus |= KEY3LED;
    if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4) == 0)
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_8 | GPIO_Pin_9);
    }
    if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) == 0)
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_8);
    }
    CAN_SendTalkCMD(0x02);
}

void CancelInterCom(void)
{
    System.SystemInfo.KeyLedStatus &= ~KEY3LED;
    GPIO_ResetBits(GPIOB, GPIO_Pin_8 | GPIO_Pin_9);
    CAN_SendTalkCMD(0x03);
}

void Keyfunction(u8 KeyID)
{
    switch (KeyID)
    {
        case 0x00:  //自动按键
        {
            if ((System.SystemInfo.DCUMatser == 1) && (System.SystemInfo.PauseStatus == 1))
            {
                if (System.SystemInfo.BroadcastType == 1)
                {
                    CAN_SendSetVoiceCMD(0x06, 0x00, 0x00);
                }
                else
                {
                    CAN_SendSetVoiceCMD(0x08, 0x00, 0x00);
                }
            }
        }
        break;
        case 0x01:  //对讲按键
        {
            if (System.SystemInfo.PauseStatus == 1)
            {
                if ((System.SystemInfo.BroadcastMode != 0xA1) && (System.SystemInfo.BroadcastMode != 0xA2))
                {
                    if (System.Run.AlarmTalk == 2)
                    {
                        System.Run.AlarmTalk = 0;
                        CAN_SendAlarmCMD(0x03, System.SystemInfo.BaoJingCheXiangID[0]);
                        DecreAlarmID(System.SystemInfo.BaoJingCheXiangID[0]);
                        System.SystemInfo.KeyLedStatus &= ~KEY5LED;

                        GPIO_ResetBits(GPIOB, GPIO_Pin_8 | GPIO_Pin_9);
                    }

                    if (System.Run.DriverTalk == 0)
                    {
                        System.Run.DriverTalk = 1;
                        CAN_SendTalkCMD(0x02);

                        System.SystemInfo.KeyLedStatus |= KEY3LED;
                        GPIO_SetBits(GPIOB, GPIO_Pin_8 | GPIO_Pin_9);
                    }
                    else if (System.Run.DriverTalk == 1)
                    {
                        System.Run.DriverTalk = 0;
                        CAN_SendTalkCMD(0x03);

                        System.SystemInfo.KeyLedStatus &= ~KEY3LED;
                        GPIO_ResetBits(GPIOB, GPIO_Pin_8 | GPIO_Pin_9);
                    }
                }
            }
        }
        break;
        case 0x02:  //报警按键
        {
            if ((System.SystemInfo.DCUMatser == 1) && (System.SystemInfo.PauseStatus == 1))
            {
                if ((System.SystemInfo.BroadcastMode != 0xA1) && (System.SystemInfo.BroadcastMode != 0xA2))
                {
                    if (System.SystemInfo.AlarmTotal)
                    {
                        if (System.Run.AlarmTalk == 1)
                        {
                            CancelInterCom();

                            System.Run.AlarmTalk = 2;

                            CAN_SendAlarmCMD(0x02, System.SystemInfo.BaoJingCheXiangID[0]);
                            TIM4_Close();
                            GPIO_ResetBits(GPIOD, GPIO_Pin_3);
                            System.SystemInfo.KeyLedStatus |= KEY5LED;

                            GPIO_SetBits(GPIOB, GPIO_Pin_8 | GPIO_Pin_9);
                        }
                        else if (System.Run.AlarmTalk == 2)
                        {
                            System.Run.AlarmTalk = 0;
                            CAN_SendAlarmCMD(0x03, System.SystemInfo.BaoJingCheXiangID[0]);
                            DecreAlarmID(System.SystemInfo.BaoJingCheXiangID[0]);
                            System.SystemInfo.KeyLedStatus &= ~KEY5LED;

                            GPIO_ResetBits(GPIOB, GPIO_Pin_8 | GPIO_Pin_9);
                            KeepInterCom();
                        }
                    }
                }
            }
        }
        break;
        case 0x08:  //开始按键
        {
            if ((System.SystemInfo.DCUMatser == 1) && (System.SystemInfo.PauseStatus == 1))
            {
                CAN_SendStartStopCMD(0x01);
            }
        }
        break;
        case 0x09:  //停止按键
        {
            if ((System.SystemInfo.DCUMatser == 1) && (System.SystemInfo.PauseStatus == 1))
            {
                CAN_SendStartStopCMD(0x00);
            }
        }
        break;
        case 0x0A:  //监听按键
        {
        }
        break;
        case 0x0B:  //主控按键
        {
            if ((System.SystemInfo.DCUMatser == 1) && (System.SystemInfo.PauseStatus == 1))
            {
                if (System.SystemInfo.Matser != System.SystemInfo.COMMatser)
                {
                    CAN_SendZhuBeiCMD(0x00, (System.SystemInfo.Matser == 1) ? 1 : 6, 0x00, 0x00, (System.SystemInfo.Matser == 1) ? 6 : 1, 0x00, 0x00);
                }
            }
        }
        break;
        case 0x0C:  //静音按键
        {
        }
        break;
        case 0x10:  //数字1按键
        {
            if ((System.SystemInfo.DCUMatser == 1) && (System.SystemInfo.PauseStatus == 1))
            {
                ShowNum(1);
            }
        }
        break;
        case 0x11:  //数字2按键
        {
            if ((System.SystemInfo.DCUMatser == 1) && (System.SystemInfo.PauseStatus == 1))
            {
                ShowNum(2);
            }
        }
        break;
        case 0x12:  //数字3按键
        {
            if ((System.SystemInfo.DCUMatser == 1) && (System.SystemInfo.PauseStatus == 1))
            {
                ShowNum(3);
            }
        }
        break;
        case 0x13:  //上/下按键
        {
            if ((System.SystemInfo.DCUMatser == 1) && (System.SystemInfo.PauseStatus == 1))
            {
                CAN_SendLineInfoCMD(System.SystemInfo.StartStation, System.SystemInfo.EndStation, System.SystemInfo.NowStation,
                                    System.SystemInfo.LineNo, System.SystemInfo.ShangXiaxing ? 0 : 1);
            }
        }
        break;
        case 0x14:  //起点按键
        {
            if ((System.SystemInfo.DCUMatser == 1) && (System.SystemInfo.PauseStatus == 1))
            {
                if ((System.Screen.MainScreen == 0) && (System.Screen.FuScreen == 1))
                {
                    System.Screen.MainScreen = 0;
                    System.Screen.FuScreen   = 2;

                    clear(0, 0, 127, 15);
                    LCD_PutCHString(0, 0, (const unsigned char *)"设置起点站", 0);

                    if (!System.Run.TIM2Flag)
                    {
                        System.Run.TIM2Flag = 2;
                        TIM2_Open();
                        Station = System.Run.KEYSelect;
                    }
                }
                else
                {
                    System.Screen.MainScreen = 0;
                    System.Screen.FuScreen   = 1;

                    System.Run.KEYSelect = System.SystemInfo.StartStation;

                    if (System.Run.TIM2Flag)
                    {
                        System.Run.TIM2Flag = 0;
                        TIM2_Close();
                    }

                    clear(0, 0, 127, 63);
                    LCD_PutCHString(40, 0, (const unsigned char *)"起点站", 0);
                    Images(4, 2, 120, 4, CHzm[System.SystemInfo.StartStation], 360);
                    LCD_PutCHString(64, 48, Number[System.SystemInfo.StartStation], 0);
                }
            }
        }
        break;
        case 0x18:  //数字4按键
        {
            if ((System.SystemInfo.DCUMatser == 1) && (System.SystemInfo.PauseStatus == 1))
            {
                ShowNum(4);
            }
        }
        break;
        case 0x19:  //数字5按键
        {
            if ((System.SystemInfo.DCUMatser == 1) && (System.SystemInfo.PauseStatus == 1))
            {
                ShowNum(5);
            }
        }
        break;
        case 0x1A:  //数字6按键
        {
            if ((System.SystemInfo.DCUMatser == 1) && (System.SystemInfo.PauseStatus == 1))
            {
                ShowNum(6);
            }
        }
        break;
        case 0x1B:  //快/慢按键
        {
            if ((System.SystemInfo.DCUMatser == 1) && (System.SystemInfo.PauseStatus == 1))
            {
            }
        }
        break;
        case 0x1C:  //终点按键
        {
            if ((System.SystemInfo.DCUMatser == 1) && (System.SystemInfo.PauseStatus == 1))
            {
                if ((System.Screen.MainScreen == 0) && (System.Screen.FuScreen == 3))
                {
                    System.Screen.MainScreen = 0;
                    System.Screen.FuScreen   = 4;

                    clear(0, 0, 127, 15);
                    LCD_PutCHString(0, 0, (const unsigned char *)"设置终点站", 0);

                    if (!System.Run.TIM2Flag)
                    {
                        System.Run.TIM2Flag = 3;
                        TIM2_Open();
                        Station = System.Run.KEYSelect;
                    }
                }
                else
                {
                    System.Screen.MainScreen = 0;
                    System.Screen.FuScreen   = 3;

                    System.Run.KEYSelect = System.SystemInfo.EndStation;

                    if (System.Run.TIM2Flag)
                    {
                        System.Run.TIM2Flag = 0;
                        TIM2_Close();
                    }

                    clear(0, 0, 127, 63);
                    LCD_PutCHString(40, 0, (const unsigned char *)"终点站", 0);
                    Images(4, 2, 120, 4, CHzm[System.SystemInfo.EndStation], 360);
                    LCD_PutCHString(64, 48, Number[System.SystemInfo.EndStation], 0);
                }
            }
        }
        break;
        case 0x20:  //数字7按键
        {
            if ((System.SystemInfo.DCUMatser == 1) && (System.SystemInfo.PauseStatus == 1))
            {
                ShowNum(7);
            }
        }
        break;
        case 0x21:  //数字8按键
        {
            if ((System.SystemInfo.DCUMatser == 1) && (System.SystemInfo.PauseStatus == 1))
            {
                ShowNum(8);
            }
        }
        break;
        case 0x22:  //数字9按键
        {
            if ((System.SystemInfo.DCUMatser == 1) && (System.SystemInfo.PauseStatus == 1))
            {
                ShowNum(9);
            }
        }
        break;
        case 0x23:  // F功能键
        {
            if ((System.SystemInfo.DCUMatser == 1) && (System.SystemInfo.PauseStatus == 1))
            {
                if ((System.Screen.MainScreen == 1) && (System.Screen.FuScreen == 0))
                {
                    System.Screen.MainScreen = 1;
                    System.Screen.FuScreen   = 1;

                    System.Run.KEYSelect = 1;

                    System.Run.TmpMasterMode = System.SystemInfo.MasterMode;
                    System.Run.TmpGBMode     = System.SystemInfo.GBMode;
                    System.Run.TmpWorkMode   = System.SystemInfo.WorkMode;
                    System.Run.TmpMonoVolume = System.SystemInfo.MonoVolume;

                    clear(0, 0, 127, 63);
                    LCD_DIS_Setting(System.Run.KEYSelect);
                }
                // else if ((System.Screen.MainScreen == 1) && (System.Screen.FuScreen == 1))
                // {
                //     System.Screen.MainScreen = 1;
                //     System.Screen.FuScreen   = 2;

                //     System.Run.KEYSelect    = 1;

                //     clear(0, 0, 127, 63);

                //     LCD_PutCHString(24, 8, "请输入密码", 0);
                //     // LCD_PutCHString(40, 24, "******", 0);
                // }
                else
                {
                    System.Screen.MainScreen = 1;
                    System.Screen.FuScreen   = 0;

                    System.Run.KEYSelect = 1;

                    clear(0, 0, 127, 63);
                    LCD_DIS_EmerGB(System.Run.KEYSelect);
                }
            }
        }
        break;
        case 0x24:  //越站按键
        {
            if ((System.SystemInfo.DCUMatser == 1) && (System.SystemInfo.PauseStatus == 1))
            {
                if ((System.Screen.MainScreen == 3) && (System.Screen.FuScreen == 0))
                {
                    if (System.Run.TIM2Flag)
                    {
                        System.Run.TIM2Flag = 0;
                        TIM2_Close();
                    }

                    clear(0, 0, 127, 63);

                    System.Screen.MainScreen = 3;
                    System.Screen.FuScreen   = 1;

                    LCD_PutCHString(0, 0, "增加越站", 0);

                    System.Run.KEYSelect = 1;

                    if (!System.Run.TIM2Flag)
                    {
                        System.Run.TIM2Flag = 4;
                        TIM2_Open();
                    }
                }
                else if ((System.Screen.MainScreen == 3) && (System.Screen.FuScreen == 1))
                {
                    if (System.Run.TIM2Flag)
                    {
                        System.Run.TIM2Flag = 0;
                        TIM2_Close();
                    }

                    clear(0, 0, 127, 63);

                    System.Screen.MainScreen = 3;
                    System.Screen.FuScreen   = 2;

                    LCD_PutCHString(0, 0, "清除越站", 0);

                    if (System.SystemInfo.TotalYueZhan)
                    {
                        System.Run.KEYSelect = 1;

                        if (!System.Run.TIM2Flag)
                        {
                            System.Run.TIM2Flag = 5;
                            TIM2_Open();
                        }
                    }
                    else
                    {
                        LCD_PutCHString(56, 24, "无", 0);
                    }
                }
                else
                {
                    if (System.Run.TIM2Flag)
                    {
                        System.Run.TIM2Flag = 0;
                        TIM2_Close();
                    }

                    clear(0, 0, 127, 63);

                    System.Screen.MainScreen = 3;
                    System.Screen.FuScreen   = 0;

                    LCD_PutCHString(0, 0, "越站", 0);

                    if (System.SystemInfo.TotalYueZhan)
                    {
                        System.Run.KEYSelect = 1;
                        Images(4, 2, 120, 4, CHzm[System.SystemInfo.YueZhan[System.Run.KEYSelect]], 360);
                    }
                    else
                    {
                        LCD_PutCHString(56, 24, "无", 0);
                    }
                }
            }
        }
        break;
        case 0x28:  //↑按键
        {
            if ((System.SystemInfo.DCUMatser == 1) && (System.SystemInfo.PauseStatus == 1))
            {
                Up();
            }
        }
        break;
        case 0x29:  //数字0按键
        {
            if ((System.SystemInfo.DCUMatser == 1) && (System.SystemInfo.PauseStatus == 1))
            {
                ShowNum(0);
            }
        }
        break;
        case 0x2A:  //↓按键
        {
            if ((System.SystemInfo.DCUMatser == 1) && (System.SystemInfo.PauseStatus == 1))
            {
                Down();
            }
        }
        break;
        case 0x2B:  //取消按键
        {
            if (System.SystemInfo.PauseStatus == 1)
            {
                Cancel();
            }
        }
        break;
        case 0x2C:  //确认按键
        {
            if ((System.SystemInfo.DCUMatser == 1) && (System.SystemInfo.PauseStatus == 1))
            {
                OK();
            }
        }
        break;
        default:
            break;
    }
}

void ShowNum(u8 Num)
{
    switch (System.Screen.MainScreen)
    {
        case 0:
        {
            switch (System.Screen.FuScreen)
            {
                case 0:  //当前站
                case 0xFF:
                {
                    System.Screen.FuScreen = 0xFF;

                    Station = (Station * 10 + Num) % 100;
                    if ((Station <= MAXSTA) && (Station > 0))
                    {
                        System.Run.KEYSelect = Station;
                    }
                }
                break;
                case 2:  //起点站
                {
                    Station = (Station * 10 + Num) % 100;

                    if ((Station <= MAXSTA) && (Station > 0))
                    {
                        System.Run.KEYSelect = Station;
                    }
                }
                break;
                case 4:  //终点站
                {
                    Station = (Station * 10 + Num) % 100;

                    if ((Station <= MAXSTA) && (Station > 0))
                    {
                        System.Run.KEYSelect = Station;
                    }
                }
                default:
                    break;
            }
        }
        break;
        case 1:
        {
            switch (System.Screen.FuScreen)
            {
                case 0:  //紧急广播
                {
                    System.Run.KEYSelect = (System.Run.KEYSelect * 10 + Num) % 100;
                    if ((System.Run.KEYSelect <= 10) && (System.Run.KEYSelect > 0))
                    {
                        LCD_PutCHString(48, 4, CHinf[8 + System.Run.KEYSelect], 1);
                    }
                    else
                    {
                        LCD_PutCHString(48, 4, Number[System.Run.KEYSelect], 1);
                        LCD_PutCHString(64, 4, CHinf[19], 1);
                    }
                }
                break;
                case 1:
                {
                    switch (System.Run.KEYSelect)
                    {
                        case 1:
                        {
                            Station = (Station * 10 + Num) % 100;
                            if (Station == 0)
                            {
                                LCD_PutCHString(104, 0, Number[Station], 0);
                                System.Run.TmpMasterMode = Station;
                            }
                        }
                        break;
                        case 2:
                        {
                            Station = (Station * 10 + Num) % 100;
                            if (Station <= 1)
                            {
                                LCD_PutCHString(104, 16, Number[Station], 0);
                                System.Run.TmpGBMode = Station;
                            }
                        }
                        break;
                        case 3:
                        {
                            Station = (Station * 10 + Num) % 100;
                            if (Station <= 1)
                            {
                                LCD_PutCHString(104, 32, Number[Station], 0);
                                System.Run.TmpWorkMode = Station;
                            }
                        }
                        break;
                        case 4:
                        {
                            Station = (Station * 10 + Num) % 100;
                            if ((Station >= 1) && (Station <= 14))
                            {
                                LCD_PutCHString(104, 48, Number[Station], 0);
                                System.Run.TmpMonoVolume = Station;
                            }
                        }
                        break;
                        default:
                            break;
                    }
                }
                break;
                case 2:
                {
                    if (strlen(System.Run.passwad) < 6)
                    {
                        System.Run.passwad[strlen(System.Run.passwad)] = Num + 0x30;
                        LCD_PutCHString(40 + (strlen(System.Run.passwad) - 1) * 8, 24, "*", 0);
                    }
                }
                break;
                default:
                    break;
            }
        }
        break;
        case 2:
        {
            switch (System.Screen.FuScreen)
            {
                case 0:
                    break;
                case 1:
                    System.Run.KEYSelect = (System.Run.KEYSelect * 10 + Num) % 100;
                    LCD_PutCHString(150, (System.Run.KEYCurrentSelect - 1) * 2, Number[System.Run.KEYSelect], 1);
                    break;
                default:
                    break;
            }
        }
        break;
        case 3:
        {
            switch (System.Screen.FuScreen)
            {
                case 0:
                case 0xFF:
                    break;
                case 1:
                case 0xFE:
                {
                    System.Screen.FuScreen = 0xFE;

                    System.Run.KEYSelect = (System.Run.KEYSelect * 10 + Num) % 100;
                    if ((System.Run.KEYSelect <= MAXSTA) && (System.Run.KEYSelect > 0))
                    {
                        LCD_PutCHString(48, 4, CHzm[System.Run.KEYSelect - 1], 1);
                    }
                    else
                    {
                        LCD_PutCHString(48, 4, Number[System.Run.KEYSelect], 1);
                        LCD_PutCHString(64, 4, CHinf[7], 1);
                    }
                }
                break;
                case 2:
                case 0xFD:
                {
                    System.Screen.FuScreen = 0xFD;
                }
                break;
                default:
                    break;
            }
        }
        break;
        default:
            break;
    }
}

void Up(void)
{
    switch (System.Screen.MainScreen)
    {
        case 0:
        {
            switch (System.Screen.FuScreen)
            {
                case 0:  //当前站
                case 0xFF:
                {
                    System.Screen.FuScreen = 0xFF;
                    if (System.Run.KEYSelect < MAXSTA)
                    {
                        System.Run.KEYSelect++;
                    }
                    else
                    {
                        System.Run.KEYSelect = 1;
                    }

                    if (!System.Run.TIM2Flag)
                    {
                        System.Run.TIM2Flag = 1;
                        TIM2_Open();
                    }

                    Station = System.Run.KEYSelect;
                }
                break;
                case 2:  //起点站
                {
                    if (System.Run.KEYSelect < MAXSTA)
                    {
                        System.Run.KEYSelect++;
                    }
                    else
                    {
                        System.Run.KEYSelect = 1;
                    }
                    Station = System.Run.KEYSelect;
                }
                break;
                case 4:  //终点站
                {
                    if (System.Run.KEYSelect < MAXSTA)
                    {
                        System.Run.KEYSelect++;
                    }
                    else
                    {
                        System.Run.KEYSelect = 1;
                    }
                    Station = System.Run.KEYSelect;
                }
                break;
                default:
                    break;
            }
        }
        break;
        case 1:
        {
            switch (System.Screen.FuScreen)
            {
                case 0:  //紧急广播
                {
                    if (System.Run.KEYSelect < 4)
                    {
                        System.Run.KEYSelect++;
                    }
                    else
                    {
                        System.Run.KEYSelect = 1;
                    }
                    LCD_DIS_EmerGB(System.Run.KEYSelect);
                }
                break;
                case 1:
                {
                    if (System.Run.KEYSelect < 4)
                    {
                        System.Run.KEYSelect++;
                    }
                    else
                    {
                        System.Run.KEYSelect = 1;
                    }
                    LCD_DIS_Setting(System.Run.KEYSelect);
                    Station = DefaultValue[System.Run.KEYSelect];
                }
                break;
                default:
                    break;
            }
        }
        break;
        case 2:
        {
            switch (System.Screen.FuScreen)
            {
                case 0:
                {
                    if (System.Run.KEYSelect > 1)
                    {
                        System.Run.KEYSelect--;
                    }
                    else
                    {
                        System.Run.KEYSelect = 4;
                    }
                    switch (System.Run.KEYSelect)
                    {
                        case 1:
                            LCD_PutCHString(0, 2, CHinf[21], 0);
                            LCD_PutCHString(0, 0, CHinf[20], 1);
                            break;
                        case 2:
                            LCD_PutCHString(0, 4, CHinf[22], 0);
                            LCD_PutCHString(0, 2, CHinf[21], 1);
                            break;
                        case 3:
                            LCD_PutCHString(0, 6, CHinf[23], 0);
                            LCD_PutCHString(0, 4, CHinf[22], 1);
                            break;
                        case 4:
                            LCD_PutCHString(0, 0, CHinf[20], 0);
                            LCD_PutCHString(0, 6, CHinf[23], 1);
                            break;
                    }
                }
                break;
                case 1:
                {
                    if (System.Run.KEYSelect < 10)
                    {
                        System.Run.KEYSelect++;
                    }
                    else
                    {
                        System.Run.KEYSelect = 1;
                    }
                    LCD_PutCHString(150, (System.Run.KEYCurrentSelect - 1) * 2, Number[System.Run.KEYSelect], 1);
                }
                break;
                default:
                    break;
            }
        }
        break;
        case 3:
        {
            switch (System.Screen.FuScreen)
            {
                case 0:
                case 0xFF:
                {
                    System.Screen.FuScreen = 0xFF;

                    if (System.SystemInfo.TotalYueZhan)
                    {
                        if (System.Run.KEYSelect > 1)
                        {
                            System.Run.KEYSelect--;
                        }
                        else
                        {
                            System.Run.KEYSelect = System.SystemInfo.TotalYueZhan;
                        }
                        LCD_PutCHString(48, 4, CHzm[System.SystemInfo.YueZhan[System.Run.KEYSelect - 1]], 1);
                    }
                }
                break;
                case 1:
                case 0xFE:
                {
                    System.Screen.FuScreen = 0xFE;

                    if (System.Run.KEYSelect < MAXSTA)
                    {
                        System.Run.KEYSelect++;
                    }
                    else
                    {
                        System.Run.KEYSelect = 1;
                    }
                    LCD_PutCHString(48, 4, CHzm[System.Run.KEYSelect - 1], 1);
                }
                break;
                case 2:
                case 0xFD:
                {
                    System.Screen.FuScreen = 0xFD;

                    if (System.SystemInfo.TotalYueZhan)
                    {
                        if (System.Run.KEYSelect > 1)
                        {
                            System.Run.KEYSelect--;
                        }
                        else
                        {
                            System.Run.KEYSelect = System.SystemInfo.TotalYueZhan;
                        }
                        LCD_PutCHString(48, 4, CHzm[System.SystemInfo.YueZhan[System.Run.KEYSelect - 1]], 1);
                    }
                }
                break;
                default:
                    break;
            }
        }
        break;
        default:
            break;
    }
}

void Down(void)
{
    switch (System.Screen.MainScreen)
    {
        case 0:
        {
            switch (System.Screen.FuScreen)
            {
                case 0:  //当前站
                case 0xFF:
                {
                    System.Screen.FuScreen = 0xFF;
                    if (System.Run.KEYSelect > 1)
                    {
                        System.Run.KEYSelect--;
                    }
                    else
                    {
                        System.Run.KEYSelect = MAXSTA;
                    }

                    if (!System.Run.TIM2Flag)
                    {
                        System.Run.TIM2Flag = 1;
                        TIM2_Open();
                    }
                    Station = System.Run.KEYSelect;
                }
                break;
                case 2:  //起点站
                {
                    if (System.Run.KEYSelect > 1)
                    {
                        System.Run.KEYSelect--;
                    }
                    else
                    {
                        System.Run.KEYSelect = MAXSTA;
                    }
                    Station = System.Run.KEYSelect;
                }
                break;
                case 4:  //终点站
                {
                    if (System.Run.KEYSelect > 1)
                    {
                        System.Run.KEYSelect--;
                    }
                    else
                    {
                        System.Run.KEYSelect = MAXSTA;
                    }
                    Station = System.Run.KEYSelect;
                }
                break;
                default:
                    break;
            }
        }
        break;
        case 1:
        {
            switch (System.Screen.FuScreen)
            {
                case 0:  //紧急广播
                {
                    if (System.Run.KEYSelect > 1)
                    {
                        System.Run.KEYSelect--;
                    }
                    else
                    {
                        System.Run.KEYSelect = 4;
                    }
                    LCD_DIS_EmerGB(System.Run.KEYSelect);
                }
                break;
                case 1:
                {
                    if (System.Run.KEYSelect > 1)
                    {
                        System.Run.KEYSelect--;
                    }
                    else
                    {
                        System.Run.KEYSelect = 4;
                    }
                    LCD_DIS_Setting(System.Run.KEYSelect);
                    Station = DefaultValue[System.Run.KEYSelect];
                }
                break;
                default:
                    break;
            }
        }
        break;
        case 2:
        {
            switch (System.Screen.FuScreen)
            {
                case 0:
                {
                    if (System.Run.KEYSelect < 4)
                    {
                        System.Run.KEYSelect++;
                    }
                    else
                    {
                        System.Run.KEYSelect = 1;
                    }
                    switch (System.Run.KEYSelect)
                    {
                        case 1:
                            LCD_PutCHString(0, 6, CHinf[23], 0);
                            LCD_PutCHString(0, 0, CHinf[20], 1);
                            break;
                        case 2:
                            LCD_PutCHString(0, 0, CHinf[20], 0);
                            LCD_PutCHString(0, 2, CHinf[21], 1);
                            break;
                        case 3:
                            LCD_PutCHString(0, 2, CHinf[21], 0);
                            LCD_PutCHString(0, 4, CHinf[22], 1);
                            break;
                        case 4:
                            LCD_PutCHString(0, 4, CHinf[22], 0);
                            LCD_PutCHString(0, 6, CHinf[23], 1);
                            break;
                    }
                }
                break;
                case 1:
                {
                    if (System.Run.KEYSelect > 1)
                    {
                        System.Run.KEYSelect--;
                    }
                    else
                    {
                        System.Run.KEYSelect = 10;
                    }
                    LCD_PutCHString(150, (System.Run.KEYCurrentSelect - 1) * 2, Number[System.Run.KEYSelect], 1);
                }
                break;
                default:
                    break;
            }
        }
        break;
        case 3:
        {
            switch (System.Screen.FuScreen)
            {
                case 0:
                case 0xFF:
                {
                    System.Screen.FuScreen = 0xFF;

                    if (System.SystemInfo.TotalYueZhan)
                    {
                        if (System.Run.KEYSelect < System.SystemInfo.TotalYueZhan)
                        {
                            System.Run.KEYSelect++;
                        }
                        else
                        {
                            System.Run.KEYSelect = 1;
                        }
                        LCD_PutCHString(48, 4, CHzm[System.SystemInfo.YueZhan[System.Run.KEYSelect - 1]], 1);
                    }
                }
                break;
                case 1:
                case 0xFE:
                {
                    System.Screen.FuScreen = 0xFE;

                    if (System.Run.KEYSelect > 1)
                    {
                        System.Run.KEYSelect--;
                    }
                    else
                    {
                        System.Run.KEYSelect = MAXSTA;
                    }
                    LCD_PutCHString(48, 4, CHzm[System.Run.KEYSelect - 1], 1);
                }
                break;
                case 2:
                case 0xFD:
                {
                    System.Screen.FuScreen = 0xFD;

                    if (System.SystemInfo.TotalYueZhan)
                    {
                        if (System.Run.KEYSelect < System.SystemInfo.TotalYueZhan)
                        {
                            System.Run.KEYSelect++;
                        }
                        else
                        {
                            System.Run.KEYSelect = 1;
                        }
                        LCD_PutCHString(48, 4, CHzm[System.SystemInfo.YueZhan[System.Run.KEYSelect - 1]], 1);
                    }
                }
                break;
                default:
                    break;
            }
        }
        break;
        default:
            break;
    }
}

void Cancel(void)
{
    switch (System.Screen.MainScreen)
    {
        case 0:
        {
            switch (System.Screen.FuScreen)
            {
                case 0:
                case 0xFF:  //当前站界面
                {
                    System.Screen.MainScreen = 0;
                    System.Screen.FuScreen   = 0;

                    System.Run.TmpNowStation = System.SystemInfo.NowStation;

                    if (System.Run.TIM2Flag)
                    {
                        System.Run.TIM2Flag = 0;
                        TIM2_Close();
                    }

                    Images(4, 2, 120, 4, CHzm[System.SystemInfo.NowStation], 360);
                    LCD_PutCHString(88, 48, Number[System.SystemInfo.NowStation], 0);
                }
                break;
                case 1:
                case 2:  //起点站界面
                {
                    System.Screen.MainScreen = 0;
                    System.Screen.FuScreen   = 0;

                    if (System.Run.TIM2Flag)
                    {
                        System.Run.TIM2Flag = 0;
                        TIM2_Close();
                    }

                    clear(0, 0, 127, 63);
                    LCD_DIS_MAIN();
                }
                break;
                case 3:
                case 4:  //终点站界面
                {
                    System.Screen.MainScreen = 0;
                    System.Screen.FuScreen   = 0;

                    if (System.Run.TIM2Flag)
                    {
                        System.Run.TIM2Flag = 0;
                        TIM2_Close();
                    }

                    clear(0, 0, 127, 63);
                    LCD_DIS_MAIN();
                }
                break;
                default:
                    break;
            }
        }
        break;
        case 1:
        {
            switch (System.Screen.FuScreen)
            {
                case 0:  //紧急广播
                {
                    System.Screen.MainScreen = 0;
                    System.Screen.FuScreen   = 0;

                    if (System.Run.TIM2Flag)
                    {
                        System.Run.TIM2Flag = 0;
                        TIM2_Close();
                    }

                    clear(0, 0, 127, 63);
                    LCD_DIS_MAIN();
                }
                break;
                case 1:
                {
                    System.Screen.MainScreen = 0;
                    System.Screen.FuScreen   = 0;

                    clear(0, 0, 127, 63);
                    LCD_DIS_MAIN();
                }
                break;
                default:
                    break;
            }
        }
        break;
        case 2:
        {
            switch (System.Screen.FuScreen)
            {
                case 0:
                {
                    System.Screen.MainScreen = 0;
                    System.Screen.FuScreen   = 0;

                    clear(0, 0, 192, 8);
                    LCD_DIS_MAIN();
                }
                break;
                case 1:
                {
                    System.Screen.MainScreen = 2;
                    System.Screen.FuScreen   = 0;
                    System.Run.KEYSelect     = System.Run.KEYCurrentSelect;

                    switch (System.Run.KEYCurrentSelect)
                    {
                        case 1:
                            LCD_PutCHString(0, 0, CHinf[20], 1);
                            LCD_PutCHString(150, 0, Number[System.Volume.MP3_Volume], 0);
                            break;
                        case 2:
                            LCD_PutCHString(0, 2, CHinf[21], 1);
                            LCD_PutCHString(150, 2, Number[System.Volume.Man_Volume], 0);
                            break;
                        case 3:
                            LCD_PutCHString(0, 4, CHinf[22], 1);
                            LCD_PutCHString(150, 4, Number[System.Volume.JT_Volume], 0);
                            break;
                        case 4:
                            LCD_PutCHString(0, 6, CHinf[23], 1);
                            LCD_PutCHString(150, 6, Number[System.Volume.Talk_Volume], 0);
                            break;
                        default:
                            break;
                    }
                }
                break;
                default:
                    break;
            }
        }
        break;
        case 3:
        {
            System.Screen.MainScreen = 0;
            System.Screen.FuScreen   = 0;

            if (System.Run.TIM2Flag)
            {
                System.Run.TIM2Flag = 0;
                TIM2_Close();
            }

            clear(0, 0, 127, 63);
            LCD_DIS_MAIN();
        }
        break;
        default:
            break;
    }
}

void OK(void)
{
    u8 Data[7] = {0}, i = 0;

    switch (System.Screen.MainScreen)
    {
        case 0:
        {
            switch (System.Screen.FuScreen)
            {
                case 0:
                case 0xFF:
                {
                    if (System.Run.KEYSelect != System.SystemInfo.NowStation)
                    {
                        if ((System.Run.KEYSelect <= MAXSTA) && (System.Run.KEYSelect > 0))
                        {
                            if (System.SystemInfo.BroadcastType != 1)
                            {
                                CAN_SendLineInfoCMD(System.SystemInfo.StartStation, System.SystemInfo.EndStation, System.Run.KEYSelect,
                                                    System.SystemInfo.LineNo, System.SystemInfo.ShangXiaxing);
                            }

                            System.Screen.MainScreen = 0;
                            System.Screen.FuScreen   = 0;

                            if (System.Run.TIM2Flag == 1)
                            {
                                System.Run.TIM2Flag = 0;
                                TIM2_Close();
                            }

                            System.SystemInfo.NowStation = System.Run.KEYSelect;

                            Images(4, 2, 120, 4, CHzm[System.SystemInfo.NowStation], 360);
                            LCD_PutCHString(88, 48, Number[System.SystemInfo.NowStation], 0);
                        }
                    }
                }
                break;
                case 1:
                {
                    System.Screen.MainScreen = 0;
                    System.Screen.FuScreen   = 0;

                    if (System.Run.TIM2Flag == 2)
                    {
                        System.Run.TIM2Flag = 0;
                        TIM2_Close();
                    }

                    clear(0, 0, 127, 63);
                    LCD_DIS_MAIN();
                }
                break;
                case 2:  //起点站界面
                {
                    if ((System.Run.KEYSelect <= MAXSTA) && (System.Run.KEYSelect > 0))
                    {
                        if (System.SystemInfo.BroadcastType != 1)
                        {
                            CAN_SendLineInfoCMD(System.Run.KEYSelect, System.SystemInfo.EndStation, System.SystemInfo.NowStation,
                                                System.SystemInfo.LineNo, System.SystemInfo.ShangXiaxing);
                        }
                    }

                    System.Screen.MainScreen = 0;
                    System.Screen.FuScreen   = 0;

                    if (System.Run.TIM2Flag == 2)
                    {
                        System.Run.TIM2Flag = 0;
                        TIM2_Close();
                    }

                    System.SystemInfo.StartStation = System.Run.KEYSelect;
                    clear(0, 0, 127, 63);
                    LCD_DIS_MAIN();
                }
                break;
                case 3:
                {
                    System.Screen.MainScreen = 0;
                    System.Screen.FuScreen   = 0;

                    if (System.Run.TIM2Flag == 2)
                    {
                        System.Run.TIM2Flag = 0;
                        TIM2_Close();
                    }

                    clear(0, 0, 127, 63);
                    LCD_DIS_MAIN();
                }
                break;
                case 4:  //终点站界面
                {
                    if ((System.Run.KEYSelect <= MAXSTA) && (System.Run.KEYSelect > 0))
                    {
                        if (System.SystemInfo.BroadcastType != 1)
                        {
                            CAN_SendLineInfoCMD(System.SystemInfo.StartStation, System.Run.KEYSelect, System.SystemInfo.NowStation,
                                                System.SystemInfo.LineNo, System.SystemInfo.ShangXiaxing);
                        }
                    }

                    System.Screen.MainScreen = 0;
                    System.Screen.FuScreen   = 0;

                    if (System.Run.TIM2Flag == 3)
                    {
                        System.Run.TIM2Flag = 0;
                        TIM2_Close();
                    }

                    System.SystemInfo.EndStation = System.Run.KEYSelect;
                    clear(0, 0, 127, 63);
                    LCD_DIS_MAIN();
                }
                break;
                default:
                    break;
            }
        }
        break;
        case 1:
        {
            switch (System.Screen.FuScreen)
            {
                case 0:  //紧急广播界面
                {
                    if ((System.Run.KEYSelect <= MAXSTA) && (System.Run.KEYSelect > 0))
                    {
                        CAN_SendSetVoiceCMD(0x04, 0x00, System.Run.KEYSelect);
                    }

                    System.Screen.MainScreen = 0;
                    System.Screen.FuScreen   = 0;

                    clear(0, 0, 127, 63);
                    LCD_DIS_MAIN();
                }
                break;
                case 1:
                {
                    switch (System.Run.KEYSelect)
                    {
                        case 1:  //主控模式
                        {
                        }
                        break;
                        case 2:  //报站模式
                        {
                        }
                        break;
                        case 3:  //工作模式
                        {
                        }
                        break;
                        case 4:  //监听音量
                        {
                        }
                        break;
                        default:
                            break;
                    }

                    System.Screen.MainScreen = 0;
                    System.Screen.FuScreen   = 0;

                    clear(0, 0, 127, 63);
                    LCD_DIS_MAIN();
                }
                break;
                default:
                    break;
            }
        }
        break;
        case 2:
        {
            switch (System.Screen.FuScreen)
            {
                case 0:
                {
                    System.Screen.MainScreen    = 2;
                    System.Screen.FuScreen      = 1;
                    System.Run.KEYCurrentSelect = System.Run.KEYSelect;

                    switch (System.Run.KEYSelect)
                    {
                        case 1:
                            LCD_PutCHString(0, 0, CHinf[20], 0);
                            LCD_PutCHString(150, 0, Number[System.Volume.MP3_Volume], 1);

                            System.Run.KEYSelect = System.Volume.MP3_Volume;
                            break;
                        case 2:
                            LCD_PutCHString(0, 2, CHinf[21], 0);
                            LCD_PutCHString(150, 2, Number[System.Volume.Man_Volume], 1);

                            System.Run.KEYSelect = System.Volume.Man_Volume;
                            break;
                        case 3:
                            LCD_PutCHString(0, 4, CHinf[22], 0);
                            LCD_PutCHString(150, 4, Number[System.Volume.JT_Volume], 1);

                            System.Run.KEYSelect = System.Volume.JT_Volume;
                            break;
                        case 4:
                            LCD_PutCHString(0, 6, CHinf[23], 0);
                            LCD_PutCHString(150, 6, Number[System.Volume.Talk_Volume], 1);

                            System.Run.KEYSelect = System.Volume.Talk_Volume;
                            break;
                        default:
                            break;
                    }
                }
                break;
                case 1:
                {
                    if ((System.Run.KEYSelect <= 10) && (System.Run.KEYSelect > 0))
                    {
                        switch (System.Run.KEYCurrentSelect)
                        {
                            case 1:
                                CAN_SendSetVolumeCMD(0x02, System.Volume.Man_Volume, System.Run.KEYSelect, System.Volume.Radio_Volume, 0x00,
                                                     System.Volume.JT_Volume, System.Volume.Talk_Volume);

                                System.Volume.MP3_Volume = System.Run.KEYSelect;
                                break;
                            case 2:
                                CAN_SendSetVolumeCMD(0x02, System.Run.KEYSelect, System.Volume.MP3_Volume, System.Volume.Radio_Volume, 0x00,
                                                     System.Volume.JT_Volume, System.Volume.Talk_Volume);

                                System.Volume.Man_Volume = System.Run.KEYSelect;

                                if (System.SystemInfo.BroadcastMode == 0xA1)
                                {
                                    DB3812WriteVolume(System.Volume.Man_Volume, System.Volume.JT_Volume, ChxVolume);
                                    DB3812WriteVolume(Gain, 0, ChxGain);  //写增益
                                }
                                break;
                            case 3:
                                CAN_SendSetVolumeCMD(0x02, System.Volume.Man_Volume, System.Volume.MP3_Volume, System.Volume.Radio_Volume, 0x00,
                                                     System.Run.KEYSelect, System.Volume.Talk_Volume);

                                System.Volume.JT_Volume = System.Run.KEYSelect;

                                if ((System.Run.DriverTalk == 2) || (System.Run.AlarmTalk == 2))
                                {
                                    DB3812WriteVolume(System.Volume.Talk_Volume, System.Volume.JT_Volume, ChxVolume);
                                    DB3812WriteVolume(Gain, 0, ChxGain);  //写增益
                                }
                                break;
                            case 4:
                                CAN_SendSetVolumeCMD(0x02, System.Volume.Man_Volume, System.Volume.MP3_Volume, System.Volume.Radio_Volume, 0x00,
                                                     System.Volume.JT_Volume, System.Run.KEYSelect);

                                System.Volume.Talk_Volume = System.Run.KEYSelect;

                                if ((System.Run.DriverTalk == 2) || (System.Run.AlarmTalk == 2))
                                {
                                    DB3812WriteVolume(System.Volume.Talk_Volume, System.Volume.JT_Volume, ChxVolume);
                                    DB3812WriteVolume(Gain, 0, ChxGain);  //写增益
                                }
                                break;
                            default:
                                break;
                        }
                    }
                }
                break;
                default:
                    break;
            }
        }
        break;
        case 3:
        {
            switch (System.Screen.FuScreen)
            {
                case 0:
                case 0xFF:
                {
                    System.Screen.FuScreen = 0xFF;

                    System.Screen.MainScreen = 0;
                    System.Screen.FuScreen   = 0;

                    clear(0, 0, 127, 63);
                    LCD_DIS_MAIN();
                }
                break;
                case 1:
                case 0xFE:
                {
                    System.Screen.FuScreen = 0xFE;

                    for (i = 0; i < System.SystemInfo.TotalYueZhan; i++)
                    {
                        Data[(System.SystemInfo.YueZhan[i] / 8)] |= (1 << (System.SystemInfo.YueZhan[i] % 8));
                    }

                    Data[((System.Run.KEYSelect - 1) / 8)] |= (1 << ((System.Run.KEYSelect - 1) % 8));

                    CAN_SendYueZhanCMD(Data[0], Data[1], Data[2], Data[3], Data[4], Data[5], Data[6], 0x00);

                    if (System.Run.TIM2Flag)
                    {
                        System.Run.TIM2Flag = 0;
                        TIM2_Close();
                    }

                    System.Screen.MainScreen = 0;
                    System.Screen.FuScreen   = 0;

                    clear(0, 0, 127, 63);
                    LCD_DIS_MAIN();
                }
                break;
                case 2:
                case 0xFD:
                {
                    System.Screen.FuScreen = 0xFD;

                    for (i = 0; i < System.SystemInfo.TotalYueZhan; i++)
                    {
                        Data[(System.SystemInfo.YueZhan[i] / 8)] |= (1 << (System.SystemInfo.YueZhan[i] % 8));
                    }

                    Data[(System.SystemInfo.YueZhan[System.Run.KEYSelect - 1] / 8)] &=
                        (u8) ~(1 << (System.SystemInfo.YueZhan[System.Run.KEYSelect - 1] % 8));

                    CAN_SendYueZhanCMD(Data[0], Data[1], Data[2], Data[3], Data[4], Data[5], Data[6], 0x00);

                    if (System.Run.TIM2Flag)
                    {
                        System.Run.TIM2Flag = 0;
                        TIM2_Close();
                    }

                    System.Screen.MainScreen = 0;
                    System.Screen.FuScreen   = 0;

                    clear(0, 0, 127, 63);
                    LCD_DIS_MAIN();
                }
                break;
                default:
                    break;
            }
        }
        break;
        default:
            break;
    }
}

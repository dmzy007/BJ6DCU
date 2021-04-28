#include "bsp.h"
#include "mcp23s17.h"

u32           TimingDelay;
u32           DispDelay = 0;
SystemTypeDef System;

static ErrorStatus HSEStartUpStatus;
extern void        _delay_ms(u32 us);
/*******************************************************************************
 * ������  		: Systick_Configuration
 * ��������    	: ��ʱ������,��ʱ1ms
 * �������      : ��
 * ������      : ��
 * ����ֵ        : ��
 *******************************************************************************/
void Systick_Configuration(void)
{
    if (SysTick_Config(SystemCoreClock / 1000))  // 1ms
        while (1)
            ;
}

/*******************************************************************************
 * Function Name  : delay_us
 * Description    : ��ʱ����
 * Input          : data  ��ʱʱ�䱶��
 * Output         : None
 * Return         : None
 * Attention		 : None
 *******************************************************************************/
void delay_us(long data)
{
    u16 k;

    while (data--)
    {
        for (k = 3; k > 0; k--)
            ;
    }
}

void RCC_Configuration(void)
{
    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/
    /* RCC system reset(for debug purpose) */
    RCC_DeInit();

    /* Enable HSE */
    RCC_HSEConfig(RCC_HSE_ON);

    /* Wait till HSE is ready */
    HSEStartUpStatus = RCC_WaitForHSEStartUp();

    if (HSEStartUpStatus == SUCCESS)
    {
        /* Enable Prefetch Buffer */
        ErrorStatus HSEStartUpStatus;

        RCC_DeInit();  //ʹ���ⲿ����
        RCC_HSEConfig(RCC_HSE_ON);
        RCC_HSICmd(DISABLE);                         //�ȴ��ⲿ�����ȶ�
        HSEStartUpStatus = RCC_WaitForHSEStartUp();  //����ⲿ���������ɹ����������һ������
        if (HSEStartUpStatus == SUCCESS)
        {
            RCC_HCLKConfig(RCC_SYSCLK_Div1);  //����HCLK��AHBʱ�ӣ�=SYSCLK
            RCC_PCLK1Config(RCC_HCLK_Div2);   // PCLK1(APB1) = HCLK/2
            RCC_PCLK2Config(RCC_HCLK_Div1);   // PCLK2(APB2) = HCLK

            // FLASHʱ�����
            //�Ƽ�ֵ��SYSCLK = 0~24MHz   Latency=0
            //        SYSCLK = 24~48MHz  Latency=1
            //        SYSCLK = 48~72MHz  Latency=2
            FLASH_SetLatency(FLASH_Latency_2);  //����FLASHԤȡָ����
            FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
            RCC_PLLConfig(((uint32_t)0x00010000), RCC_PLLMul_6);  // PLL���� SYSCLK/1 * 6 = 12*1*6 = 72MHz
            //       RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_9);
            RCC_PLLCmd(ENABLE);  //����PLL
            while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
                ;                                       //�ȴ�PLL�ȶ�
            RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  //ϵͳʱ��SYSCLK����PLL���
            while (RCC_GetSYSCLKSource() != 0x08)
                ;  //�л�ʱ�Ӻ�ȴ�ϵͳʱ���ȶ�
        }
        //����GPIO
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
        //����AFIO
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
        //����CAN1
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
    }
}

/*******************************************************************************
 * ������  		: GPIO_Configuration
 * ��������    	: ���ø�GPIO�˿ڹ���
 * �������      : ��
 * ������      : ��
 * ����ֵ        : ��
 *******************************************************************************/

void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    //�������е�I/O��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);
    //����AFIO
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    /* LCD IO�ӿ� ��ʼ��*/
    LCD_IO_Config();
    /* ��ʼ��������ص�IO*/
    KeyLed_IO_Config();
    /* MCP23S17 IO�ӿ� ��ʼ��*/
    MCP23S17_Configuration();
    /* ������һЩIO*/
    // MIC�̵�������	����
    // DJC PB8  CROSSC PB9
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_Pin_8 | GPIO_Pin_9);
    // PD3 ������
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOD, GPIO_Pin_3);

    // MAX3812   AMC
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    // MAX3812   WDI
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    // MAX3812   MANC
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /* 3812-DATA */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* 3812-CLK */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    /* ��ͲHOOK */
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    //�㲥���� GBJTC PD4���Խ����� DJJTC PD5  ���ž��� DJOUTC PD6 �͵�ƽ������
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOD, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6);
}

void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    //   NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0X0);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    /* Enable CAN RX0 interrupt IRQ channel */
    NVIC_InitStructure.NVIC_IRQChannel                   = CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel                   = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel                   = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel                   = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 4;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel                   = TIM5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 5;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel                   = TIM6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 6;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void IWDG_Configuration(void)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); /* д��0x5555,�����������Ĵ���д�빦�� */
    IWDG_SetPrescaler(IWDG_Prescaler_256);        /* �ڲ�����ʱ��256��Ƶ 40K/256=156HZ(6.4ms) */
    /* ���Ź���ʱ�����ʱ�� */
    IWDG_SetReload(781);  /* ι��ʱ�� 5s/6.4MS=781 .ע�ⲻ�ܴ���0xfff*/
    IWDG_ReloadCounter(); /* ι��*/
    IWDG_Enable();        /* ʹ�ܿ��Ź�*/
}

void CLR_MANGB(void)
{
    if (!MANGB_in)
    {
        do
        {
            WDI_CLR();  //��MAX813��ʹMCU����AMC��Ч;
            _delay_ms(5);
            AMC1_OUTPUT_L();
            WDI_CLR();  //��MAX813��ʹMCU����AMC��Ч;
            _delay_ms(5);
            AMC1_OUTPUT_H();
            _delay_ms(10);
        } while (!MANGB_in);
    }
}

void Bsp_Init(void)
{
    u8 Num = 0, i;
    /* ����Systickʱ�� */
    Systick_Configuration();
    /* ����GPIO�˿� */
    GPIO_Configuration();
    /* ����Timer2��ʱ��*/
    Timer2_Configuration();
    /* ����Timer3��ʱ��*/
    Timer3_Configuration();
    /* ����Timer4��ʱ��*/
    Timer4_Configuration();
    /* ����Timer5��ʱ��*/
    Timer5_Configuration();
    // /* ����Timer6��ʱ��*/
    // Timer6_Configuration();
    /* �����ж�*/
    NVIC_Configuration();
    /* ��ʼ��CAN�շ�*/
    CAN_Configuration();
    /*��ʼ��LCD��*/
    LCD_init();
    /* ��ʼ��3812*/
    initbd3812();

    System.SystemInfo.SystemStatus  = false;
    System.SystemInfo.CmdStatus     = false;
    System.SystemInfo.BroadcastType = 0;
    System.SystemInfo.PauseStatus   = 1;
    System.SystemInfo.KeyMan        = 0;
    System.SystemInfo.MasterMode    = 0;
    System.SystemInfo.GBMode        = 1;
    System.SystemInfo.WorkMode      = 0;
    System.SystemInfo.MonoVolume    = 13;

    System.Run.LineInfoCmdSta    = 1;
    System.Run.YunZhanCmdSta     = 1;
    System.Run.StartStopSta      = 1;
    System.Run.ZhuBeiCmdSta      = 1;
    System.Run.SetVolumeCmdSta   = 1;
    System.Run.ZhuBeiCmdSta      = 1;
    System.Run.QueryVoiceCmdSta  = 1;
    System.Run.CurrentHostCmdSta = 1;

    System.Run.TmpMasterMode = System.SystemInfo.MasterMode;
    System.Run.TmpGBMode     = System.SystemInfo.GBMode;
    System.Run.TmpWorkMode   = System.SystemInfo.WorkMode;
    System.Run.TmpMonoVolume = System.SystemInfo.MonoVolume;

    System.Run.FirstZhuBeiCmdSta      = 0;
    System.Run.FirstQueryVoiceCmdSta  = 0;
    System.Run.FirstCurrentHostCmdSta = 0;

    System.SystemInfo.AlarmTotal = 0;

    System.SystemInfo.BroadcastMode = 0xA3;

    System.SystemInfo.ListenerFlag = true;

    System.SystemInfo.LieCheAddress = TailID;

    System.Screen.MainScreen       = 0xFF;
    System.Screen.FuScreen         = 0xFF;
    System.SystemInfo.KeyLedStatus = ALLKEYLEDOFF;

    LCD_PutCHString(0, 24, "ϵͳ������", 0);
    TIM3_Open();

    while ((System.SystemInfo.SystemStatus & System.Run.FirstZhuBeiCmdSta & System.Run.FirstCurrentHostCmdSta & System.Run.FirstQueryVoiceCmdSta) ==
           0)
    {
        if (DispDelay == 1000)
        {
            DispDelay = 0;
            LCD_PutChar(80 + Num * 8, 24, '.');
            Num++;
            if (Num == 6)
            {
                Num = 0;
                clear(80, 24, 127, 39);
                break;
            }
            if ((Num == 1) && (System.Run.FirstQueryVoiceCmdSta == 0))
            {
                CAN_FirstSendQueryVoiceCMD(0x00);
            }
            if ((Num == 2) && (System.Run.FirstZhuBeiCmdSta == 0))
            {
                CAN_FirstSendZhuBeiCMD(0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
            }
            if ((Num == 3) && (System.Run.FirstCurrentHostCmdSta == 0))
            {
                CAN_FirstSendZhuBeiCMD(0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
            }
        }
        CAN_Reciver_data_Analyse();
    }

    System.Screen.MainScreen = 0;
    System.Screen.FuScreen   = 0;

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
    System.SystemInfo.COMMatser = 2;

    System.Run.DriverTalk = 0;
    System.Run.AlarmTalk  = 0;

    System.SystemInfo.StartStation = 1;
    System.SystemInfo.NowStation   = 1;
    System.SystemInfo.EndStation   = MAXSTA;

    System.Run.TmpStartStation = System.SystemInfo.StartStation;
    System.Run.TmpNowStation   = System.SystemInfo.NowStation;
    System.Run.TmpEndStation   = System.SystemInfo.EndStation;

    DB3812WriteVolume(System.Volume.Talk_Volume, System.Volume.JT_Volume, ChxVolume);
    DB3812WriteVolume(Gain, 0, ChxGain);  //д����

    clear(0, 0, 127, 63);
    LCD_DIS_MAIN();

    AMC1_OUTPUT_H();

    for (i = 0; i < 5; i++)
    {
        _delay_ms(5);
        WDI_CLR();  //��MAX813��ʹMCU����AMC��Ч;
    }

    WDI_CLR();  //��MAX813��ʹMCU����AMC��Ч;
    CLR_MANGB();

    // TIM5_Open();
    // IWDG_Configuration();

    // KeepInterCom();

    System.SystemInfo.DCUMatser = 1;
}

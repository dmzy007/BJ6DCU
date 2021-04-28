#include "bsp.h"
#include "mcp23s17.h"

u32           TimingDelay;
u32           DispDelay = 0;
SystemTypeDef System;

static ErrorStatus HSEStartUpStatus;
extern void        _delay_ms(u32 us);
/*******************************************************************************
 * 函数名  		: Systick_Configuration
 * 函数描述    	: 定时器功能,定时1ms
 * 输入参数      : 无
 * 输出结果      : 无
 * 返回值        : 无
 *******************************************************************************/
void Systick_Configuration(void)
{
    if (SysTick_Config(SystemCoreClock / 1000))  // 1ms
        while (1)
            ;
}

/*******************************************************************************
 * Function Name  : delay_us
 * Description    : 延时函数
 * Input          : data  延时时间倍数
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

        RCC_DeInit();  //使能外部晶振
        RCC_HSEConfig(RCC_HSE_ON);
        RCC_HSICmd(DISABLE);                         //等待外部晶振稳定
        HSEStartUpStatus = RCC_WaitForHSEStartUp();  //如果外部晶振启动成功，则进行下一步操作
        if (HSEStartUpStatus == SUCCESS)
        {
            RCC_HCLKConfig(RCC_SYSCLK_Div1);  //设置HCLK（AHB时钟）=SYSCLK
            RCC_PCLK1Config(RCC_HCLK_Div2);   // PCLK1(APB1) = HCLK/2
            RCC_PCLK2Config(RCC_HCLK_Div1);   // PCLK2(APB2) = HCLK

            // FLASH时序控制
            //推荐值：SYSCLK = 0~24MHz   Latency=0
            //        SYSCLK = 24~48MHz  Latency=1
            //        SYSCLK = 48~72MHz  Latency=2
            FLASH_SetLatency(FLASH_Latency_2);  //开启FLASH预取指功能
            FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
            RCC_PLLConfig(((uint32_t)0x00010000), RCC_PLLMul_6);  // PLL设置 SYSCLK/1 * 6 = 12*1*6 = 72MHz
            //       RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_9);
            RCC_PLLCmd(ENABLE);  //启动PLL
            while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
                ;                                       //等待PLL稳定
            RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  //系统时钟SYSCLK来自PLL输出
            while (RCC_GetSYSCLKSource() != 0x08)
                ;  //切换时钟后等待系统时钟稳定
        }
        //启动GPIO
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
        //启动AFIO
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
        //启动CAN1
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
    }
}

/*******************************************************************************
 * 函数名  		: GPIO_Configuration
 * 函数描述    	: 设置各GPIO端口功能
 * 输入参数      : 无
 * 输出结果      : 无
 * 返回值        : 无
 *******************************************************************************/

void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    //启动所有的I/O的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);
    //启动AFIO
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    /* LCD IO接口 初始化*/
    LCD_IO_Config();
    /* 初始化按键相关的IO*/
    KeyLed_IO_Config();
    /* MCP23S17 IO接口 初始化*/
    MCP23S17_Configuration();
    /* 其他的一些IO*/
    // MIC继电器开关	交叉
    // DJC PB8  CROSSC PB9
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_Pin_8 | GPIO_Pin_9);
    // PD3 蜂鸣器
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
    /* 话筒HOOK */
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    //广播监听 GBJTC PD4，对讲监听 DJJTC PD5  功放静音 DJOUTC PD6 低电平不静音
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
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); /* 写入0x5555,用于允许狗狗寄存器写入功能 */
    IWDG_SetPrescaler(IWDG_Prescaler_256);        /* 内部低速时钟256分频 40K/256=156HZ(6.4ms) */
    /* 看门狗定时器溢出时间 */
    IWDG_SetReload(781);  /* 喂狗时间 5s/6.4MS=781 .注意不能大于0xfff*/
    IWDG_ReloadCounter(); /* 喂狗*/
    IWDG_Enable();        /* 使能看门狗*/
}

void CLR_MANGB(void)
{
    if (!MANGB_in)
    {
        do
        {
            WDI_CLR();  //清MAX813；使MCU控制AMC有效;
            _delay_ms(5);
            AMC1_OUTPUT_L();
            WDI_CLR();  //清MAX813；使MCU控制AMC有效;
            _delay_ms(5);
            AMC1_OUTPUT_H();
            _delay_ms(10);
        } while (!MANGB_in);
    }
}

void Bsp_Init(void)
{
    u8 Num = 0, i;
    /* 设置Systick时钟 */
    Systick_Configuration();
    /* 设置GPIO端口 */
    GPIO_Configuration();
    /* 设置Timer2定时器*/
    Timer2_Configuration();
    /* 设置Timer3定时器*/
    Timer3_Configuration();
    /* 设置Timer4定时器*/
    Timer4_Configuration();
    /* 设置Timer5定时器*/
    Timer5_Configuration();
    // /* 设置Timer6定时器*/
    // Timer6_Configuration();
    /* 设置中断*/
    NVIC_Configuration();
    /* 初始化CAN收发*/
    CAN_Configuration();
    /*初始化LCD屏*/
    LCD_init();
    /* 初始化3812*/
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

    LCD_PutCHString(0, 24, "系统启动中", 0);
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
            System.SystemInfo.LieCheAddress = HeadID;  // 10车头
        }
        if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) == 0)
        {
            System.SystemInfo.LieCheAddress = TailID;  // 28车尾
        }
    }
    else
    {
        System.SystemInfo.LieCheAddress = TailID;  // 28车尾
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
    DB3812WriteVolume(Gain, 0, ChxGain);  //写增益

    clear(0, 0, 127, 63);
    LCD_DIS_MAIN();

    AMC1_OUTPUT_H();

    for (i = 0; i < 5; i++)
    {
        _delay_ms(5);
        WDI_CLR();  //清MAX813；使MCU控制AMC有效;
    }

    WDI_CLR();  //清MAX813；使MCU控制AMC有效;
    CLR_MANGB();

    // TIM5_Open();
    // IWDG_Configuration();

    // KeepInterCom();

    System.SystemInfo.DCUMatser = 1;
}

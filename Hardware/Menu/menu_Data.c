#include "menu.h"
#include <string.h>
#include <math.h>
#include "Delay.h"
#include "OLED.h"
#include "AD.h"
#include "voltage_mea.h"
#include "MyRTC.h"
#include "MPU6050.h"
#include "CH7800play.h"
#include "Time_page.h"
#include "Serial2.h"
#include "Serial3.h"
#include <time.h>
#include "stm32_power.h"    


#define Ev1527     0  //定义Ev1527使能
#define Life_Play  1  //定义Life_Play使能



void error(void);

//***************************** 电压表 示波器 *****************************//

void volt(void)
{
    struct option_class2 option_list[] = {
        {"- 退出",		Function,	RETURN},			//{选项名,需要运行的函数}
        {"- 电压表",	Function,	voltage_mt},
        {"- 示波器",	Function,	voltage_waveform},
        {".."}											//结尾标志,方便计算数量
    };
    run_menu2(option_list);

}

//***************************** 无线遥控 *****************************//

// #include "RF_Ev1527.h"
void YAOKON315M(void)
{
    // 为1时开启
#if Ev1527
    s8 p;
    u8 pas = 1, fa = 0;


    OLED_Clear();
    OLED_Update();
    while (1)
    {
        OLED_transition(1);
        OLED_Clear();

        Time_page_small_Timer(0, 0);//获取时间并在指定位置显示
        Time_page_BAT(89, 0);


        OLED_ShowString(4, 24, "当前通道:", OLED_6X8);
        OLED_ShowNum(60, 20, pas, 2, OLED_8X16);


        OLED_ShowString(4, 48, "格式:EV1527", OLED_6X8);
        OLED_ShowString(75, 48, "下拨发送", OLED_6X8);


        if (fa)OLED_DrawRectangle(73, 46, 52, 16, OLED_UNFILLED);
        else OLED_DrawRectangle(58, 18, 20, 20, OLED_UNFILLED);


        OLED_Update();

        p = menu_Roll_event();
        if (p == 1)
        {

            if (fa)
                EV1527_Send(pas);
            else if
                (pas < 15)pas++;

        }
        else if (p == -1)
        {
            if (!fa) { if (pas > 1)pas--; }
        }


        p = menu_Enter_event();
        if (p == 1)
        {
            fa = !fa;
        }
        else if (p == 2)
        {

            OLED_transition(0);
            return;
        }

    }

#else

    error();

#endif
}

//***************************************************************//

void DID(void)
{
    s8 p;

    CH7800_Init();

    OLED_Clear();
    OLED_Update();
    while (1)
    {
        OLED_transition(1);
        p = menu_Roll_event();
        if (p == 1)
        {

            CH7800_Unicast(Loudness, 30);
            delay_ms(100);
            CH7800_Unicast(Play_audio, 1);
            delay_ms(500);
        }
        else if (p == -1)
        {


        }
        p = menu_Enter_event();
        if (p == 1)
        {


            OLED_transition(0);

            //进入某某函数//



        }
        else if (p == 2)
        {


            OLED_transition(0);
            return;

        }
    }
}


//***************************** Life Play *****************************//

#if Life_Play
extern u8 OLED_DisplayBuf[8][128];      //拿来吧你

u8 Nodes_Cache[8][128];                 //计算缓存

void Update_Display(void)               //上传屏幕
{
    memcpy(OLED_DisplayBuf, Nodes_Cache, 1024);
}

void Point_life(u8 X, u8 Y)             //写点生
{
    Nodes_Cache[(Y / 8)][X] |= (0x01 << (Y % 8));
}

void Point_death(u8 X, u8 Y)            //写点死
{
    Nodes_Cache[(Y / 8)][X] &= ~(0x01 << (Y % 8));
}

u8 CountPointRound(u8 X, u8 Y)          //统计点周围细胞数量
{
    return (
        OLED_GetPoint(X - 1, Y - 1) + OLED_GetPoint(X, Y - 1) + OLED_GetPoint(X + 1, Y - 1) +
        OLED_GetPoint(X - 1, Y) + OLED_GetPoint(X + 1, Y) +
        OLED_GetPoint(X - 1, Y + 1) + OLED_GetPoint(X, Y + 1) + OLED_GetPoint(X + 1, Y + 1)
        );
}

void OLED_Rotation_C_P(s8 CX, s8 CY, float* PX, float* PY, s16 Angle)//旋转点
{
    float Theta = (3.14 / 180) * Angle;
    float Xd = *PX - CX;
    float Yd = *PY - CY;

    *PX = (Xd)*cos(Theta) - (Yd)*sin(Theta) + CX;// + 0.5;
    *PY = (Xd)*sin(Theta) + (Yd)*cos(Theta) + CY;// + 0.5;
}

void Game_Of_Life_Turn(void)            //刷新游戏回合
{
    u8 x, y;
    u8 Count;

    for (y = 0; y < 64; y++)
    {
        for (x = 0; x < 128; x++)
        {
            Count = CountPointRound(x, y);
            if (OLED_GetPoint(x, y))
            {
                if (Count < 2 || Count > 3)
                {
                    Point_death(x, y);
                }
            }
            else
            {
                if (Count == 3)
                {
                    Point_life(x, y);
                }
            }
        }
    }
    Update_Display();
}

void Game_Of_Life_Seed(s16 seed)		//游戏初始化种子
{
    srand(seed);
    for (u8 i = 0; i < 8; i++)
        for (u8 j = 0; j < 128; j++)
        {
            Nodes_Cache[i][j] = rand();
        }
    Update_Display();
}

void Game_Of_Life_Play(void)            //游戏开始
{


    u8 x1 = 8, x2 = 16, y1 = 32, y2 = 32;
    s8 shift = -1;


    OLED_Clear();
    u8 i, j;
    for (j = 0; j < 8; j++)				//遍历8页
    {
        for (i = 0; i < 128; i++)			//遍历128列
        {
            Nodes_Cache[j][i] = 0x00;	//将显存数组数据全部清零
        }
    }


    //	Game_Of_Life_Seed(1);		//放置种子

    s8 p;
    while (1)
    {
        Game_Of_Life_Turn();

        if (shift > 0) { y2 += menu_Roll_event() * 8; }
        else { x2 += menu_Roll_event() * 8; }
        x2 %= 128;
        y2 %= 64;
        OLED_DrawLine(x1, y1, x2, y2);
        if ((x2 - x1) > 1) { x1 += (x2 - x1) / 8 + 1; }
        else if ((x2 - x1) < -1) { x1 += (x2 - x1) / 8 - 1; }
        else { x1 = x2; }
        if ((y2 - y1) > 1) { y1 += (y2 - y1) / 2 + 1; }
        else if ((y2 - y1) < -1) { y1 += (y2 - y1) / 2 - 1; }
        else { y1 = y2; }

        //		OLED_Rotation_C_P(64, 32, &px, &py, Angle);
        //		OLED_DrawLine(64, 32, px+0.5, py+0.5);

        OLED_Update();
        OLED_transition(1);

        //Delay_ms(100);

        p = menu_Enter_event();
        if (p == 1) { shift = -shift; }
        else if (p == 2)
        {
            OLED_transition(0);
            return;
        }
    }
}

#else

void Game_Of_Life_Play(void)
{
    error();
}

#endif

//***************************** 收款、支付码 *****************************//

void Show_QR_Code_L(void)
{
    for (u8 i = 0;i < 20;i++)
    {
        OLED_UpdateArea(i, 0, 1, 64);
        delay_ms(10);
    }
    for (u8 i = 20;i < 30;i++)
    {
        OLED_UpdateArea(i, 0, 1, 64);
        delay_ms(7);
    }
    for (u8 i = 30;i < 128;i++)
    {
        OLED_UpdateArea(i, 0, 1, 64);
        delay_ms(1);
    }
}

void Show_QR_Code_L1(void)
{
    for (u8 i = 0;i < 100;i++)
    {
        OLED_UpdateArea(i, 0, 1, 64);
        delay_ms(1);
    }
    for (u8 i = 100;i < 120;i++)
    {
        OLED_UpdateArea(i, 0, 1, 64);
        delay_ms(7);
    }
    for (u8 i = 120;i < 128;i++)
    {
        OLED_UpdateArea(i, 0, 1, 64);
        delay_ms(10);
    }

}

void Show_QR_Code_R(void)
{
    for (u8 i = 128;i > 108;i--)
    {
        OLED_UpdateArea(i, 0, 1, 64);
        delay_ms(10);
    }
    for (u8 i = 108;i > 98;i--)
    {
        OLED_UpdateArea(i, 0, 1, 64);
        delay_ms(7);
    }
    for (u8 i = 98;i > 0;i--)
    {
        OLED_UpdateArea(i, 0, 1, 64);
        delay_ms(1);
    }

}

void Show_QR_Code_R1(void)
{
    for (u8 i = 128;i > 28;i--)
    {
        OLED_UpdateArea(i, 0, 1, 64);
        delay_ms(1);
    }
    for (u8 i = 28;i > 8;i--)
    {
        OLED_UpdateArea(i, 0, 1, 64);
        delay_ms(7);
    }
    for (u8 i = 8;i > 0;i--)
    {
        OLED_UpdateArea(i, 0, 1, 64);
        delay_ms(10);
    }
}

void Show_QR_Code(u8 w)
{
    OLED_WriteCommand(0x40);	//设置显示开始行,0x40~0x7F
    OLED_WriteCommand(0xA1);	//设置左右方向,0xA1正常,0xA0左右反置
    OLED_WriteCommand(0xC8);	//设置上下方向,0xC8正常,0xC0上下反置

    OLED_Clear();
    OLED_Reverse();

    switch (w)
    {
    case 0: // 微信收款
    {
        Show_QR_Code_L();
        OLED_Clear();
        OLED_ShowImage(41, 0, 45, 45, SKM_WX);
        OLED_ShowString(40, 51, "微信收款", OLED_6X8);
        Show_QR_Code_L1();
        break;
    }
    case 1: // 微信支付
    {
        Show_QR_Code_R();
        OLED_Clear();
        OLED_ShowImage(41, 0, 45, 45, ZFM_WX);
        OLED_ShowString(40, 51, "微信支付", OLED_6X8);
        Show_QR_Code_R1();
        break;
    }
    case 2: // 支付宝收款
    {
        Show_QR_Code_R();
        OLED_Clear();
        OLED_ShowImage(41, 0, 45, 45, SKM_ZFB);
        OLED_ShowString(34, 51, "支付宝收款", OLED_6X8);
        Show_QR_Code_R1();
        break;
    }
    case 3: // 支付宝支付
    {
        Show_QR_Code_L();
        OLED_Clear();
        OLED_ShowImage(41, 0, 45, 45, ZFM_ZFB);
        OLED_ShowString(34, 51, "支付宝支付", OLED_6X8);
        Show_QR_Code_L1();
        break;
    }
    default:
        break;
    }

}

void WeChat(void)
{
    s8 p;
    u8 w = 0;

    delay_ms(5);

    Show_QR_Code(w);

    while (1)
    {
        OLED_transition(1);
        OLED_Update();
        p = menu_Roll_event();
        if (p == 1)
        {
            w = !w;
            OLED_transition(0);
            Show_QR_Code(w);
        }
        else if (p == -1)
        {
            w = !w;
            OLED_transition(0);
            Show_QR_Code(w);
        }
        p = menu_Enter_event();
        if (p == 1)
        {
            OLED_transition(0);
            return;

        }
        else if (p == 2)
        {
            OLED_transition(0);
            return;

        }
    }

}

void Alipay(void)
{
    s8 p;
    u8 w = 3;

    delay_ms(5);

    Show_QR_Code(w);

    while (1)
    {
        OLED_transition(1);
        OLED_Update();

        p = menu_Roll_event();
        if (p == 1)
        {
            w = !w;
            OLED_transition(0);
            Show_QR_Code(w + 2);
        }
        else if (p == -1)
        {
            w = !w;
            OLED_transition(0);

            Show_QR_Code(w + 2);
        }
        p = menu_Enter_event();
        if (p == 1)
        {


            OLED_transition(0);
            return;

        }
        else if (p == 2)
        {


            OLED_transition(0);
            return;

        }
    }

}


//***************************** 光标样式 *****************************//

extern u8 Cursor;

void System_Cursor_BKP(u8 ttt)
{
    u16 temp;PWR_BackupAccessCmd(ENABLE);
    temp = (BKP_ReadBackupRegister(BKP_DR3) & 0x0fff) | (Cursor << 12);
    BKP_WriteBackupRegister(BKP_DR3, temp); //光标样式写入备份寄存器 BKP_DR3 15-12位

}

void System_Cursor1(void)
{
    Cursor = 0;
    System_Cursor_BKP(Cursor);
}

void System_Cursor2(void)
{
    Cursor = 1;
    System_Cursor_BKP(Cursor);
}

void System_Cursor3(void)
{
    Cursor = 2;
    System_Cursor_BKP(Cursor);
}

void System_Cursor(void)
{

    struct option_class2 option_list[] =
    {
        {"- 返回",  Function,    RETURN},
        {"- 反色",  Function,    System_Cursor1},
        {"- 矩形",  Function,    System_Cursor2},
        {"- 指针",  Function,    System_Cursor3},
        {".."}
    };
    run_menu2(option_list);
}


//***************************** 抬腕唤醒 *****************************//


u8 RAWA = 1;		//抬腕唤醒启用标志位
u8 RAWAtri = 60;	//抬腕触发值
u16 RAWAtrigger;	//实际抬腕触发值

// 抬腕亮屏
void Raise_Wake(void)
{
    if (WorkingMode != SuperSaving)    //如果低功耗启用
    {
        MPU_Init();
        RAWAtrigger = RAWAtri * 70;

        // u16 temp;PWR_BackupAccessCmd(ENABLE);
        // temp = (BKP_ReadBackupRegister(BKP_DR4) & 0x00ff) | (8 << RAWAtri);
        // BKP_WriteBackupRegister(BKP_DR4, temp); //抬腕触发值写入备份寄存器 BKP_DR4 高8位

        u16 temp;PWR_BackupAccessCmd(ENABLE);
        temp = (BKP_ReadBackupRegister(BKP_DR4) & 0xfff0) | RAWA;
        BKP_WriteBackupRegister(BKP_DR4, temp); //抬腕唤醒启用标志位写入备份寄存器BKP_DR4 3-0位

    }
    else
    {
        OLED_DrawRectangle(0, 31, 96, 14, OLED_UNFILLED);
        OLED_ShowString(1, 32, "请关闭超级省电", OLED_6X8);
        OLED_Update();
        RAWA = 0;
        delay_ms(500);
        return;
    }
}

// 抬腕亮屏触发阈值
void Raise_Wake_trigger(void)
{
    RAWAtrigger = RAWAtri * 70;

    u16 temp;PWR_BackupAccessCmd(ENABLE);
    temp = (BKP_ReadBackupRegister(BKP_DR4) & 0xff) | (RAWAtri << 8);

    // OLED_Clear();
    // OLED_ShowASCII(0, 0, "RAWAtri", OLED_8X16);
    // OLED_ShowNum(0, 16, RAWAtri, 8, OLED_8X16);
    // OLED_Update();
    // delay_ms(3000);


    BKP_WriteBackupRegister(BKP_DR4, temp); //抬腕触发值写入备份寄存器 BKP_DR4 高8位



}



//***************************** 系统电压显示 *****************************//

extern float RTI, VIN;
void System_Voltage(void)
{


    s8 p;
    u16 ADValue;
    float voltage;
    while (1)
    {
        OLED_WriteCommand(0x40);	//设置显示开始行,0x40~0x7F
        OLED_WriteCommand(0xA1);	//设置左右方向,0xA1正常,0xA0左右反置
        OLED_WriteCommand(0xC8);	//设置上下方向,0xC8正常,0xC0上下反置

        OLED_Clear();

        OLED_ShowFloatNum(0, 2, VIN, 2, 3, OLED_6X8);

        ADValue = AD_GetValue(ADC_Channel_8);

        OLED_ShowNum(5, 29, ADValue, 4, OLED_6X8);

        voltage = ((float)ADValue * VIN) / 4095 * 2;

        OLED_ShowASCII(0, 20, "battery", OLED_6X8);
        OLED_ShowFloatNum(55, 20, voltage, 2, 2, OLED_6X8);


        ADValue = AD_GetValue(ADC_Channel_9);

        OLED_ShowNum(5, 49, ADValue, 4, OLED_6X8);

        voltage = ((float)ADValue * VIN) / 4095;
        voltage = voltage * RTI;

        OLED_ShowASCII(0, 40, "external", OLED_6X8);
        OLED_ShowFloatNum(55, 40, voltage, 2, 2, OLED_6X8);

        OLED_Update();
        OLED_transition(1);
        p = menu_Enter_event();
        if (p) {

            OLED_transition(0);
            return;
        }
    }
}


//***************************** 信息 *****************************//

void KAIJIDONGHUA(void)
{
    OLED_Clear();
    OLED_Update();
    float gx = 40, gy = 24;
    float gt = 62;

    delay_ms(500);
    for (u16 i = 0;i < 220;i++)
    {
        OLED_Clear();
        gy /= 1.1;
        if (gy < 0.5)
        {
            gx /= 1.1;gt = 22 + gx;
            OLED_ShowASCII(30 + 42 - (gt * 2), 19, "ZI-O Watch", OLED_8X16);
            OLED_ShowASCII(28 + 22 - gt, 36, "V.24.11.1", OLED_6X8);
            OLED_ClearArea(0, 20, gt, 24);
        }
        OLED_DrawLine(gt, 20, gt, 44 - gy);
        OLED_DrawLine(gt - 1, 20, gt - 1, 44 - gy);
        OLED_Update();
        OLED_transition(1);
    }

    OLED_transition(0);
    OLED_Clear();
    OLED_Update();
}

void System_Information(void)
{
    struct option_class2 option_list[] = {
        {"- 返回",Function,RETURN},
        {"---STM32F103C8T6---",Display},
        {"- RAM: 20K",Display},
        {"- FLASH: 64K",Display},
        {"-------------------",Display},
        {"- 软件版本V1.0",Display},
        {"-------------------",Display},
        {"- 开机动画",Function,KAIJIDONGHUA},
        {".."}
    };
    run_menu2(option_list);
}


//***************************** USB响应 *****************************//

void USB_CH7800(void)
{
    CH7800_Unicast(Loudness, CHvolume);    //发个调整音量叫醒一下语音芯片,要不然它不知道在它睡觉的时候有东西插进来了
}


//***************************** 错误提示 *****************************//
void error(void)
{
    s8 p;


    OLED_Clear();
    OLED_ShowString(39, 16, "待开发中", OLED_6X8);
    OLED_Update();
    while (1)
    {
        OLED_WriteCommand(0x40);	//设置显示开始行,0x40~0x7F
        OLED_WriteCommand(0xA1);	//设置左右方向,0xA1正常,0xA0左右反置
        OLED_WriteCommand(0xC8);	//设置上下方向,0xC8正常,0xC0上下反置

        OLED_transition(1);
        p = menu_Roll_event();
        if (p)
        {
            OLED_transition(0);
            return;
        }
        p = menu_Enter_event();
        if (p)
        {

            OLED_transition(0);
            return;
        }
    }
}


//****************************** 手电筒 *****************************//
s8 mubz = 50;   //手电筒默认亮度
void flashlight(void)
{
    s8 p;

    OLED_Clear();
    OLED_Update();

    TIM_TimeBaseInitTypeDef  TIM_TimeBaseSrtructure;        //TIM时间基数初始化结构体
    TIM_OCInitTypeDef	TIM_OCInitStructure;                //TIM输出比较功能结构体

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   //使能GPIOA外设时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);    //重映射需开启AFIO时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);    //使能TIM3时钟 

    /* 端口(GPIO)设置 */
    GPIO_InitTypeDef  GPIO_InitStructure;                   //GPIO初始化结构体
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;               //PA7=T3 Ch2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       //速度 50MHz
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;         //复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);                  //初始化 GPIOA	

    /* TIM时间基数初始化 */
    TIM_TimeBaseSrtructure.TIM_Period = 100 - 1;           				//计数器 TIMx_CNT计数 从0累加到ARR次后溢出,设置自动重装载值
    TIM_TimeBaseSrtructure.TIM_Prescaler = 72 - 1;         				//设置预分频系数 PSC
    TIM_TimeBaseSrtructure.TIM_ClockDivision = 0;                       //采用时钟分割,见库函数.TIM_Clock_Division_CKD定义,查手册 00:Tdts=Tck_int:01:Tdts=2*Tck_int(2分频)
    TIM_TimeBaseSrtructure.TIM_CounterMode = TIM_CounterMode_Up;        //TIM 向上计数模式溢出
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseSrtructure);				    //初始化TIM3的时钟

    /* TIM外设初始化 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;     			    //选择定时器模式:TIM 脉冲宽度调制模式1,模式1为比较低输出.
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;           //输出极性:TIM输出比较极性高
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;       //比较输出使能
    TIM_OCInitStructure.TIM_Pulse = mubz;                               //设置占空比,即CCR

    TIM_OC2Init(TIM3, &TIM_OCInitStructure);

    /* 初始化CH1 */
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);		//CH2 预装载使能
    TIM_ARRPreloadConfig(TIM3, ENABLE);              		//使能 TIMx在ARR上的预装载寄存器

    TIM_Cmd(TIM3, ENABLE);									//使能 TIM3	

    //	TIM_PrescalerConfig
    //	TIM_SetAutoreload

    float GP = 0, GP_target;                 //条条长度,目标条条长度

    while (1)
    {
        OLED_WriteCommand(0x40);	//设置显示开始行,0x40~0x7F
        OLED_WriteCommand(0xA1);	//设置左右方向,0xA1正常,0xA0左右反置
        OLED_WriteCommand(0xC8);	//设置上下方向,0xC8正常,0xC0上下反置

        OLED_Clear();

        OLED_transition(1);

        OLED_ShowNum(19, 30, mubz, 3, OLED_8X16);
        OLED_ShowASCII(45, 30, "%", OLED_8X16);

        GP_target = mubz * 0.9;
        if (GP < GP_target)
        {
            if (GP_target - GP > 25) GP += 5;
            else if (GP_target - GP > 7) GP += 2.4;
            else if (GP_target - GP > 0) GP += 0.5;
        }
        if (GP > GP_target)
        {
            if (GP - GP_target > 25) GP -= 5;
            else if (GP - GP_target > 7) GP -= 2.4;
            else if (GP - GP_target > 0) GP -= 0.5;
        }
        OLED_DrawRectangle(17, 50, 94, 10, OLED_UNFILLED);
        OLED_DrawRectangle(19, 52, GP, 6, OLED_FILLED);
        OLED_Update();

        p = menu_Roll_event();
        if (p == 1)
        {

            mubz++;
            if (mubz > 100)mubz = 100;
            TIM_SetCompare2(TIM3, (u8)mubz);//写CRR

        }
        else if (p == -1)
        {
            mubz--;
            if (mubz < 0)mubz = 0;
            TIM_SetCompare2(TIM3, (u8)mubz);//写CRR
        }
        p = menu_Enter_event();
        if (p)
        {
            TIM_SetCompare2(TIM3, 0);//写CRR


            OLED_transition(0);

            TIM_Cmd(TIM3, DISABLE);//TIM3失能

            /*备份寄存器访问使能*/
            u16 temp;PWR_BackupAccessCmd(ENABLE);
            temp = BKP_ReadBackupRegister(BKP_DR2) | mubz << 8;
            BKP_WriteBackupRegister(BKP_DR2, temp); //手电筒亮度写入高八位备份寄存器

            return;
        }
    }
}


//***************************** 屏幕亮度 *****************************//

void System_Brightness(void)
{
    u16 temp;PWR_BackupAccessCmd(ENABLE);
    temp = (BKP_ReadBackupRegister(BKP_DR2) & 0xff00) | liangdu;
    BKP_WriteBackupRegister(BKP_DR2, temp); //屏幕亮度写入备份寄存器 BKP_DR2 低八位,高八位为手电筒亮度值
}


//****************************** 秒表 *****************************//

void Stop_Watch(void)
{
    u8  x = 5;
    u8  y = 18;
    u8 ms = 0;  // 低位
    u8 ms1 = 0; // 高位
    u8 s = 0;
    u8 s1 = 0;
    u8 min = 0;
    u8 min1 = 0;
    u8 p = 0;
    u8 flag = 0;


    OLED_Clear();
    OLED_Update();

    while (1)
    {
        OLED_WriteCommand(0x40);	//设置显示开始行,0x40~0x7F
        OLED_WriteCommand(0xA1);	//设置左右方向,0xA1正常,0xA0左右反置
        OLED_WriteCommand(0xC8);	//设置上下方向,0xC8正常,0xC0上下反置

        OLED_transition(1);

        if (flag == 1)
        {
            delay_ms(1);
            ms++;
            if (ms >= 7)
            {
                ms = 0;
                ms1++;
                if (ms1 >= 10)
                {
                    ms1 = 0;
                    s++;
                    if (s >= 10)
                    {
                        s = 0;
                        s1++;
                        if (s1 >= 6)// 60秒 = 1分钟
                        {
                            s1 = 0;
                            min++;
                            if (min >= 10)
                            {
                                min = 0;
                                min1++;
                                if (min1 >= 6) // 60分钟 = 1小时
                                {
                                    min1 = 0;

                                }
                            }
                        }
                    }
                }
            }
        }

        Time_page_figures(x + 19 + 44 + 44, y, ms, 1);
        Time_page_figures(x + 44 + 44, y, ms1, 1);

        OLED_DrawRectangle(x + 37, y + 6, 2, 4, OLED_UNFILLED);
        OLED_DrawRectangle(x + 37, y + 19, 2, 4, OLED_UNFILLED);

        Time_page_figures(x + 19 + 44, y, s, 1);
        Time_page_figures(x + 44, y, s1, 1);

        OLED_DrawRectangle(x + 63 + 18, y + 6, 2, 4, OLED_UNFILLED);
        OLED_DrawRectangle(x + 63 + 18, y + 19, 2, 4, OLED_UNFILLED);

        Time_page_figures(x + 19, y, min, 1);
        Time_page_figures(x, y, min1, 1);

        OLED_Update();

        p = menu_Enter_event(); //获取按键(确定)返回1确定,2长按
        if (p == 1)
        {
            flag = !flag;
        }
        else if (p == 2)
        {
            OLED_transition(0);
            return;
        }
    }


}


//****************************** 时间设置 *****************************//

extern u8 Time_12_flag;
extern u8 Time_24_flag;

void System_Hour_BKP(u8 x)
{
    u16 temp;PWR_BackupAccessCmd(ENABLE);
    temp = (BKP_ReadBackupRegister(BKP_DR4) & 0xff0f) | (x << 4);
    BKP_WriteBackupRegister(BKP_DR4, temp); //时间格式标志位写入备份寄存器 BKP_DR4 7-4位
}

void Time_12_Hour(void)
{
    Time_24_flag = 0;
    System_Hour_BKP(0);

}

void Time_24_Hour(void)
{
    Time_12_flag = 0;
    System_Hour_BKP(1);

}

// 时间模式
void System_Time_format(void)
{
    struct option_class2 option_list[] =
    {
        {"- 返回",          Function,    RETURN},
        {"- 12小时制",      ON_OFF,      Time_12_Hour,    &Time_12_flag},
        {"- 24小时制",      ON_OFF,      Time_24_Hour,    &Time_24_flag},
        {".."}
    };
    run_menu2(option_list);

}

void System_Time(void)
{

    struct option_class2 option_list[] =
    {
        {"- 返回",          Function,   RETURN},
        {"- 时间调整",      Function,   Time_Adjust},
        {"- 时间格式",      Function,   System_Time_format},
        {".."}
    };
    run_menu2(option_list);
}


//***************************** 计步 *****************************//
extern u8 Step_Flag;
void System_Step_Counter(void)
{
    u16 temp;PWR_BackupAccessCmd(ENABLE);
    temp = (BKP_ReadBackupRegister(BKP_DR3) & 0xff0f) | (Step_Flag << 4);
    BKP_WriteBackupRegister(BKP_DR3, temp); //计步标志位写入备份寄存器 BKP_DR3 7-4位

    if (Step_Flag == 0)
    {
        TIM_Cmd(TIM4, DISABLE);			//使能TIM4失能
    }
    else
    {
        TIM_Cmd(TIM4, ENABLE);			//使能TIM4,定时器开始运行
    }

}


//***************************** 模式切换 *****************************//

extern Mode WorkingMode;
extern u8 Normal_Flag;
extern u8 PowerSaving_Flag;
extern u8 SuperSaving_Flag;

void System_Mode_BKP(u8 f)
{

    u16 temp;PWR_BackupAccessCmd(ENABLE);
    temp = (BKP_ReadBackupRegister(BKP_DR3) & 0xf0ff) | ((f << 8) | 0xf0ff);
    BKP_WriteBackupRegister(BKP_DR3, temp); //工作模式写入备份寄存器 BKP_DR3 11-8位工作模式标志位
}

// 标准模式
void System_Normal_Mode(void)
{
    WorkingMode = Normal;

    Normal_Flag = 1;
    PowerSaving_Flag = 0;
    SuperSaving_Flag = 0;
    System_Mode_BKP(Normal_Flag);

}

// 省电模式
void System_Saving_Mode(void)
{
    WorkingMode = PowerSaving;
    Normal_Flag = 0;
    PowerSaving_Flag = 2;
    SuperSaving_Flag = 0;
    System_Mode_BKP(PowerSaving_Flag);

    if (RAWA)
    {
        RAWA = 0;
        OLED_DrawRectangle(0, 31, 84, 14, OLED_UNFILLED);
        OLED_ShowString(1, 32, "已关闭抬腕亮屏", OLED_6X8);
        OLED_Update();
        delay_ms(500);
    }

}

// 超级省电
void System_Super_Mode(void)
{
    WorkingMode = SuperSaving;

    Normal_Flag = 0;
    PowerSaving_Flag = 0;
    SuperSaving_Flag = 4;
    System_Mode_BKP(SuperSaving_Flag);
    if (RAWA)
    {
        RAWA = 0;
        OLED_DrawRectangle(0, 31, 84, 14, OLED_UNFILLED);
        OLED_ShowString(1, 32, "已关闭抬腕亮屏", OLED_6X8);
        OLED_Update();
        delay_ms(500);
    }

    if (Step_Flag)
    {
        Step_Flag = 0;
        OLED_DrawRectangle(0, 31, 84, 14, OLED_UNFILLED);
        OLED_ShowString(1, 32, "已关闭计步", OLED_6X8);
        OLED_Update();
        delay_ms(500);
    }



}

void System_Mode_Switch(void)
{
    struct option_class2 option_list[] = {
        {"- 返回",			    Function,	RETURN},
        {"- 标准模式",			ON_OFF,		System_Normal_Mode,			&Normal_Flag},
        {"- 省电模式",			ON_OFF,		System_Saving_Mode,			&PowerSaving_Flag},
        {"- 超级省电",			ON_OFF,		System_Super_Mode,			&SuperSaving_Flag},
        {".."}								//结尾标志,方便计算数量
    };
    run_menu2(option_list);
}



//功能模板

//void xxxxxxx(void)
//{
//	s8 p;
//	
//	while(1)
//	{
//		
//       OLED_transition(1);
//	   p = menu_Roll_event(); //获取按键(上下)返回1为下,-1为上  
//	   if( p == 1){ 
//	   }	 
//       else if( p == -1){ 
//	   }	
//		p=menu_Enter_event(); //获取按键(确定)返回1确定,2长按
//		if(p==1){
//			
//			OLED_transition(0);
//			
//			//进入某某函数//

//			
//		}
//		else if ( p==2 ){
//		
//			OLED_transition(0);
//			return;
//			
//		}	
//	}
//}






//****************************** 设置 ******************************//

void System_Settings(void)
{
    struct option_class2 option_list[] = {
        {"- 返回",			Function,	RETURN},
        {"- 模式切换",      Function , 	 System_Mode_Switch},
        {"- 抬腕亮屏",		ON_OFF ,	 Raise_Wake,			&RAWA},
        {"- 抬腕检测速度",	 Number ,	 Raise_Wake_trigger,	&RAWAtri},
        {"- 计步",          ON_OFF,     System_Step_Counter,    &Step_Flag},
        {"- 扬声器音量",    Number,      Display,			    &CHvolume},
        {"- 屏幕亮度",      Number,      System_Brightness,     &liangdu},
        {"- 时间设置",      Function,    System_Time},
        {"- 光标样式",		Function, 	 System_Cursor},
        {"- 系统电压",		Function,	 System_Voltage},
        {"- USB 响应",      Function,	 USB_CH7800},
        {"- 信息",			Function,	 System_Information},
        {".."}								//结尾标志,方便计算数量
    };
    run_menu2(option_list);

    System_Brightness();
}


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


#define Ev1527     0  //����Ev1527ʹ��
#define Life_Play  1  //����Life_Playʹ��



void error(void);

//***************************** ��ѹ�� ʾ���� *****************************//

void volt(void)
{
    struct option_class2 option_list[] = {
        {"- �˳�",		Function,	RETURN},			//{ѡ����,��Ҫ���еĺ���}
        {"- ��ѹ��",	Function,	voltage_mt},
        {"- ʾ����",	Function,	voltage_waveform},
        {".."}											//��β��־,�����������
    };
    run_menu2(option_list);

}

//***************************** ����ң�� *****************************//

// #include "RF_Ev1527.h"
void YAOKON315M(void)
{
    // Ϊ1ʱ����
#if Ev1527
    s8 p;
    u8 pas = 1, fa = 0;


    OLED_Clear();
    OLED_Update();
    while (1)
    {
        OLED_transition(1);
        OLED_Clear();

        Time_page_small_Timer(0, 0);//��ȡʱ�䲢��ָ��λ����ʾ
        Time_page_BAT(89, 0);


        OLED_ShowString(4, 24, "��ǰͨ��:", OLED_6X8);
        OLED_ShowNum(60, 20, pas, 2, OLED_8X16);


        OLED_ShowString(4, 48, "��ʽ:EV1527", OLED_6X8);
        OLED_ShowString(75, 48, "�²�����", OLED_6X8);


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

            //����ĳĳ����//



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
extern u8 OLED_DisplayBuf[8][128];      //��������

u8 Nodes_Cache[8][128];                 //���㻺��

void Update_Display(void)               //�ϴ���Ļ
{
    memcpy(OLED_DisplayBuf, Nodes_Cache, 1024);
}

void Point_life(u8 X, u8 Y)             //д����
{
    Nodes_Cache[(Y / 8)][X] |= (0x01 << (Y % 8));
}

void Point_death(u8 X, u8 Y)            //д����
{
    Nodes_Cache[(Y / 8)][X] &= ~(0x01 << (Y % 8));
}

u8 CountPointRound(u8 X, u8 Y)          //ͳ�Ƶ���Χϸ������
{
    return (
        OLED_GetPoint(X - 1, Y - 1) + OLED_GetPoint(X, Y - 1) + OLED_GetPoint(X + 1, Y - 1) +
        OLED_GetPoint(X - 1, Y) + OLED_GetPoint(X + 1, Y) +
        OLED_GetPoint(X - 1, Y + 1) + OLED_GetPoint(X, Y + 1) + OLED_GetPoint(X + 1, Y + 1)
        );
}

void OLED_Rotation_C_P(s8 CX, s8 CY, float* PX, float* PY, s16 Angle)//��ת��
{
    float Theta = (3.14 / 180) * Angle;
    float Xd = *PX - CX;
    float Yd = *PY - CY;

    *PX = (Xd)*cos(Theta) - (Yd)*sin(Theta) + CX;// + 0.5;
    *PY = (Xd)*sin(Theta) + (Yd)*cos(Theta) + CY;// + 0.5;
}

void Game_Of_Life_Turn(void)            //ˢ����Ϸ�غ�
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

void Game_Of_Life_Seed(s16 seed)		//��Ϸ��ʼ������
{
    srand(seed);
    for (u8 i = 0; i < 8; i++)
        for (u8 j = 0; j < 128; j++)
        {
            Nodes_Cache[i][j] = rand();
        }
    Update_Display();
}

void Game_Of_Life_Play(void)            //��Ϸ��ʼ
{


    u8 x1 = 8, x2 = 16, y1 = 32, y2 = 32;
    s8 shift = -1;


    OLED_Clear();
    u8 i, j;
    for (j = 0; j < 8; j++)				//����8ҳ
    {
        for (i = 0; i < 128; i++)			//����128��
        {
            Nodes_Cache[j][i] = 0x00;	//���Դ���������ȫ������
        }
    }


    //	Game_Of_Life_Seed(1);		//��������

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

//***************************** �տ֧���� *****************************//

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
    OLED_WriteCommand(0x40);	//������ʾ��ʼ��,0x40~0x7F
    OLED_WriteCommand(0xA1);	//�������ҷ���,0xA1����,0xA0���ҷ���
    OLED_WriteCommand(0xC8);	//�������·���,0xC8����,0xC0���·���

    OLED_Clear();
    OLED_Reverse();

    switch (w)
    {
    case 0: // ΢���տ�
    {
        Show_QR_Code_L();
        OLED_Clear();
        OLED_ShowImage(41, 0, 45, 45, SKM_WX);
        OLED_ShowString(40, 51, "΢���տ�", OLED_6X8);
        Show_QR_Code_L1();
        break;
    }
    case 1: // ΢��֧��
    {
        Show_QR_Code_R();
        OLED_Clear();
        OLED_ShowImage(41, 0, 45, 45, ZFM_WX);
        OLED_ShowString(40, 51, "΢��֧��", OLED_6X8);
        Show_QR_Code_R1();
        break;
    }
    case 2: // ֧�����տ�
    {
        Show_QR_Code_R();
        OLED_Clear();
        OLED_ShowImage(41, 0, 45, 45, SKM_ZFB);
        OLED_ShowString(34, 51, "֧�����տ�", OLED_6X8);
        Show_QR_Code_R1();
        break;
    }
    case 3: // ֧����֧��
    {
        Show_QR_Code_L();
        OLED_Clear();
        OLED_ShowImage(41, 0, 45, 45, ZFM_ZFB);
        OLED_ShowString(34, 51, "֧����֧��", OLED_6X8);
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


//***************************** �����ʽ *****************************//

extern u8 Cursor;

void System_Cursor_BKP(u8 ttt)
{
    u16 temp;PWR_BackupAccessCmd(ENABLE);
    temp = (BKP_ReadBackupRegister(BKP_DR3) & 0x0fff) | (Cursor << 12);
    BKP_WriteBackupRegister(BKP_DR3, temp); //�����ʽд�뱸�ݼĴ��� BKP_DR3 15-12λ

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
        {"- ����",  Function,    RETURN},
        {"- ��ɫ",  Function,    System_Cursor1},
        {"- ����",  Function,    System_Cursor2},
        {"- ָ��",  Function,    System_Cursor3},
        {".."}
    };
    run_menu2(option_list);
}


//***************************** ̧���� *****************************//


u8 RAWA = 1;		//̧�������ñ�־λ
u8 RAWAtri = 60;	//̧�󴥷�ֵ
u16 RAWAtrigger;	//ʵ��̧�󴥷�ֵ

// ̧������
void Raise_Wake(void)
{
    if (WorkingMode != SuperSaving)    //����͹�������
    {
        MPU_Init();
        RAWAtrigger = RAWAtri * 70;

        // u16 temp;PWR_BackupAccessCmd(ENABLE);
        // temp = (BKP_ReadBackupRegister(BKP_DR4) & 0x00ff) | (8 << RAWAtri);
        // BKP_WriteBackupRegister(BKP_DR4, temp); //̧�󴥷�ֵд�뱸�ݼĴ��� BKP_DR4 ��8λ

        u16 temp;PWR_BackupAccessCmd(ENABLE);
        temp = (BKP_ReadBackupRegister(BKP_DR4) & 0xfff0) | RAWA;
        BKP_WriteBackupRegister(BKP_DR4, temp); //̧�������ñ�־λд�뱸�ݼĴ���BKP_DR4 3-0λ

    }
    else
    {
        OLED_DrawRectangle(0, 31, 96, 14, OLED_UNFILLED);
        OLED_ShowString(1, 32, "��رճ���ʡ��", OLED_6X8);
        OLED_Update();
        RAWA = 0;
        delay_ms(500);
        return;
    }
}

// ̧������������ֵ
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


    BKP_WriteBackupRegister(BKP_DR4, temp); //̧�󴥷�ֵд�뱸�ݼĴ��� BKP_DR4 ��8λ



}



//***************************** ϵͳ��ѹ��ʾ *****************************//

extern float RTI, VIN;
void System_Voltage(void)
{


    s8 p;
    u16 ADValue;
    float voltage;
    while (1)
    {
        OLED_WriteCommand(0x40);	//������ʾ��ʼ��,0x40~0x7F
        OLED_WriteCommand(0xA1);	//�������ҷ���,0xA1����,0xA0���ҷ���
        OLED_WriteCommand(0xC8);	//�������·���,0xC8����,0xC0���·���

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


//***************************** ��Ϣ *****************************//

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
        {"- ����",Function,RETURN},
        {"---STM32F103C8T6---",Display},
        {"- RAM: 20K",Display},
        {"- FLASH: 64K",Display},
        {"-------------------",Display},
        {"- ����汾V1.0",Display},
        {"-------------------",Display},
        {"- ��������",Function,KAIJIDONGHUA},
        {".."}
    };
    run_menu2(option_list);
}


//***************************** USB��Ӧ *****************************//

void USB_CH7800(void)
{
    CH7800_Unicast(Loudness, CHvolume);    //����������������һ������оƬ,Ҫ��Ȼ����֪������˯����ʱ���ж����������
}


//***************************** ������ʾ *****************************//
void error(void)
{
    s8 p;


    OLED_Clear();
    OLED_ShowString(39, 16, "��������", OLED_6X8);
    OLED_Update();
    while (1)
    {
        OLED_WriteCommand(0x40);	//������ʾ��ʼ��,0x40~0x7F
        OLED_WriteCommand(0xA1);	//�������ҷ���,0xA1����,0xA0���ҷ���
        OLED_WriteCommand(0xC8);	//�������·���,0xC8����,0xC0���·���

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


//****************************** �ֵ�Ͳ *****************************//
s8 mubz = 50;   //�ֵ�ͲĬ������
void flashlight(void)
{
    s8 p;

    OLED_Clear();
    OLED_Update();

    TIM_TimeBaseInitTypeDef  TIM_TimeBaseSrtructure;        //TIMʱ�������ʼ���ṹ��
    TIM_OCInitTypeDef	TIM_OCInitStructure;                //TIM����ȽϹ��ܽṹ��

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   //ʹ��GPIOA����ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);    //��ӳ���迪��AFIOʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);    //ʹ��TIM3ʱ�� 

    /* �˿�(GPIO)���� */
    GPIO_InitTypeDef  GPIO_InitStructure;                   //GPIO��ʼ���ṹ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;               //PA7=T3 Ch2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       //�ٶ� 50MHz
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;         //�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);                  //��ʼ�� GPIOA	

    /* TIMʱ�������ʼ�� */
    TIM_TimeBaseSrtructure.TIM_Period = 100 - 1;           				//������ TIMx_CNT���� ��0�ۼӵ�ARR�κ����,�����Զ���װ��ֵ
    TIM_TimeBaseSrtructure.TIM_Prescaler = 72 - 1;         				//����Ԥ��Ƶϵ�� PSC
    TIM_TimeBaseSrtructure.TIM_ClockDivision = 0;                       //����ʱ�ӷָ�,���⺯��.TIM_Clock_Division_CKD����,���ֲ� 00:Tdts=Tck_int:01:Tdts=2*Tck_int(2��Ƶ)
    TIM_TimeBaseSrtructure.TIM_CounterMode = TIM_CounterMode_Up;        //TIM ���ϼ���ģʽ���
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseSrtructure);				    //��ʼ��TIM3��ʱ��

    /* TIM�����ʼ�� */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;     			    //ѡ��ʱ��ģʽ:TIM �����ȵ���ģʽ1,ģʽ1Ϊ�Ƚϵ����.
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;           //�������:TIM����Ƚϼ��Ը�
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;       //�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_Pulse = mubz;                               //����ռ�ձ�,��CCR

    TIM_OC2Init(TIM3, &TIM_OCInitStructure);

    /* ��ʼ��CH1 */
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);		//CH2 Ԥװ��ʹ��
    TIM_ARRPreloadConfig(TIM3, ENABLE);              		//ʹ�� TIMx��ARR�ϵ�Ԥװ�ؼĴ���

    TIM_Cmd(TIM3, ENABLE);									//ʹ�� TIM3	

    //	TIM_PrescalerConfig
    //	TIM_SetAutoreload

    float GP = 0, GP_target;                 //��������,Ŀ����������

    while (1)
    {
        OLED_WriteCommand(0x40);	//������ʾ��ʼ��,0x40~0x7F
        OLED_WriteCommand(0xA1);	//�������ҷ���,0xA1����,0xA0���ҷ���
        OLED_WriteCommand(0xC8);	//�������·���,0xC8����,0xC0���·���

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
            TIM_SetCompare2(TIM3, (u8)mubz);//дCRR

        }
        else if (p == -1)
        {
            mubz--;
            if (mubz < 0)mubz = 0;
            TIM_SetCompare2(TIM3, (u8)mubz);//дCRR
        }
        p = menu_Enter_event();
        if (p)
        {
            TIM_SetCompare2(TIM3, 0);//дCRR


            OLED_transition(0);

            TIM_Cmd(TIM3, DISABLE);//TIM3ʧ��

            /*���ݼĴ�������ʹ��*/
            u16 temp;PWR_BackupAccessCmd(ENABLE);
            temp = BKP_ReadBackupRegister(BKP_DR2) | mubz << 8;
            BKP_WriteBackupRegister(BKP_DR2, temp); //�ֵ�Ͳ����д��߰�λ���ݼĴ���

            return;
        }
    }
}


//***************************** ��Ļ���� *****************************//

void System_Brightness(void)
{
    u16 temp;PWR_BackupAccessCmd(ENABLE);
    temp = (BKP_ReadBackupRegister(BKP_DR2) & 0xff00) | liangdu;
    BKP_WriteBackupRegister(BKP_DR2, temp); //��Ļ����д�뱸�ݼĴ��� BKP_DR2 �Ͱ�λ,�߰�λΪ�ֵ�Ͳ����ֵ
}


//****************************** ��� *****************************//

void Stop_Watch(void)
{
    u8  x = 5;
    u8  y = 18;
    u8 ms = 0;  // ��λ
    u8 ms1 = 0; // ��λ
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
        OLED_WriteCommand(0x40);	//������ʾ��ʼ��,0x40~0x7F
        OLED_WriteCommand(0xA1);	//�������ҷ���,0xA1����,0xA0���ҷ���
        OLED_WriteCommand(0xC8);	//�������·���,0xC8����,0xC0���·���

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
                        if (s1 >= 6)// 60�� = 1����
                        {
                            s1 = 0;
                            min++;
                            if (min >= 10)
                            {
                                min = 0;
                                min1++;
                                if (min1 >= 6) // 60���� = 1Сʱ
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

        p = menu_Enter_event(); //��ȡ����(ȷ��)����1ȷ��,2����
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


//****************************** ʱ������ *****************************//

extern u8 Time_12_flag;
extern u8 Time_24_flag;

void System_Hour_BKP(u8 x)
{
    u16 temp;PWR_BackupAccessCmd(ENABLE);
    temp = (BKP_ReadBackupRegister(BKP_DR4) & 0xff0f) | (x << 4);
    BKP_WriteBackupRegister(BKP_DR4, temp); //ʱ���ʽ��־λд�뱸�ݼĴ��� BKP_DR4 7-4λ
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

// ʱ��ģʽ
void System_Time_format(void)
{
    struct option_class2 option_list[] =
    {
        {"- ����",          Function,    RETURN},
        {"- 12Сʱ��",      ON_OFF,      Time_12_Hour,    &Time_12_flag},
        {"- 24Сʱ��",      ON_OFF,      Time_24_Hour,    &Time_24_flag},
        {".."}
    };
    run_menu2(option_list);

}

void System_Time(void)
{

    struct option_class2 option_list[] =
    {
        {"- ����",          Function,   RETURN},
        {"- ʱ�����",      Function,   Time_Adjust},
        {"- ʱ���ʽ",      Function,   System_Time_format},
        {".."}
    };
    run_menu2(option_list);
}


//***************************** �Ʋ� *****************************//
extern u8 Step_Flag;
void System_Step_Counter(void)
{
    u16 temp;PWR_BackupAccessCmd(ENABLE);
    temp = (BKP_ReadBackupRegister(BKP_DR3) & 0xff0f) | (Step_Flag << 4);
    BKP_WriteBackupRegister(BKP_DR3, temp); //�Ʋ���־λд�뱸�ݼĴ��� BKP_DR3 7-4λ

    if (Step_Flag == 0)
    {
        TIM_Cmd(TIM4, DISABLE);			//ʹ��TIM4ʧ��
    }
    else
    {
        TIM_Cmd(TIM4, ENABLE);			//ʹ��TIM4,��ʱ����ʼ����
    }

}


//***************************** ģʽ�л� *****************************//

extern Mode WorkingMode;
extern u8 Normal_Flag;
extern u8 PowerSaving_Flag;
extern u8 SuperSaving_Flag;

void System_Mode_BKP(u8 f)
{

    u16 temp;PWR_BackupAccessCmd(ENABLE);
    temp = (BKP_ReadBackupRegister(BKP_DR3) & 0xf0ff) | ((f << 8) | 0xf0ff);
    BKP_WriteBackupRegister(BKP_DR3, temp); //����ģʽд�뱸�ݼĴ��� BKP_DR3 11-8λ����ģʽ��־λ
}

// ��׼ģʽ
void System_Normal_Mode(void)
{
    WorkingMode = Normal;

    Normal_Flag = 1;
    PowerSaving_Flag = 0;
    SuperSaving_Flag = 0;
    System_Mode_BKP(Normal_Flag);

}

// ʡ��ģʽ
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
        OLED_ShowString(1, 32, "�ѹر�̧������", OLED_6X8);
        OLED_Update();
        delay_ms(500);
    }

}

// ����ʡ��
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
        OLED_ShowString(1, 32, "�ѹر�̧������", OLED_6X8);
        OLED_Update();
        delay_ms(500);
    }

    if (Step_Flag)
    {
        Step_Flag = 0;
        OLED_DrawRectangle(0, 31, 84, 14, OLED_UNFILLED);
        OLED_ShowString(1, 32, "�ѹرռƲ�", OLED_6X8);
        OLED_Update();
        delay_ms(500);
    }



}

void System_Mode_Switch(void)
{
    struct option_class2 option_list[] = {
        {"- ����",			    Function,	RETURN},
        {"- ��׼ģʽ",			ON_OFF,		System_Normal_Mode,			&Normal_Flag},
        {"- ʡ��ģʽ",			ON_OFF,		System_Saving_Mode,			&PowerSaving_Flag},
        {"- ����ʡ��",			ON_OFF,		System_Super_Mode,			&SuperSaving_Flag},
        {".."}								//��β��־,�����������
    };
    run_menu2(option_list);
}



//����ģ��

//void xxxxxxx(void)
//{
//	s8 p;
//	
//	while(1)
//	{
//		
//       OLED_transition(1);
//	   p = menu_Roll_event(); //��ȡ����(����)����1Ϊ��,-1Ϊ��  
//	   if( p == 1){ 
//	   }	 
//       else if( p == -1){ 
//	   }	
//		p=menu_Enter_event(); //��ȡ����(ȷ��)����1ȷ��,2����
//		if(p==1){
//			
//			OLED_transition(0);
//			
//			//����ĳĳ����//

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






//****************************** ���� ******************************//

void System_Settings(void)
{
    struct option_class2 option_list[] = {
        {"- ����",			Function,	RETURN},
        {"- ģʽ�л�",      Function , 	 System_Mode_Switch},
        {"- ̧������",		ON_OFF ,	 Raise_Wake,			&RAWA},
        {"- ̧�����ٶ�",	 Number ,	 Raise_Wake_trigger,	&RAWAtri},
        {"- �Ʋ�",          ON_OFF,     System_Step_Counter,    &Step_Flag},
        {"- ����������",    Number,      Display,			    &CHvolume},
        {"- ��Ļ����",      Number,      System_Brightness,     &liangdu},
        {"- ʱ������",      Function,    System_Time},
        {"- �����ʽ",		Function, 	 System_Cursor},
        {"- ϵͳ��ѹ",		Function,	 System_Voltage},
        {"- USB ��Ӧ",      Function,	 USB_CH7800},
        {"- ��Ϣ",			Function,	 System_Information},
        {".."}								//��β��־,�����������
    };
    run_menu2(option_list);

    System_Brightness();
}


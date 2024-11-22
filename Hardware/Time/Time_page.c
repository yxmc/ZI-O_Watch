#include "Time_page.h"

// 定义时间格式
u8 Time_12_flag = 0;
u8 Time_24_flag = 1;


//定义电压与电池容量关系的数组
const float battery_voltage[31] = { 6.6, 4.35,  4.18, 4.15,4.12,4.10, 4.08, 4.05, 4.03, 3.97, 3.93, 3.90, 3.87, 3.84, 3.81, 3.79, 3.77, 3.76, 3.74, 3.73, 3.72, 3.71, 3.69, 3.66, 3.65, 3.64, 3.63, 3.60, 3.55, 3.50, 3.40 };
const u8 battery_level[31] = { 255,  100,   100,   98,  96,  93,   90,   87,   85,   80,   75,   70,   65,   60,   55,   50,   45,   42,   35,   30,   25,   20,   17,   14,   11,    8,    6,    5,    4,    2,  0 };

float VIN = 3.28;            //STM32电源脚电压,这个也要校准,可以用万用表测量降压芯片的输出3.3v或者直接测量单片机对屏幕的RES脚
float UP_BAT = 100;          //上次电池电量
float BAT_volt_CAL = 2.000;	 //电池电压校准系数  

//获取电池电量并显示在指定位置
void Time_page_BAT(u8 x, u8 y)
{
	static u8 ADVx = 0;
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12) == 0) //如果在充电
	{
		//一个小动画//
		if (ADVx > 3 && ADVx < 33) { OLED_DrawLine(x + 21, y + 2, x + 21, y + 6); }
		if (ADVx > 6 && ADVx < 36) { OLED_DrawLine(x + 22, y + 2, x + 22, y + 6); }
		if (ADVx > 9 && ADVx < 39) { OLED_DrawLine(x + 23, y + 2, x + 23, y + 6); }
		if (ADVx > 12 && ADVx < 42) { OLED_DrawLine(x + 24, y + 2, x + 24, y + 6); }
		if (ADVx > 15 && ADVx < 45) { OLED_DrawLine(x + 25, y + 2, x + 25, y + 6); }
		if (ADVx > 18 && ADVx < 48) { OLED_DrawLine(x + 26, y + 2, x + 26, y + 6); }
		if (ADVx > 21 && ADVx < 51) { OLED_DrawLine(x + 27, y + 2, x + 27, y + 6); }
		if (ADVx > 24 && ADVx < 54) { OLED_DrawLine(x + 28, y + 2, x + 28, y + 6); }
		if (ADVx > 27 && ADVx < 57) { OLED_DrawLine(x + 29, y + 2, x + 29, y + 6); }
		if (ADVx > 30 && ADVx < 60) { OLED_DrawLine(x + 30, y + 2, x + 30, y + 6); }
		ADVx++;
		if (ADVx > 60) ADVx = 0;

	}
	else
	{
		ADVx = 0;
		u16 ADValue;
		ADValue = AD_GetValue(ADC_Channel_8);

		float voltage = ((float)ADValue * VIN) / 4095 * BAT_volt_CAL;//得到电池电压

		for (u8 i = 0;i < 31;i++)//对应数组有31档
		{
			if ((voltage < battery_voltage[i]) & (voltage >= battery_voltage[i + 1]))
			{
				//线性插值得到电池电量
				voltage = battery_level[i + 1] + ((voltage - battery_voltage[i + 1]) * ((battery_level[i] - battery_level[i + 1]) / (battery_voltage[i] - battery_voltage[i + 1])));
				//此时voltage为电池电量 100 到 0
				break;
			}
		}

		if (voltage < UP_BAT) //防止电量上下抖动
		{
			UP_BAT = voltage;
		}
		else if (voltage - UP_BAT > 10)//如果电量增加
		{
			UP_BAT = voltage;
		}

	}

	OLED_DrawRectangle(x + 20, y + 1, 12, 7, OLED_UNFILLED);	//画电池图案
	OLED_DrawLine(x + 32, y + 3, x + 32, y + 5);                //画电池图案



	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12) == 1)//如果没在充电
	{
		if (UP_BAT > 9)OLED_DrawLine(x + 21, y + 2, x + 21, y + 6);
		if (UP_BAT > 19)OLED_DrawLine(x + 22, y + 2, x + 22, y + 6);
		if (UP_BAT > 29)OLED_DrawLine(x + 23, y + 2, x + 23, y + 6);
		if (UP_BAT > 39)OLED_DrawLine(x + 24, y + 2, x + 24, y + 6);
		if (UP_BAT > 49)OLED_DrawLine(x + 25, y + 2, x + 25, y + 6);
		if (UP_BAT > 59)OLED_DrawLine(x + 26, y + 2, x + 26, y + 6);
		if (UP_BAT > 69)OLED_DrawLine(x + 27, y + 2, x + 27, y + 6);
		if (UP_BAT > 79)OLED_DrawLine(x + 28, y + 2, x + 28, y + 6);
		if (UP_BAT > 89)OLED_DrawLine(x + 29, y + 2, x + 29, y + 6);//根据电池电量来绘制格子 
		if (UP_BAT >= 100 || GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == 0)//3位数//或充电完成
		{
			OLED_DrawLine(x + 30, y + 2, x + 30, y + 6);
			OLED_ShowNum(x, y + 1, 100, 3, OLED_6X8);
		}
		else OLED_ShowNum(x + 6, y + 1, UP_BAT, 2, OLED_6X8);
	}
	else//当充电时不显示电量,因为不准
	{
		OLED_ShowASCII(x + 6, y + 1, ">>", OLED_6X8);
	}

}

//在指定位置显示表盘数字; i:0-9; p:0大数字,1小数字;
void Time_page_figures(u8 x, u8 y, u8 i, u8 p)
{
	if (p)
	{
		switch (i)
		{
		case 1:OLED_ShowImage(x, y, 13, 30, t1);break;
		case 2:OLED_ShowImage(x, y, 13, 30, t2);break;
		case 3:OLED_ShowImage(x, y, 13, 30, t3);break;
		case 4:OLED_ShowImage(x, y, 13, 30, t4);break;
		case 5:OLED_ShowImage(x, y, 13, 30, t5);break;
		case 6:OLED_ShowImage(x, y, 13, 30, t6);break;
		case 7:OLED_ShowImage(x, y, 13, 30, t7);break;
		case 8:OLED_ShowImage(x, y, 13, 30, t8);break;
		case 9:OLED_ShowImage(x, y, 13, 30, t9);break;
		case 0:OLED_ShowImage(x, y, 13, 30, t0);break;
		}
	}
	else
	{
		switch (i)
		{
		case 1:OLED_ShowImage(x, y, 9, 21, i1);break;
		case 2:OLED_ShowImage(x, y, 9, 21, i2);break;
		case 3:OLED_ShowImage(x, y, 9, 21, i3);break;
		case 4:OLED_ShowImage(x, y, 9, 21, i4);break;
		case 5:OLED_ShowImage(x, y, 9, 21, i5);break;
		case 6:OLED_ShowImage(x, y, 9, 21, i6);break;
		case 7:OLED_ShowImage(x, y, 9, 21, i7);break;
		case 8:OLED_ShowImage(x, y, 9, 21, i8);break;
		case 9:OLED_ShowImage(x, y, 9, 21, i9);break;
		case 0:OLED_ShowImage(x, y, 9, 21, i0);break;
		}
	}
}

void Time_page_Timer(u8 x, u8 y)	//获取时间并在指定位置显示
{

	MyRTC_ReadTime();
	u16 i = MyRTC_Time[3];
	u16 p = i;

	if (Time_12_flag == 1)
	{
		if (i > 12)
		{
			i = i - 12;
			p = i;
			OLED_ShowASCII(x + 109, y + 21, "PM", OLED_6X8);
		}
		else OLED_ShowASCII(x + 109, y + 21, "AM", OLED_6X8);

	}


	i = i % 10;
	Time_page_figures(x + 19, y + 0, i, 1);
	i = (p - i) / 10;
	Time_page_figures(x + 0, y + 0, i, 1);  //计算要显示的数字

	OLED_DrawRectangle(x + 37, y + 6, 2, 4, OLED_UNFILLED);
	OLED_DrawRectangle(x + 37, y + 19, 2, 4, OLED_UNFILLED);

	i = MyRTC_Time[4];
	p = i;
	i = i % 10;
	Time_page_figures(x + 63, y + 0, i, 1);
	i = (p - i) / 10;
	Time_page_figures(x + 44, y + 0, i, 1);

	i = MyRTC_Time[5];
	p = i;
	i = i % 10;
	Time_page_figures(x + 95, y + 8, i, 0);
	i = (p - i) / 10;
	Time_page_figures(x + 81, y + 8, i, 0);
}

void Time_page_date(u8 x, u8 y)
{
	// OLED_ShowNum(x, y, MyRTC_Time[0], 4, OLED_6X8);
	// OLED_ShowNum(x + 26, y, MyRTC_Time[1], 2, OLED_6X8);
	// OLED_ShowNum(x + 40, y, MyRTC_Time[2], 2, OLED_6X8);
	OLED_ShowNum(x, y, step_cnt, 4, OLED_6X8);
}

void Time_page_week(u8 x, u8 y)
{

	u16 week = MyRTC_Time[6];

	OLED_ShowASCII(x, y, "week:", OLED_6X8);
	switch (week)
	{
	case 1:OLED_ShowASCII(x + 30, y, "1", OLED_6X8);break;
	case 2:OLED_ShowASCII(x + 30, y, "2", OLED_6X8);break;
	case 3:OLED_ShowASCII(x + 30, y, "3", OLED_6X8);break;
	case 4:OLED_ShowASCII(x + 30, y, "4", OLED_6X8);break;
	case 5:OLED_ShowASCII(x + 30, y, "5", OLED_6X8);break;
	case 6:OLED_ShowASCII(x + 30, y, "6", OLED_6X8);break;
	case 0:OLED_ShowASCII(x + 30, y, "R", OLED_6X8);break;

	}

}


void Time_Adjust(void)
{

	s8 p;
	u8 i = 0;
	u16 t = 0;
	while (1)
	{

		OLED_WriteCommand(0x40);	//设置显示开始行,0x40~0x7F

		OLED_WriteCommand(0xA1);	//设置左右方向,0xA1正常,0xA0左右反置

		OLED_WriteCommand(0xC8);	//设置上下方向,0xC8正常,0xC0上下反置

		OLED_transition(1);

		OLED_Clear();
		OLED_ShowNum(6, 5, MyRTC_Time[0], 4, OLED_6X8);
		OLED_ShowNum(41, 5, MyRTC_Time[1], 2, OLED_6X8);
		OLED_ShowNum(66, 5, MyRTC_Time[2], 2, OLED_6X8);
		OLED_ShowNum(6, 20, MyRTC_Time[3], 2, OLED_6X8);
		OLED_ShowNum(31, 20, MyRTC_Time[4], 2, OLED_6X8);
		OLED_ShowNum(56, 20, MyRTC_Time[5], 2, OLED_6X8);

		switch (i)
		{
		case 0:OLED_ShowASCII(0, 5, "-", OLED_6X8);break;
		case 1:OLED_ShowASCII(35, 5, "-", OLED_6X8);break;
		case 2:OLED_ShowASCII(60, 5, "-", OLED_6X8);break;
		case 3:OLED_ShowASCII(0, 20, "-", OLED_6X8);break;
		case 4:OLED_ShowASCII(25, 20, "-", OLED_6X8);break;
		case 5:OLED_ShowASCII(50, 20, "-", OLED_6X8);break;
		}

		OLED_Update();

		p = menu_Roll_event();
		if (p == 1) {

			t = MyRTC_Time[i];
			t++;
			MyRTC_Time[i] = t;

		}
		else if (p == -1) {

			t = MyRTC_Time[i];
			t--;
			MyRTC_Time[i] = t;

		}
		p = menu_Enter_event();
		if (p == 1)
		{
			i++;
			if (i >= 6)
			{
				MyRTC_SetTime();


				OLED_transition(0);

				return;
			}


		}

	}

}

void Time_page_small_Timer(u8 x, u8 y)//获取时间并在指定位置显示
{

	MyRTC_ReadTime();
	u16 i = MyRTC_Time[3];

	if (Time_12_flag == 1)
	{
		if (i > 12)
		{
			i = i - 12;

			OLED_ShowASCII(x + 42, y, "PM", OLED_6X8);
		}
		else OLED_ShowASCII(x + 42, y, "AM", OLED_6X8);

	}


	OLED_ShowNum(x, y, i, 2, OLED_6X8);

	i = MyRTC_Time[4];

	OLED_ShowNum(x + 14, y, i, 2, OLED_6X8);

	i = MyRTC_Time[5];

	OLED_ShowNum(x + 28, y, i, 2, OLED_6X8);

}

void Time_page_io_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;//电池充电检测脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;     //电池检测电路控制到地
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOA, GPIO_Pin_15);
}

void Time_page(void)
{
	OLED_Clear();
	Time_page_BAT(89, 0);
	Time_page_Timer(5, 18);
	Time_page_date(0, 56);
	Time_page_week(90, 57);
	OLED_Update();
}



#include "CH7800PLAY.h" 
#include "MyRTC.h"

u8 CHvolume = 25;	//!模块音量	最好和配置文件里面一样

//CH7800 单播
void CH7800_Unicast(u8 CMD, u8 DATA)
{
	u8 Send_buf[8];

	Send_buf[0] = 0x7E;    //头
	Send_buf[1] = CMD;     //控制指令
	Send_buf[2] = 0x00;
	Send_buf[3] = 0x02;    //数据长度
	Send_buf[4] = 0x00;
	Send_buf[5] = DATA;    //data
	Send_buf[6] = 0xEF;    //尾

	for (u8 i = 0; i < 7; i++)	//遍历数组
	{
		Serial3_SendByte(Send_buf[i]);//依次调用Serial3_SendByte发送每个字节数据
	}

}

// CH7800 组播
void CH7800_Multicast(u8 Send_buf[], u8 len)
{

	for (u8 i = 0; i <= len; i++)	//遍历数组
	{
		Serial3_SendByte(Send_buf[i]);//依次调用Serial3_SendByte发送每个字节数据
	}

}

//唤醒语音循环播放,1--3,3段
void CH7800_Unicast_reply(void)
{
	static u8 reply = 0;

	reply++;
	CH7800_Unicast(0x16, CHvolume);
	delay_ms(100);
	CH7800_Unicast(0x13, reply);

	if (reply >= 3)reply = 0;
}

//回复语音循环播放,4--6,3段
void CH7800_Unicast_return(void)
{
	static u8 retu = 3;

	retu++;
	CH7800_Unicast(0x16, CHvolume);
	delay_ms(100);
	CH7800_Unicast(0x13, retu);

	if (retu >= 6)retu = 3;
}

// u16 MyRTC_Time[] = { 2024, 11, 6, 14, 10, 0, 3 };	//定义全局的时间数组,数组内容分别为年、月、日、时、分、秒、星期


// 报时
void CH7800_TelltheTime()
{
	MyRTC_ReadTime(); //读取系统时间

	u8 falg = 0;

	CH7800_Unicast(0x16, CHvolume);
	delay_ms(100);

	u8 Send_buf[12];

	Send_buf[0] = 0x7E;    //头
	Send_buf[1] = 0x31;    //组播控制指令
	Send_buf[2] = 0x00;
	
	Send_buf[4] = 0x07;    //当前时间是
	Send_buf[5] = MyRTC_Time[3] <= 12 ? 0x08 : 0x09;    //上午 下午
	Send_buf[6] = (((MyRTC_Time[3] % 12) == 0) ? 12 : MyRTC_Time[3] % 12) + 11;    // 小时
	Send_buf[7] = 0x0A;    //点


	u8 minute = MyRTC_Time[4];  // 分钟
	if (minute < 10)
	{
		Send_buf[8] = minute + 11;
	}
	else if (minute == 10)
	{
		Send_buf[8] = 0x15;
	}
	else
	{
		u8 tens = minute / 10;
		u8 units = minute % 10;
		Send_buf[8] = (tens == 2) ? 0x18 : (tens == 3) ? 0x19 : 0x1A; // 对应 20, 30, 40 的处理

		if (units > 0)
		{
			Send_buf[9] = units + 11;
			falg = 1;
		}
	}

	if (falg == 1)
	{
		Send_buf[3] = 0x06;    //数据长度
		Send_buf[10] = 0x0B; 	//分
		Send_buf[11] = 0xEF;    //尾
	
	}
	else
	{
		Send_buf[3] = 0x05;    //数据长度
		Send_buf[9] = 0x0B; 	//分
		Send_buf[10] = 0xEF;    //尾
	}

	CH7800_Multicast(Send_buf, falg == 1 ? 11 : 10);
}
















// //报时
// void CH7800_TelltheTime()
// {
// 	MyRTC_ReadTime();	//读取系统时间

// 	CH7800_Unicast(0x16, CHvolume);
// 	delay_ms(100);
// 	CH7800_Unicast(0x13, 0x08);	//当前时间是
// 	delay_ms(100);

// 	if (MyRTC_Time[3] <= 12)   //上午或下午
// 	{
// 		CH7800_Unicast(0x13, 0x08);	//上午
// 		delay_ms(100);
// 	}
// 	else
// 	{
// 		CH7800_Unicast(0x13, 0x09);	//下午
// 		delay_ms(100);
// 	}

// 	CH7800_Unicast(0x13, (((MyRTC_Time[3] % 12) == 0) ? 12 : MyRTC_Time[3] % 12) + 11);	//小时
// 	delay_ms(100);

// 	CH7800_Unicast(0x13, 0x0A);	//点
// 	delay_ms(100);


// 	if (MyRTC_Time[4] < 10)		//分钟
// 	{
// 		CH7800_Unicast(0x13, MyRTC_Time[4] + 11);	//分钟
// 		delay_ms(100);
// 	}
// 	else if (MyRTC_Time[4] == 10)
// 	{
// 		CH7800_Unicast(0x13, 0x15);	//分钟
// 		delay_ms(100);
// 	}
// 	else if (MyRTC_Time[4] > 10 && MyRTC_Time[4] < 20)
// 	{
// 		CH7800_Unicast(0x13, 0x10);	//十
// 		delay_ms(100);
// 		CH7800_Unicast(0x13, MyRTC_Time[4] - 10 + 11);	//个位
// 		delay_ms(100);
// 	}
// 	else if (MyRTC_Time[4] == 20)
// 	{
// 		CH7800_Unicast(0x13, 0x18);	//二十
// 		delay_ms(100);
// 	}
// 	else if (MyRTC_Time[4] > 20 && MyRTC_Time[4] < 30) 
// 	{
// 		CH7800_Unicast(0x13, 0x18);	//二十
// 		delay_ms(100);
// 		CH7800_Unicast(0x13, MyRTC_Time[4] - 20 + 11);	//个位
// 		delay_ms(100);
// 	}
// 	else if (MyRTC_Time[4] == 30)
// 	{
// 		CH7800_Unicast(0x13, 0x19);	//三十
// 		delay_ms(100);
// 	}
// 	else if (MyRTC_Time[4] > 30 && MyRTC_Time[4] < 40)
// 	{
// 		CH7800_Unicast(0x13, 0x19);	//三十
// 		delay_ms(100);
// 		CH7800_Unicast(0x13, MyRTC_Time[4] - 30 + 11);	//个位
// 		delay_ms(100);
// 	}
// 	else if (MyRTC_Time[4] == 40)
// 	{
// 		CH7800_Unicast(0x13, 0x1A);	//四十
// 		delay_ms(100);
// 	}
// 	else if (MyRTC_Time[4] > 40 && MyRTC_Time[4] < 50)
// 	{
// 		CH7800_Unicast(0x13, 0x1A);	//四十
// 		delay_ms(100);
// 		CH7800_Unicast(0x13, MyRTC_Time[4] - 40 + 11);	//个位
// 		delay_ms(100);
// 	}
// 	else if (MyRTC_Time[4] == 50)
// 	{
// 		CH7800_Unicast(0x13, 0x1B);	//五十
// 		delay_ms(100);
// 	}
// 	else if (MyRTC_Time[4] > 50 && MyRTC_Time[4] < 60)
// 	{
// 		CH7800_Unicast(0x13, 0x1B);	//五十
// 		delay_ms(100);
// 		CH7800_Unicast(0x13, MyRTC_Time[4] - 50 + 11);	//个位
// 		delay_ms(100);
// 	}

// 	CH7800_Unicast(0x13, 0x0B);	//分
// 	delay_ms(100);


// }


void CH7800_Init(void)
{
	Serial3_Init();
}

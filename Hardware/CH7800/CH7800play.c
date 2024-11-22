#include "CH7800PLAY.h" 
#include "MyRTC.h"

u8 CHvolume = 25;	//!ģ������	��ú������ļ�����һ��

//CH7800 ����
void CH7800_Unicast(u8 CMD, u8 DATA)
{
	u8 Send_buf[8];

	Send_buf[0] = 0x7E;    //ͷ
	Send_buf[1] = CMD;     //����ָ��
	Send_buf[2] = 0x00;
	Send_buf[3] = 0x02;    //���ݳ���
	Send_buf[4] = 0x00;
	Send_buf[5] = DATA;    //data
	Send_buf[6] = 0xEF;    //β

	for (u8 i = 0; i < 7; i++)	//��������
	{
		Serial3_SendByte(Send_buf[i]);//���ε���Serial3_SendByte����ÿ���ֽ�����
	}

}

// CH7800 �鲥
void CH7800_Multicast(u8 Send_buf[], u8 len)
{

	for (u8 i = 0; i <= len; i++)	//��������
	{
		Serial3_SendByte(Send_buf[i]);//���ε���Serial3_SendByte����ÿ���ֽ�����
	}

}

//��������ѭ������,1--3,3��
void CH7800_Unicast_reply(void)
{
	static u8 reply = 0;

	reply++;
	CH7800_Unicast(0x16, CHvolume);
	delay_ms(100);
	CH7800_Unicast(0x13, reply);

	if (reply >= 3)reply = 0;
}

//�ظ�����ѭ������,4--6,3��
void CH7800_Unicast_return(void)
{
	static u8 retu = 3;

	retu++;
	CH7800_Unicast(0x16, CHvolume);
	delay_ms(100);
	CH7800_Unicast(0x13, retu);

	if (retu >= 6)retu = 3;
}

// u16 MyRTC_Time[] = { 2024, 11, 6, 14, 10, 0, 3 };	//����ȫ�ֵ�ʱ������,�������ݷֱ�Ϊ�ꡢ�¡��ա�ʱ���֡��롢����


// ��ʱ
void CH7800_TelltheTime()
{
	MyRTC_ReadTime(); //��ȡϵͳʱ��

	u8 falg = 0;

	CH7800_Unicast(0x16, CHvolume);
	delay_ms(100);

	u8 Send_buf[12];

	Send_buf[0] = 0x7E;    //ͷ
	Send_buf[1] = 0x31;    //�鲥����ָ��
	Send_buf[2] = 0x00;
	
	Send_buf[4] = 0x07;    //��ǰʱ����
	Send_buf[5] = MyRTC_Time[3] <= 12 ? 0x08 : 0x09;    //���� ����
	Send_buf[6] = (((MyRTC_Time[3] % 12) == 0) ? 12 : MyRTC_Time[3] % 12) + 11;    // Сʱ
	Send_buf[7] = 0x0A;    //��


	u8 minute = MyRTC_Time[4];  // ����
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
		Send_buf[8] = (tens == 2) ? 0x18 : (tens == 3) ? 0x19 : 0x1A; // ��Ӧ 20, 30, 40 �Ĵ���

		if (units > 0)
		{
			Send_buf[9] = units + 11;
			falg = 1;
		}
	}

	if (falg == 1)
	{
		Send_buf[3] = 0x06;    //���ݳ���
		Send_buf[10] = 0x0B; 	//��
		Send_buf[11] = 0xEF;    //β
	
	}
	else
	{
		Send_buf[3] = 0x05;    //���ݳ���
		Send_buf[9] = 0x0B; 	//��
		Send_buf[10] = 0xEF;    //β
	}

	CH7800_Multicast(Send_buf, falg == 1 ? 11 : 10);
}
















// //��ʱ
// void CH7800_TelltheTime()
// {
// 	MyRTC_ReadTime();	//��ȡϵͳʱ��

// 	CH7800_Unicast(0x16, CHvolume);
// 	delay_ms(100);
// 	CH7800_Unicast(0x13, 0x08);	//��ǰʱ����
// 	delay_ms(100);

// 	if (MyRTC_Time[3] <= 12)   //���������
// 	{
// 		CH7800_Unicast(0x13, 0x08);	//����
// 		delay_ms(100);
// 	}
// 	else
// 	{
// 		CH7800_Unicast(0x13, 0x09);	//����
// 		delay_ms(100);
// 	}

// 	CH7800_Unicast(0x13, (((MyRTC_Time[3] % 12) == 0) ? 12 : MyRTC_Time[3] % 12) + 11);	//Сʱ
// 	delay_ms(100);

// 	CH7800_Unicast(0x13, 0x0A);	//��
// 	delay_ms(100);


// 	if (MyRTC_Time[4] < 10)		//����
// 	{
// 		CH7800_Unicast(0x13, MyRTC_Time[4] + 11);	//����
// 		delay_ms(100);
// 	}
// 	else if (MyRTC_Time[4] == 10)
// 	{
// 		CH7800_Unicast(0x13, 0x15);	//����
// 		delay_ms(100);
// 	}
// 	else if (MyRTC_Time[4] > 10 && MyRTC_Time[4] < 20)
// 	{
// 		CH7800_Unicast(0x13, 0x10);	//ʮ
// 		delay_ms(100);
// 		CH7800_Unicast(0x13, MyRTC_Time[4] - 10 + 11);	//��λ
// 		delay_ms(100);
// 	}
// 	else if (MyRTC_Time[4] == 20)
// 	{
// 		CH7800_Unicast(0x13, 0x18);	//��ʮ
// 		delay_ms(100);
// 	}
// 	else if (MyRTC_Time[4] > 20 && MyRTC_Time[4] < 30) 
// 	{
// 		CH7800_Unicast(0x13, 0x18);	//��ʮ
// 		delay_ms(100);
// 		CH7800_Unicast(0x13, MyRTC_Time[4] - 20 + 11);	//��λ
// 		delay_ms(100);
// 	}
// 	else if (MyRTC_Time[4] == 30)
// 	{
// 		CH7800_Unicast(0x13, 0x19);	//��ʮ
// 		delay_ms(100);
// 	}
// 	else if (MyRTC_Time[4] > 30 && MyRTC_Time[4] < 40)
// 	{
// 		CH7800_Unicast(0x13, 0x19);	//��ʮ
// 		delay_ms(100);
// 		CH7800_Unicast(0x13, MyRTC_Time[4] - 30 + 11);	//��λ
// 		delay_ms(100);
// 	}
// 	else if (MyRTC_Time[4] == 40)
// 	{
// 		CH7800_Unicast(0x13, 0x1A);	//��ʮ
// 		delay_ms(100);
// 	}
// 	else if (MyRTC_Time[4] > 40 && MyRTC_Time[4] < 50)
// 	{
// 		CH7800_Unicast(0x13, 0x1A);	//��ʮ
// 		delay_ms(100);
// 		CH7800_Unicast(0x13, MyRTC_Time[4] - 40 + 11);	//��λ
// 		delay_ms(100);
// 	}
// 	else if (MyRTC_Time[4] == 50)
// 	{
// 		CH7800_Unicast(0x13, 0x1B);	//��ʮ
// 		delay_ms(100);
// 	}
// 	else if (MyRTC_Time[4] > 50 && MyRTC_Time[4] < 60)
// 	{
// 		CH7800_Unicast(0x13, 0x1B);	//��ʮ
// 		delay_ms(100);
// 		CH7800_Unicast(0x13, MyRTC_Time[4] - 50 + 11);	//��λ
// 		delay_ms(100);
// 	}

// 	CH7800_Unicast(0x13, 0x0B);	//��
// 	delay_ms(100);


// }


void CH7800_Init(void)
{
	Serial3_Init();
}

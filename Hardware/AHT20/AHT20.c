#include "stm32f10x.h" 
#include "AHT20.h" 
#include "OLED.h"

//AHT20��ȡ����   /ʪ��ֵc1/�¶�ֵt1
void AHT20_Get_data(int* c1, int* t1)
{
	static u16 yans = 10000;
	static u8 yans2 = 100;

	if (yans > 300)//�Ƴ�һ��ʱ��(Լ5��)
	{
		yans = 0;

		u32 CT_data[2];
		AHT20_Read_CTdata(CT_data);

		*c1 = CT_data[0] * 100 * 10 / 1024 / 1024;  //����õ�ʪ��ֵc1(�Ŵ���10��,���c1=523,��ʾ����ʪ��Ϊ52.3%)
		*t1 = CT_data[1] * 200 * 10 / 1024 / 1024 - 500;//����õ��¶�ֵt1(�Ŵ���10��,���t1=245,��ʾ�����¶�Ϊ24.5��)

		if (yans2 > 5) {
			yans2 = 0;
			if ((AHT20_Read_Status() & 0x18) != 0x18) {
				AHT20_Start_Init(); //���³�ʼ���Ĵ���,һ�㲻��Ҫ�˳�ʼ��,ֻ�е����ص�״̬�ֽڲ���ȷʱ�ų�ʼ��AHT20
			}
		}
		else yans2++;
	}
	else yans++;

}


//AHT20��ʼ��
void AHT20_Init(void)
{
	Init_I2C_Sensor_Port();
}



/*******************************************/
/*@��Ȩ����:���ݰ��ɵ������޹�˾          */
/*@����:��ʪ�ȴ�������ҵ��                */
/*@�汾:V1.2                              */
/*******************************************/
void Delay_N10us(u32 t)//��ʱ����
{
	u32 k;
	while (t--)
	{
		for (k = 0; k < 2; k++);//110
	}
}
void SensorDelay_us(u32 t)//��ʱ����
{
	for (t = t - 2; t > 0; t--)
	{
		Delay_N10us(1);
	}
}
void Delay_4us(void)		//��ʱ����
{
	Delay_N10us(1);
	Delay_N10us(1);
	Delay_N10us(1);
	Delay_N10us(1);
}
void Delay_5us(void)		//��ʱ����
{
	Delay_N10us(1);
	Delay_N10us(1);
	Delay_N10us(1);
	Delay_N10us(1);
	Delay_N10us(1);
}

void Delay_1ms(u32 t)		//��ʱ����
{
	while (t--)
	{

		SensorDelay_us(1000);//////��ʱ1ms
	}
}


void AHT20_Clock_Init(void)		//��ʱ����
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
}

void SDA_Pin_Output_High(void)   //��PA5����Ϊ��� , ������Ϊ�ߵ�ƽ, PA5��ΪI2C��SDA
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;//��©���
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_SetBits(GPIOA, GPIO_Pin_5);
}

void SDA_Pin_Output_Low(void)  //��PA5����Ϊ���  ������Ϊ�͵�ƽ
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;//��©���
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_ResetBits(GPIOA, GPIO_Pin_5);
}

void SDA_Pin_IN_FLOATING(void)  //SDA����Ϊ��������
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;//
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void SCL_Pin_Output_High(void) //SCL����ߵ�ƽ,PA4��ΪI2C��SCL
{
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
}

void SCL_Pin_Output_Low(void) //SCL����͵�ƽ
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);
}

void Init_I2C_Sensor_Port(void) //��ʼ��I2C�ӿ�,���Ϊ�ߵ�ƽ
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	AHT20_Clock_Init();
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;//��©���
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_SetBits(GPIOA, GPIO_Pin_5);//����ߵ�ƽ	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;//��©���
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_SetBits(GPIOA, GPIO_Pin_4);//����ߵ�ƽ	
}

void I2C_Start(void)		 //I2C��������START�ź�
{
	SDA_Pin_Output_High();
	SensorDelay_us(8);
	SCL_Pin_Output_High();
	SensorDelay_us(8);
	SDA_Pin_Output_Low();
	SensorDelay_us(8);
	SCL_Pin_Output_Low();
	SensorDelay_us(8);
}


void AHT20_WR_Byte(u8 Byte) //��AHT20дһ���ֽ�
{
	u8 Data, N, i;
	Data = Byte;
	i = 0x80;
	for (N = 0;N < 8;N++)
	{
		SCL_Pin_Output_Low();
		Delay_4us();
		if (i & Data)
		{
			SDA_Pin_Output_High();
		}
		else
		{
			SDA_Pin_Output_Low();
		}

		SCL_Pin_Output_High();
		Delay_4us();
		Data <<= 1;
	}

	SCL_Pin_Output_Low();
	SensorDelay_us(8);
	SDA_Pin_IN_FLOATING();
	SensorDelay_us(8);
}


u8 AHT20_RD_Byte(void)//��AHT20��ȡһ���ֽ�
{
	u8 Byte, i, a;
	Byte = 0;
	SCL_Pin_Output_Low();
	SDA_Pin_IN_FLOATING();
	SensorDelay_us(8);

	for (i = 0;i < 8;i++)
	{

		SCL_Pin_Output_High();
		Delay_5us();

		a = 0;
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5)) a = 1;
		Byte = (Byte << 1) | a;
		SCL_Pin_Output_Low();
		Delay_5us();
	}
	SDA_Pin_IN_FLOATING();
	SensorDelay_us(8);
	return Byte;
}


u8 Receive_ACK(void)   //��AHT20�Ƿ��лظ�ACK
{
	u16 CNT;
	CNT = 0;
	SCL_Pin_Output_Low();
	SDA_Pin_IN_FLOATING();
	SensorDelay_us(8);
	SCL_Pin_Output_High();
	SensorDelay_us(8);
	while ((GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5)) && CNT < 100)
		CNT++;
	if (CNT == 100)
	{
		return 0;
	}
	SCL_Pin_Output_Low();
	SensorDelay_us(8);
	return 1;
}

void Send_ACK(void)		  //�����ظ�ACK�ź�
{
	SCL_Pin_Output_Low();
	SensorDelay_us(8);
	SDA_Pin_Output_Low();
	SensorDelay_us(8);
	SCL_Pin_Output_High();
	SensorDelay_us(8);
	SCL_Pin_Output_Low();
	SensorDelay_us(8);
	SDA_Pin_IN_FLOATING();
	SensorDelay_us(8);
}

void Send_NOT_ACK(void)	//�������ظ�ACK
{
	SCL_Pin_Output_Low();
	SensorDelay_us(8);
	SDA_Pin_Output_High();
	SensorDelay_us(8);
	SCL_Pin_Output_High();
	SensorDelay_us(8);
	SCL_Pin_Output_Low();
	SensorDelay_us(8);
	SDA_Pin_Output_Low();
	SensorDelay_us(8);
}

void Stop_I2C(void)	  //һ��Э�����
{
	SDA_Pin_Output_Low();
	SensorDelay_us(8);
	SCL_Pin_Output_High();
	SensorDelay_us(8);
	SDA_Pin_Output_High();
	SensorDelay_us(8);
}

u8 AHT20_Read_Status(void)//��ȡAHT20��״̬�Ĵ���
{
	u8 Byte_first;
	I2C_Start();
	AHT20_WR_Byte(0x71);
	Receive_ACK();
	Byte_first = AHT20_RD_Byte();
	Send_NOT_ACK();
	Stop_I2C();
	return Byte_first;
}

void AHT20_SendAC(void) //��AHT20����AC����
{
	I2C_Start();
	AHT20_WR_Byte(0x70);
	Receive_ACK();
	AHT20_WR_Byte(0xac);//0xAC�ɼ�����
	Receive_ACK();
	AHT20_WR_Byte(0x33);
	Receive_ACK();
	AHT20_WR_Byte(0x00);
	Receive_ACK();
	Stop_I2C();
}

//CRCУ������:CRC8/MAXIM
//����ʽ:X8+X5+X4+1
//Poly:0011 0001  0x31
//��λ�ŵ�����ͱ�� 1000 1100 0x8c
//C��ʵ����:
u8 Calc_CRC8(u8* message, u8 Num)
{

	u8 i;
	u8 byte;
	u8 crc = 0xFF;


	for (byte = 0; byte < Num; byte++)
	{
		crc ^= (message[byte]);
		for (i = 8;i > 0;--i)
		{
			if (crc & 0x80) crc = (crc << 1) ^ 0x31;
			else crc = (crc << 1);
		}
	}
	return crc;
}

void AHT20_Read_CTdata(u32* ct) //û��CRCУ��,ֱ�Ӷ�ȡAHT20���¶Ⱥ�ʪ������
{
	volatile u8  Byte_1th = 0;
	volatile u8  Byte_2th = 0;
	volatile u8  Byte_3th = 0;
	volatile u8  Byte_4th = 0;
	volatile u8  Byte_5th = 0;
	volatile u8  Byte_6th = 0;
	u32 RetuData = 0;
	u16 cnt = 0;
	AHT20_SendAC();	//��AHT20����AC����
	Delay_1ms(80);	//��ʱ80ms����	
	cnt = 0;

	while (((AHT20_Read_Status() & 0x80) == 0x80))//ֱ��״̬bit[7]Ϊ0,��ʾΪ����״̬,��Ϊ1,��ʾæ״̬
	{
		SensorDelay_us(1508);

		if (cnt++ >= 100)
		{
			break;
		}
	}
	I2C_Start();
	AHT20_WR_Byte(0x71);
	Receive_ACK();
	Byte_1th = AHT20_RD_Byte();//״̬��,��ѯ��״̬Ϊ0x98,��ʾΪæ״̬,bit[7]Ϊ1;״̬Ϊ0x1C,����0x0C,����0x08��ʾΪ����״̬,bit[7]Ϊ0
	Send_ACK();
	Byte_2th = AHT20_RD_Byte();//ʪ��
	Send_ACK();
	Byte_3th = AHT20_RD_Byte();//ʪ��
	Send_ACK();
	Byte_4th = AHT20_RD_Byte();//ʪ��/�¶�
	Send_ACK();
	Byte_5th = AHT20_RD_Byte();//�¶�
	Send_ACK();
	Byte_6th = AHT20_RD_Byte();//�¶�
	Send_NOT_ACK();
	Stop_I2C();


	RetuData = (RetuData | Byte_2th) << 8;
	RetuData = (RetuData | Byte_3th) << 8;
	RetuData = (RetuData | Byte_4th);
	RetuData = RetuData >> 4;
	ct[0] = RetuData;	//ʪ��
	RetuData = 0;
	RetuData = (RetuData | Byte_4th) << 8;
	RetuData = (RetuData | Byte_5th) << 8;
	RetuData = (RetuData | Byte_6th);
	RetuData = RetuData & 0xfffff;
	ct[1] = RetuData;	//�¶�

}


void AHT20_Read_CTdata_crc(u32* ct) //CRCУ���,��ȡAHT20���¶Ⱥ�ʪ������
{

	volatile u8  Byte_1th = 0;
	volatile u8  Byte_2th = 0;
	volatile u8  Byte_3th = 0;
	volatile u8  Byte_4th = 0;
	volatile u8  Byte_5th = 0;
	volatile u8  Byte_6th = 0;
	volatile u8  Byte_7th = 0;
	u32 RetuData = 0;
	u16 cnt = 0;
	u8  CTDATA[6] = { 0 };//����CRC��������

	AHT20_SendAC();//��AHT20����AC����
	Delay_1ms(80);//��ʱ80ms����	
	cnt = 0;

	while (((AHT20_Read_Status() & 0x80) == 0x80))//ֱ��״̬bit[7]Ϊ0,��ʾΪ����״̬,��Ϊ1,��ʾæ״̬
	{
		SensorDelay_us(1508);
		if (cnt++ >= 100)
		{
			break;
		}
	}

	I2C_Start();

	AHT20_WR_Byte(0x71);
	Receive_ACK();
	CTDATA[0] = Byte_1th = AHT20_RD_Byte();//״̬��,��ѯ��״̬Ϊ0x98,��ʾΪæ״̬,bit[7]Ϊ1;״̬Ϊ0x1C,����0x0C,����0x08��ʾΪ����״̬,bit[7]Ϊ0
	Send_ACK();
	CTDATA[1] = Byte_2th = AHT20_RD_Byte();//ʪ��
	Send_ACK();
	CTDATA[2] = Byte_3th = AHT20_RD_Byte();//ʪ��
	Send_ACK();
	CTDATA[3] = Byte_4th = AHT20_RD_Byte();//ʪ��/�¶�
	Send_ACK();
	CTDATA[4] = Byte_5th = AHT20_RD_Byte();//�¶�
	Send_ACK();
	CTDATA[5] = Byte_6th = AHT20_RD_Byte();//�¶�
	Send_ACK();
	Byte_7th = AHT20_RD_Byte();//CRC����
	Send_NOT_ACK();                           //ע��: ����Ƿ���NAK
	Stop_I2C();

	if (Calc_CRC8(CTDATA, 6) == Byte_7th)
	{

		RetuData = (RetuData | Byte_2th) << 8;
		RetuData = (RetuData | Byte_3th) << 8;
		RetuData = (RetuData | Byte_4th);
		RetuData = RetuData >> 4;
		ct[0] = RetuData;	//ʪ��
		RetuData = 0;

		RetuData = (RetuData | Byte_4th) << 8;
		RetuData = (RetuData | Byte_5th) << 8;
		RetuData = (RetuData | Byte_6th);
		RetuData = RetuData & 0xfffff;
		ct[1] = RetuData;	//�¶�

	}
	else
	{
		ct[0] = 0x00;
		ct[1] = 0x00;//У����󷵻�ֵ,�ͻ����Ը����Լ���Ҫ����
	}//CRC����
}


void JH_Reset_REG(u8 addr)
{
	//����ᱬ����,���Ժ���
	u8 Byte_first, Byte_second, Byte_third;
	// u8 Byte_first, Byte_second, Byte_third, Byte_fourth;

	I2C_Start();
	AHT20_WR_Byte(0x70);//ԭ����0x70
	Receive_ACK();
	AHT20_WR_Byte(addr);
	Receive_ACK();
	AHT20_WR_Byte(0x00);
	Receive_ACK();
	AHT20_WR_Byte(0x00);
	Receive_ACK();
	Stop_I2C();

	Delay_1ms(5);//��ʱ5ms����
	I2C_Start();
	AHT20_WR_Byte(0x71);//
	Receive_ACK();
	Byte_first = AHT20_RD_Byte();
	Send_ACK();
	Byte_second = AHT20_RD_Byte();
	Send_ACK();
	Byte_third = AHT20_RD_Byte();
	Send_NOT_ACK();
	Stop_I2C();

	Delay_1ms(10);//��ʱ10ms����
	I2C_Start();
	AHT20_WR_Byte(0x70);///
	Receive_ACK();
	AHT20_WR_Byte(0xB0 | addr);////�Ĵ�������
	Receive_ACK();
	AHT20_WR_Byte(Byte_second);
	Receive_ACK();
	AHT20_WR_Byte(Byte_third);
	Receive_ACK();
	Stop_I2C();

	Byte_second = 0x00;
	Byte_third = 0x00;
}

void AHT20_Start_Init(void)
{
	JH_Reset_REG(0x1b);
	JH_Reset_REG(0x1c);
	JH_Reset_REG(0x1e);
}

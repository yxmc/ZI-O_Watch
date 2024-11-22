#include "stm32f10x.h" 
#include "AHT20.h" 
#include "OLED.h"

//AHT20获取数据   /湿度值c1/温度值t1
void AHT20_Get_data(int* c1, int* t1)
{
	static u16 yans = 10000;
	static u8 yans2 = 100;

	if (yans > 300)//推迟一段时间(约5秒)
	{
		yans = 0;

		u32 CT_data[2];
		AHT20_Read_CTdata(CT_data);

		*c1 = CT_data[0] * 100 * 10 / 1024 / 1024;  //计算得到湿度值c1(放大了10倍,如果c1=523,表示现在湿度为52.3%)
		*t1 = CT_data[1] * 200 * 10 / 1024 / 1024 - 500;//计算得到温度值t1(放大了10倍,如果t1=245,表示现在温度为24.5℃)

		if (yans2 > 5) {
			yans2 = 0;
			if ((AHT20_Read_Status() & 0x18) != 0x18) {
				AHT20_Start_Init(); //重新初始化寄存器,一般不需要此初始化,只有当读回的状态字节不正确时才初始化AHT20
			}
		}
		else yans2++;
	}
	else yans++;

}


//AHT20初始化
void AHT20_Init(void)
{
	Init_I2C_Sensor_Port();
}



/*******************************************/
/*@版权所有:广州奥松电子有限公司          */
/*@作者:温湿度传感器事业部                */
/*@版本:V1.2                              */
/*******************************************/
void Delay_N10us(u32 t)//延时函数
{
	u32 k;
	while (t--)
	{
		for (k = 0; k < 2; k++);//110
	}
}
void SensorDelay_us(u32 t)//延时函数
{
	for (t = t - 2; t > 0; t--)
	{
		Delay_N10us(1);
	}
}
void Delay_4us(void)		//延时函数
{
	Delay_N10us(1);
	Delay_N10us(1);
	Delay_N10us(1);
	Delay_N10us(1);
}
void Delay_5us(void)		//延时函数
{
	Delay_N10us(1);
	Delay_N10us(1);
	Delay_N10us(1);
	Delay_N10us(1);
	Delay_N10us(1);
}

void Delay_1ms(u32 t)		//延时函数
{
	while (t--)
	{

		SensorDelay_us(1000);//////延时1ms
	}
}


void AHT20_Clock_Init(void)		//延时函数
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
}

void SDA_Pin_Output_High(void)   //将PA5配置为输出 , 并设置为高电平, PA5作为I2C的SDA
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;//开漏输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_SetBits(GPIOA, GPIO_Pin_5);
}

void SDA_Pin_Output_Low(void)  //将PA5配置为输出  并设置为低电平
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;//开漏输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_ResetBits(GPIOA, GPIO_Pin_5);
}

void SDA_Pin_IN_FLOATING(void)  //SDA配置为浮空输入
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;//
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void SCL_Pin_Output_High(void) //SCL输出高电平,PA4作为I2C的SCL
{
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
}

void SCL_Pin_Output_Low(void) //SCL输出低电平
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);
}

void Init_I2C_Sensor_Port(void) //初始化I2C接口,输出为高电平
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	AHT20_Clock_Init();
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;//开漏输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_SetBits(GPIOA, GPIO_Pin_5);//输出高电平	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;//开漏输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_SetBits(GPIOA, GPIO_Pin_4);//输出高电平	
}

void I2C_Start(void)		 //I2C主机发送START信号
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


void AHT20_WR_Byte(u8 Byte) //往AHT20写一个字节
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


u8 AHT20_RD_Byte(void)//从AHT20读取一个字节
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


u8 Receive_ACK(void)   //看AHT20是否有回复ACK
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

void Send_ACK(void)		  //主机回复ACK信号
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

void Send_NOT_ACK(void)	//主机不回复ACK
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

void Stop_I2C(void)	  //一条协议结束
{
	SDA_Pin_Output_Low();
	SensorDelay_us(8);
	SCL_Pin_Output_High();
	SensorDelay_us(8);
	SDA_Pin_Output_High();
	SensorDelay_us(8);
}

u8 AHT20_Read_Status(void)//读取AHT20的状态寄存器
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

void AHT20_SendAC(void) //向AHT20发送AC命令
{
	I2C_Start();
	AHT20_WR_Byte(0x70);
	Receive_ACK();
	AHT20_WR_Byte(0xac);//0xAC采集命令
	Receive_ACK();
	AHT20_WR_Byte(0x33);
	Receive_ACK();
	AHT20_WR_Byte(0x00);
	Receive_ACK();
	Stop_I2C();
}

//CRC校验类型:CRC8/MAXIM
//多项式:X8+X5+X4+1
//Poly:0011 0001  0x31
//高位放到后面就变成 1000 1100 0x8c
//C现实代码:
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

void AHT20_Read_CTdata(u32* ct) //没有CRC校验,直接读取AHT20的温度和湿度数据
{
	volatile u8  Byte_1th = 0;
	volatile u8  Byte_2th = 0;
	volatile u8  Byte_3th = 0;
	volatile u8  Byte_4th = 0;
	volatile u8  Byte_5th = 0;
	volatile u8  Byte_6th = 0;
	u32 RetuData = 0;
	u16 cnt = 0;
	AHT20_SendAC();	//向AHT20发送AC命令
	Delay_1ms(80);	//延时80ms左右	
	cnt = 0;

	while (((AHT20_Read_Status() & 0x80) == 0x80))//直到状态bit[7]为0,表示为空闲状态,若为1,表示忙状态
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
	Byte_1th = AHT20_RD_Byte();//状态字,查询到状态为0x98,表示为忙状态,bit[7]为1;状态为0x1C,或者0x0C,或者0x08表示为空闲状态,bit[7]为0
	Send_ACK();
	Byte_2th = AHT20_RD_Byte();//湿度
	Send_ACK();
	Byte_3th = AHT20_RD_Byte();//湿度
	Send_ACK();
	Byte_4th = AHT20_RD_Byte();//湿度/温度
	Send_ACK();
	Byte_5th = AHT20_RD_Byte();//温度
	Send_ACK();
	Byte_6th = AHT20_RD_Byte();//温度
	Send_NOT_ACK();
	Stop_I2C();


	RetuData = (RetuData | Byte_2th) << 8;
	RetuData = (RetuData | Byte_3th) << 8;
	RetuData = (RetuData | Byte_4th);
	RetuData = RetuData >> 4;
	ct[0] = RetuData;	//湿度
	RetuData = 0;
	RetuData = (RetuData | Byte_4th) << 8;
	RetuData = (RetuData | Byte_5th) << 8;
	RetuData = (RetuData | Byte_6th);
	RetuData = RetuData & 0xfffff;
	ct[1] = RetuData;	//温度

}


void AHT20_Read_CTdata_crc(u32* ct) //CRC校验后,读取AHT20的温度和湿度数据
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
	u8  CTDATA[6] = { 0 };//用于CRC传递数组

	AHT20_SendAC();//向AHT20发送AC命令
	Delay_1ms(80);//延时80ms左右	
	cnt = 0;

	while (((AHT20_Read_Status() & 0x80) == 0x80))//直到状态bit[7]为0,表示为空闲状态,若为1,表示忙状态
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
	CTDATA[0] = Byte_1th = AHT20_RD_Byte();//状态字,查询到状态为0x98,表示为忙状态,bit[7]为1;状态为0x1C,或者0x0C,或者0x08表示为空闲状态,bit[7]为0
	Send_ACK();
	CTDATA[1] = Byte_2th = AHT20_RD_Byte();//湿度
	Send_ACK();
	CTDATA[2] = Byte_3th = AHT20_RD_Byte();//湿度
	Send_ACK();
	CTDATA[3] = Byte_4th = AHT20_RD_Byte();//湿度/温度
	Send_ACK();
	CTDATA[4] = Byte_5th = AHT20_RD_Byte();//温度
	Send_ACK();
	CTDATA[5] = Byte_6th = AHT20_RD_Byte();//温度
	Send_ACK();
	Byte_7th = AHT20_RD_Byte();//CRC数据
	Send_NOT_ACK();                           //注意: 最后是发送NAK
	Stop_I2C();

	if (Calc_CRC8(CTDATA, 6) == Byte_7th)
	{

		RetuData = (RetuData | Byte_2th) << 8;
		RetuData = (RetuData | Byte_3th) << 8;
		RetuData = (RetuData | Byte_4th);
		RetuData = RetuData >> 4;
		ct[0] = RetuData;	//湿度
		RetuData = 0;

		RetuData = (RetuData | Byte_4th) << 8;
		RetuData = (RetuData | Byte_5th) << 8;
		RetuData = (RetuData | Byte_6th);
		RetuData = RetuData & 0xfffff;
		ct[1] = RetuData;	//温度

	}
	else
	{
		ct[0] = 0x00;
		ct[1] = 0x00;//校验错误返回值,客户可以根据自己需要更改
	}//CRC数据
}


void JH_Reset_REG(u8 addr)
{
	//这里会爆警告,可以忽略
	u8 Byte_first, Byte_second, Byte_third;
	// u8 Byte_first, Byte_second, Byte_third, Byte_fourth;

	I2C_Start();
	AHT20_WR_Byte(0x70);//原来是0x70
	Receive_ACK();
	AHT20_WR_Byte(addr);
	Receive_ACK();
	AHT20_WR_Byte(0x00);
	Receive_ACK();
	AHT20_WR_Byte(0x00);
	Receive_ACK();
	Stop_I2C();

	Delay_1ms(5);//延时5ms左右
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

	Delay_1ms(10);//延时10ms左右
	I2C_Start();
	AHT20_WR_Byte(0x70);///
	Receive_ACK();
	AHT20_WR_Byte(0xB0 | addr);////寄存器命令
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

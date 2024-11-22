#include "mpu_iic.h"
#include "sys.h"
#include "Delay.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ��,δ���������,�������������κ���;
//ALIENTEK STM32F407������
//IIC ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/6
//�汾:V1.0
//��Ȩ����,����ؾ�.
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	


// ��ʼ��IIC
void MPU_IIC_Init(void)
{
	// ����ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // ����GPIOB��ʱ��

	// GPIO��ʼ��
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure); // ��PB10��PB11���ų�ʼ��Ϊ��©���

	// ����Ĭ�ϵ�ƽ
	MPU_IIC_SCL = 1;
	MPU_IIC_SDA = 1;
}

// ����IIC��ʼ�ź�
void MPU_IIC_Start(void)
{
	MPU_SDA_OUT(); // sda�����

	MPU_IIC_SDA = 1; delay_us(1);
	MPU_IIC_SCL = 1; delay_us(1); // f407��IICʱ���ܵ���3us
	MPU_IIC_SDA = 0; delay_us(1);
	MPU_IIC_SCL = 0; delay_us(1); // ǯסI2C����,׼�����ͻ�������� 
}

// ����IICֹͣ�ź�
void MPU_IIC_Stop(void)
{
	MPU_SDA_OUT(); // sda�����
	MPU_IIC_SCL = 0; delay_us(1);
	MPU_IIC_SDA = 0; delay_us(1);
	MPU_IIC_SCL = 1; delay_us(1);
	MPU_IIC_SDA = 1; delay_us(1);
}

// �ȴ�Ӧ���źŵ���
// ����ֵ:1,����Ӧ��ʧ��
//        0,����Ӧ��ɹ�
u8 MPU_IIC_Wait_Ack(void)
{
	u8 ucErrTime = 0;
	MPU_SDA_IN(); // SDA����Ϊ����  

	MPU_IIC_SDA = 1; delay_us(1);
	MPU_IIC_SCL = 1; delay_us(1);
	while (READ_SDA)
	{
		ucErrTime++;
		if (ucErrTime > 250)
		{
			MPU_IIC_Stop();
			return 1;
		}
	}
	MPU_IIC_SCL = 0; delay_us(1); // ʱ�����0 	   
	return 0;
}

// ����ACKӦ��
void MPU_IIC_Ack(void)
{
	MPU_IIC_SCL = 0; delay_us(1);
	MPU_SDA_OUT();
	MPU_IIC_SDA = 0; delay_us(1);
	MPU_IIC_SCL = 1; delay_us(1);
	MPU_IIC_SCL = 0; delay_us(1);
}

// ������ACKӦ��		    
void MPU_IIC_NAck(void)
{
	MPU_IIC_SCL = 0; delay_us(1);
	MPU_SDA_OUT();
	MPU_IIC_SDA = 1; delay_us(1);
	MPU_IIC_SCL = 1; delay_us(1);
	MPU_IIC_SCL = 0; delay_us(1);
}

// IIC����һ���ֽ�
// ���شӻ�����Ӧ��
// 1,��Ӧ��
// 0,��Ӧ��			  
void MPU_IIC_Send_Byte(u8 txd)
{
	u8 t;
	MPU_SDA_OUT();

	MPU_IIC_SCL = 0; delay_us(1); // ����ʱ�ӿ�ʼ���ݴ���
	for (t = 0; t < 8; t++)
	{
		MPU_IIC_SDA = (txd & 0x80) >> 7;
		txd <<= 1;
		delay_us(3); // ��TEA5767��������ʱ���Ǳ����
		MPU_IIC_SCL = 1; delay_us(1);
		MPU_IIC_SCL = 0; delay_us(1);
	}
}

// ��1���ֽ�,ack=1ʱ,����ACK,ack=0,����nACK   
u8 MPU_IIC_Read_Byte(unsigned char ack)
{
	unsigned char i, receive = 0;
	MPU_SDA_IN(); // SDA����Ϊ����
	for (i = 0; i < 8; i++)
	{
		MPU_IIC_SCL = 0; delay_us(1);
		MPU_IIC_SCL = 1;
		receive <<= 1;
		if (READ_SDA) receive++;
		delay_us(3);
	}
	if (!ack)
		MPU_IIC_NAck(); // ����nACK
	else
		MPU_IIC_Ack(); // ����ACK   
	return receive;
}

#include "stm32f10x.h"              
#include <stdio.h>
#include <stdarg.h>
#include "CH7800PLAY.h"
#include "string.h"             //�ַ����жϺ�����
#include "OLED.h"    
#include "MyRtc.h"

u8 Serial2_RxPacket[10];	//����������ݰ�����,���ݰ���ʽ"@XX"
u8 Serial2_RxFlag;	   		//����������ݰ���־λ

/**
  * ��    ��:���ڳ�ʼ��
  * ��    ��:��
  * �� �� ֵ:��
  */
void Serial2_Init(void)
{
	/*����ʱ��*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//����USART2��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //����GPIOA��ʱ��

	/*GPIO��ʼ��*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				//PA2����ΪUSART2_TX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;				//PA3����ΪUSART2_RX	
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/*USART��ʼ��*/
	USART_InitTypeDef USART_InitStructure;											 //����ṹ�����
	USART_InitStructure.USART_BaudRate = 9600;		   								//������
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//Ӳ��������,����Ҫ
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;           	  //ģʽ,����ģʽ�ͽ���ģʽ��ѡ��
	USART_InitStructure.USART_Parity = USART_Parity_No;		     				 //��żУ��,����Ҫ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	     				//ֹͣλ,ѡ��1λ
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  			   //�ֳ�,ѡ��8λ
	USART_Init(USART2, &USART_InitStructure);				     			  //���ṹ���������USART_Init,����USART2

	/*�ж��������*/
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);		   //�������ڽ������ݵ��ж�

	/*NVIC����*/
	NVIC_InitTypeDef NVIC_InitStructure;					       //����ṹ�����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;		      //ѡ������NVIC��USART2��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		    	 //ָ��NVIC��·ʹ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   //ָ��NVIC��·����ռ���ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		   //ָ��NVIC��·����Ӧ���ȼ�Ϊ0
	NVIC_Init(&NVIC_InitStructure);							  //���ṹ���������NVIC_Init,����NVIC����

	/*USARTʹ��*/
	USART_Cmd(USART2, ENABLE);							   //ʹ��USART2,���ڿ�ʼ����
}

void Serial2_Power_OFF(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_Cmd(USART2, DISABLE);
}


/**
  * ��    ��:���ڷ���һ���ֽ�
  * ��    ��:Byte Ҫ���͵�һ���ֽ�
  * �� �� ֵ:��
  */
void Serial2_SendByte(u8 Byte)
{
	USART_SendData(USART2, Byte);		//���ֽ�����д�����ݼĴ���,д���USART�Զ�����ʱ����
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);	//�ȴ��������
	/*�´�д�����ݼĴ������Զ����������ɱ�־λ,�ʴ�ѭ����,���������־λ*/
}

/**
  * ��    ��:���ڷ���һ������
  * ��    ��:Array Ҫ����������׵�ַ
  * ��    ��:Length Ҫ��������ĳ���
  * �� �� ֵ:��
  */
void Serial2_SendArray(u8* Array, u16 Length)
{
	u16 i;
	for (i = 0; i < Length; i++)		//��������
	{
		Serial2_SendByte(Array[i]);	  //���ε���Serial_SendByte����ÿ���ֽ�����
	}
}

///**
//  * ��    ��:���ڷ���һ���ַ���
//  * ��    ��:String Ҫ�����ַ������׵�ַ
//  * �� �� ֵ:��
//  */
//void Serial2_SendString(char *String)
//{
//	u8 i;
//	for (i = 0; String[i] != '\0'; i ++)//�����ַ�����(�ַ���),�����ַ���������־λ��ֹͣ
//	{
//		Serial2_SendByte(String[i]);	  //���ε���Serial_SendByte����ÿ���ֽ�����
//	}
//}

///**
//  * ��    ��:�η�����(�ڲ�ʹ��)
//  * �� �� ֵ:����ֵ����X��Y�η�
//  */
//uint32_t Serial2_Pow(uint32_t X, uint32_t Y)
//{
//	uint32_t Result = 1;	  //���ý����ֵΪ1
//	while (Y --)			 //ִ��Y��
//	{
//		Result *= X;	   //��X�۳˵����
//	}
//	return Result;
//}


// #include "RF_Ev1527.h"

extern u8 Voice, VoiceAPP;

/**
  * ��    ��:USART2�жϺ���
  * ��    ��:��
  * �� �� ֵ:��
  */
void USART2_IRQHandler(void)
{
	static u8 RxState = 0;		 //�����ʾ��ǰ״̬��״̬�ľ�̬����

	Serial2_RxPacket[9] = 0x66;

	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //�ж��Ƿ���USART2�Ľ����¼��������ж�
	{

		u8 RxData = USART_ReceiveData(USART2);		   //��ȡ���ݼĴ���,����ڽ��յ����ݱ���		
		/*���δ������ݰ��Ĳ�ͬ����*/

		/*��ǰ״̬Ϊ0,�������ݰ���ͷ�������֤*/
		if (RxState == 0)
		{
			if (RxData == 0xAA) //�������ȷʵ�ǰ�ͷ
			{
				Serial2_RxPacket[0] = RxData;	  //�����ݴ������ݰ������ָ��λ��				
				RxState = 1;					  //����һ��״̬
			}
		}
		/*��ǰ״̬Ϊ1,�������ݰ���һ������*/
		else if (RxState == 1)
		{
			Serial2_RxPacket[1] = RxData;		 //�����ݴ������ݰ������ָ��λ��
			Serial2_RxFlag = 1;		             //�������ݰ���־λ��1,�ɹ�����һ�����ݰ�

			RxState = 0;			             //״̬��0
		}


		// Serial2_RxPacket[9] = 0x66;

		if (Serial2_RxFlag)//���������
		{

			if (Serial2_RxPacket[1] == 0xFA)//����
			{

			}
			else if (Serial2_RxPacket[1] == 0x30)	// ���ֵ�Ͳ
			{
				Voice = 0;
				VoiceAPP = 1;
			}
			else if (Serial2_RxPacket[1] == 0x31)	// ΢���տ���
			{
				Voice = 0;
				VoiceAPP = 2;
			}
			else if (Serial2_RxPacket[1] == 0x32)	// ������
			{
				Voice = 0;
				VoiceAPP = 3;
			}
			else if (Serial2_RxPacket[1] == 0x33)	// ��ѹ����
			{
				Voice = 0;

				VoiceAPP = 4;
			}
			else if (Serial2_RxPacket[1] == 0x34)	// ��ʱ
			{
				Voice = 0;
				VoiceAPP = 5;
			}

			Serial2_RxFlag = 0;
		}

		USART_ClearITPendingBit(USART2, USART_IT_RXNE);		//�����־λ
	}


}


#include "stm32f10x.h"              
#include <stdio.h>
#include <stdarg.h>
#include "CH7800PLAY.h"
#include "string.h"             //字符串判断函数库
#include "OLED.h"    
#include "MyRtc.h"

u8 Serial2_RxPacket[10];	//定义接收数据包数组,数据包格式"@XX"
u8 Serial2_RxFlag;	   		//定义接收数据包标志位

/**
  * 函    数:串口初始化
  * 参    数:无
  * 返 回 值:无
  */
void Serial2_Init(void)
{
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//开启USART2的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //开启GPIOA的时钟

	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				//PA2复用为USART2_TX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;				//PA3复用为USART2_RX	
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/*USART初始化*/
	USART_InitTypeDef USART_InitStructure;											 //定义结构体变量
	USART_InitStructure.USART_BaudRate = 9600;		   								//波特率
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流控制,不需要
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;           	  //模式,发送模式和接收模式均选择
	USART_InitStructure.USART_Parity = USART_Parity_No;		     				 //奇偶校验,不需要
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	     				//停止位,选择1位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  			   //字长,选择8位
	USART_Init(USART2, &USART_InitStructure);				     			  //将结构体变量交给USART_Init,配置USART2

	/*中断输出配置*/
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);		   //开启串口接收数据的中断

	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;					       //定义结构体变量
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;		      //选择配置NVIC的USART2线
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		    	 //指定NVIC线路使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   //指定NVIC线路的抢占优先级为0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		   //指定NVIC线路的响应优先级为0
	NVIC_Init(&NVIC_InitStructure);							  //将结构体变量交给NVIC_Init,配置NVIC外设

	/*USART使能*/
	USART_Cmd(USART2, ENABLE);							   //使能USART2,串口开始运行
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
  * 函    数:串口发送一个字节
  * 参    数:Byte 要发送的一个字节
  * 返 回 值:无
  */
void Serial2_SendByte(u8 Byte)
{
	USART_SendData(USART2, Byte);		//将字节数据写入数据寄存器,写入后USART自动生成时序波形
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);	//等待发送完成
	/*下次写入数据寄存器会自动清除发送完成标志位,故此循环后,无需清除标志位*/
}

/**
  * 函    数:串口发送一个数组
  * 参    数:Array 要发送数组的首地址
  * 参    数:Length 要发送数组的长度
  * 返 回 值:无
  */
void Serial2_SendArray(u8* Array, u16 Length)
{
	u16 i;
	for (i = 0; i < Length; i++)		//遍历数组
	{
		Serial2_SendByte(Array[i]);	  //依次调用Serial_SendByte发送每个字节数据
	}
}

///**
//  * 函    数:串口发送一个字符串
//  * 参    数:String 要发送字符串的首地址
//  * 返 回 值:无
//  */
//void Serial2_SendString(char *String)
//{
//	u8 i;
//	for (i = 0; String[i] != '\0'; i ++)//遍历字符数组(字符串),遇到字符串结束标志位后停止
//	{
//		Serial2_SendByte(String[i]);	  //依次调用Serial_SendByte发送每个字节数据
//	}
//}

///**
//  * 函    数:次方函数(内部使用)
//  * 返 回 值:返回值等于X的Y次方
//  */
//uint32_t Serial2_Pow(uint32_t X, uint32_t Y)
//{
//	uint32_t Result = 1;	  //设置结果初值为1
//	while (Y --)			 //执行Y次
//	{
//		Result *= X;	   //将X累乘到结果
//	}
//	return Result;
//}


// #include "RF_Ev1527.h"

extern u8 Voice, VoiceAPP;

/**
  * 函    数:USART2中断函数
  * 参    数:无
  * 返 回 值:无
  */
void USART2_IRQHandler(void)
{
	static u8 RxState = 0;		 //定义表示当前状态机状态的静态变量

	Serial2_RxPacket[9] = 0x66;

	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //判断是否是USART2的接收事件触发的中断
	{

		u8 RxData = USART_ReceiveData(USART2);		   //读取数据寄存器,存放在接收的数据变量		
		/*依次处理数据包的不同部分*/

		/*当前状态为0,接收数据包包头或配对验证*/
		if (RxState == 0)
		{
			if (RxData == 0xAA) //如果数据确实是包头
			{
				Serial2_RxPacket[0] = RxData;	  //将数据存入数据包数组的指定位置				
				RxState = 1;					  //置下一个状态
			}
		}
		/*当前状态为1,接收数据包第一个数据*/
		else if (RxState == 1)
		{
			Serial2_RxPacket[1] = RxData;		 //将数据存入数据包数组的指定位置
			Serial2_RxFlag = 1;		             //接收数据包标志位置1,成功接收一个数据包

			RxState = 0;			             //状态归0
		}


		// Serial2_RxPacket[9] = 0x66;

		if (Serial2_RxFlag)//如果有数据
		{

			if (Serial2_RxPacket[1] == 0xFA)//唤醒
			{

			}
			else if (Serial2_RxPacket[1] == 0x30)	// 打开手电筒
			{
				Voice = 0;
				VoiceAPP = 1;
			}
			else if (Serial2_RxPacket[1] == 0x31)	// 微信收款码
			{
				Voice = 0;
				VoiceAPP = 2;
			}
			else if (Serial2_RxPacket[1] == 0x32)	// 计算器
			{
				Voice = 0;
				VoiceAPP = 3;
			}
			else if (Serial2_RxPacket[1] == 0x33)	// 电压测量
			{
				Voice = 0;

				VoiceAPP = 4;
			}
			else if (Serial2_RxPacket[1] == 0x34)	// 报时
			{
				Voice = 0;
				VoiceAPP = 5;
			}

			Serial2_RxFlag = 0;
		}

		USART_ClearITPendingBit(USART2, USART_IT_RXNE);		//清除标志位
	}


}


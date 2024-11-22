#include "stm32f10x.h"              
#include <stdio.h>
#include <stdarg.h>

#include "string.h"             //字符串判断函数库
#include "OLED.h"    


char Seria1_RxPacket[100];	//定义接收数据包数组,数据包格式"@XX"
u8 Seria1_RxFlag;	   //定义接收数据包标志位

/**
  * 函    数:串口初始化
  * 参    数:无
  * 返 回 值:无
  */
void Seria1_Init(void)
{
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//开启USART1的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //开启GPIOA的时钟

	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//将PA9引脚初始化为复用推挽输出

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);				//将PA10引脚初始化为上拉输入

	/*USART初始化*/
	USART_InitTypeDef USART_InitStructure;					//定义结构体变量
	USART_InitStructure.USART_BaudRate = 9600;		   //波特率
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流控制,不需要
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;           	  //模式,发送模式和接收模式均选择
	USART_InitStructure.USART_Parity = USART_Parity_No;		     //奇偶校验,不需要
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	    //停止位,选择1位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长,选择8位
	USART_Init(USART1, &USART_InitStructure);				  //将结构体变量交给USART_Init,配置USART1

	/*中断输出配置*/
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);		   //开启串口接收数据的中断


	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;					       //定义结构体变量
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		      //选择配置NVIC的USART1线
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		    	 //指定NVIC线路使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;   //指定NVIC线路的抢占优先级为1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		   //指定NVIC线路的响应优先级为1
	NVIC_Init(&NVIC_InitStructure);							  //将结构体变量交给NVIC_Init,配置NVIC外设

	/*USART使能*/
	USART_Cmd(USART1, ENABLE);							   //使能USART1,串口开始运行
}

/**
  * 函    数:串口发送一个字节
  * 参    数:Byte 要发送的一个字节
  * 返 回 值:无
  */
void Seria1_SendByte(u8 Byte)
{
	USART_SendData(USART1, Byte);		//将字节数据写入数据寄存器,写入后USART自动生成时序波形
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	//等待发送完成
	/*下次写入数据寄存器会自动清除发送完成标志位,故此循环后,无需清除标志位*/
}

/**
  * 函    数:串口发送一个数组
  * 参    数:Array 要发送数组的首地址
  * 参    数:Length 要发送数组的长度
  * 返 回 值:无
  */
void Seria1_SendArray(u8* Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i++)		//遍历数组
	{
		Seria1_SendByte(Array[i]);	  //依次调用Serial_SendByte发送每个字节数据
	}
}

/**
  * 函    数:串口发送一个字符串
  * 参    数:String 要发送字符串的首地址
  * 返 回 值:无
  */
void Seria1_SendString(char* String)
{
	u8 i;
	for (i = 0; String[i] != '\0'; i++)//遍历字符数组(字符串),遇到字符串结束标志位后停止
	{
		Seria1_SendByte(String[i]);	  //依次调用Serial_SendByte发送每个字节数据
	}
}

/**
  * 函    数:次方函数(内部使用)
  * 返 回 值:返回值等于X的Y次方
  */
u32 Seria1_Pow(u32 X, u32 Y)
{
	u32 Result = 1;	  //设置结果初值为1
	while (Y--)			 //执行Y次
	{
		Result *= X;	   //将X累乘到结果
	}
	return Result;
}





/**
  * 函    数:USART1中断函数
  * 参    数:无
  * 返 回 值:无
  */
void USART1_IRQHandler(void)
{
	static u8 RxState = 0;		 //定义表示当前状态机状态的静态变量
	static u8 pRxPacket = 0;	//定义表示当前接收数据位置的静态变量
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)	 //判断是否是USART1的接收事件触发的中断
	{
		u8 RxData = USART_ReceiveData(USART1);		   //读取数据寄存器,存放在接收的数据变量		
		/*使用状态机的思路,依次处理数据包的不同部分*/

		/*当前状态为0,接收数据包包头或配对验证*/
		if (RxState == 0)
		{
			if (RxData == 0xFF && Seria1_RxFlag == 0) //如果数据确实是包头,并且上一个数据包已处理完毕
			{
				RxState = 1;			//置下一个状态
				pRxPacket = 0;		   //数据包的位置归零
			}

		}
		/*当前状态为1,接收数据包第一个数据*/
		else if (RxState == 1)
		{
			Seria1_RxPacket[pRxPacket] = RxData;//将数据存入数据包数组的指定位置
			pRxPacket++;			           //数据包的位置自增
			RxState = 2;		              //置下一个状态

		}

		else if (RxState == 2)
		{
			Seria1_RxPacket[pRxPacket] = RxData;//将数据存入数据包数组的指定位置
			pRxPacket++;			           //数据包的位置自增
			RxState = 3;		              //置下一个状态

		}

		else if (RxState == 3)
		{
			Seria1_RxPacket[pRxPacket] = RxData;//将数据存入数据包数组的指定位置
			pRxPacket++;			           //数据包的位置自增
			RxState = 4;		              //置下一个状态

		}

		else if (RxState == 4)
		{
			Seria1_RxPacket[pRxPacket] = RxData;//将数据存入数据包数组的指定位置
			pRxPacket++;			           //数据包的位置自增
			RxState = 5;		              //置下一个状态

		}

		/*当前状态为2,接收数据包第二个数据*/
		else if (RxState == 5)
		{
			Seria1_RxPacket[pRxPacket] = RxData;	  //将数据存入数据包数组的指定位置
			pRxPacket++;			                 //数据包的位置自增
			Seria1_RxPacket[pRxPacket] = '\0';		//将收到的字符数据包添加一个字符串结束标志
			Seria1_RxFlag = 1;		               //接收数据包标志位置1,成功接收一个数据包
			RxState = 0;			              //状态归0
		}



		if (Seria1_RxFlag == 1)		          //如果接收到数据包
		{
			if (Seria1_RxPacket[1] == 0x3F)
			{
				if (Seria1_RxPacket[4] == 0x02)
				{

				}

			}
			else if (strcmp(Seria1_RxPacket, "ls") == 0)
			{
			}

			Seria1_RxFlag = 0;			//处理完成,接收数据包标志位清零
		}


		USART_ClearITPendingBit(USART1, USART_IT_RXNE);		//清除标志位
	}


}

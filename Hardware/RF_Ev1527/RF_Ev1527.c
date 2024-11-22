#include "stm32f10x.h"               
#include "Delay.h"  

void EV1527_IO_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOB, GPIO_Pin_3);
	GPIO_ResetBits(GPIOB, GPIO_Pin_4);
	GPIO_ResetBits(GPIOB, GPIO_Pin_5);
	GPIO_ResetBits(GPIOB, GPIO_Pin_6);	   //EV1527的4个端口

	GPIO_ResetBits(GPIOA, GPIO_Pin_10);    //EV1527的供电
}

//EV1527发送数据,通道1—15
void EV1527_Send(u8 passage)
{
	u8 temp;
	temp = passage;
	//判断通道是否在范围内
	if (temp < 1 || temp>15)return;

	//判断对应数据要发送的电平
	if ((temp & 0x01) == 0) GPIO_ResetBits(GPIOB, GPIO_Pin_3);
	else GPIO_SetBits(GPIOB, GPIO_Pin_3);
	temp = temp >> 1;//将数据右移

	if ((temp & 0x01) == 0) GPIO_ResetBits(GPIOB, GPIO_Pin_4);
	else GPIO_SetBits(GPIOB, GPIO_Pin_4);
	temp = temp >> 1;

	if ((temp & 0x01) == 0) GPIO_ResetBits(GPIOB, GPIO_Pin_5);
	else GPIO_SetBits(GPIOB, GPIO_Pin_5);
	temp = temp >> 1;

	if ((temp & 0x01) == 0) GPIO_ResetBits(GPIOB, GPIO_Pin_6);
	else GPIO_SetBits(GPIOB, GPIO_Pin_6);
	temp = temp >> 1;

	//给EV1527上电
	GPIO_SetBits(GPIOA, GPIO_Pin_10);

	delay_ms(500);//等待EV1527发送完毕

	GPIO_ResetBits(GPIOB, GPIO_Pin_3);
	GPIO_ResetBits(GPIOB, GPIO_Pin_4);
	GPIO_ResetBits(GPIOB, GPIO_Pin_5);
	GPIO_ResetBits(GPIOB, GPIO_Pin_6);

	GPIO_ResetBits(GPIOA, GPIO_Pin_10);
}


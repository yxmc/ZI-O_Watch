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
	GPIO_ResetBits(GPIOB, GPIO_Pin_6);	   //EV1527��4���˿�

	GPIO_ResetBits(GPIOA, GPIO_Pin_10);    //EV1527�Ĺ���
}

//EV1527��������,ͨ��1��15
void EV1527_Send(u8 passage)
{
	u8 temp;
	temp = passage;
	//�ж�ͨ���Ƿ��ڷ�Χ��
	if (temp < 1 || temp>15)return;

	//�ж϶�Ӧ����Ҫ���͵ĵ�ƽ
	if ((temp & 0x01) == 0) GPIO_ResetBits(GPIOB, GPIO_Pin_3);
	else GPIO_SetBits(GPIOB, GPIO_Pin_3);
	temp = temp >> 1;//����������

	if ((temp & 0x01) == 0) GPIO_ResetBits(GPIOB, GPIO_Pin_4);
	else GPIO_SetBits(GPIOB, GPIO_Pin_4);
	temp = temp >> 1;

	if ((temp & 0x01) == 0) GPIO_ResetBits(GPIOB, GPIO_Pin_5);
	else GPIO_SetBits(GPIOB, GPIO_Pin_5);
	temp = temp >> 1;

	if ((temp & 0x01) == 0) GPIO_ResetBits(GPIOB, GPIO_Pin_6);
	else GPIO_SetBits(GPIOB, GPIO_Pin_6);
	temp = temp >> 1;

	//��EV1527�ϵ�
	GPIO_SetBits(GPIOA, GPIO_Pin_10);

	delay_ms(500);//�ȴ�EV1527�������

	GPIO_ResetBits(GPIOB, GPIO_Pin_3);
	GPIO_ResetBits(GPIOB, GPIO_Pin_4);
	GPIO_ResetBits(GPIOB, GPIO_Pin_5);
	GPIO_ResetBits(GPIOB, GPIO_Pin_6);

	GPIO_ResetBits(GPIOA, GPIO_Pin_10);
}


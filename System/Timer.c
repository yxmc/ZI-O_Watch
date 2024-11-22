
#include "stm32f10x.h"  
#include "Timer.h"


/**
  * ��    ��:��ʱ�жϳ�ʼ��
  * ��    ��:��
  * �� �� ֵ:��
  */
void timer1_Init(void)
{
  // ʹ�ܶ�ʱ��1ʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

  // ���嶨ʱ����ʼ���ṹ��
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

  NVIC_InitTypeDef NVIC_InitStructure;

  // ���ö�ʱ��1�Ļ�������
  TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;        // ʱ�ӷ�Ƶ, ����Ƶ
  TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;    // ���ϼ���ģʽ
  TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;                    // �Զ���װ��ֵ,������100ʱ���
  TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;                // Ԥ��Ƶ��,72MHz / 7200 = 10kHz
  TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;               // �ظ�������,�������ڸ߼���ʱ��

  // ��ʼ����ʱ��1
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

  // �����ʱ��1�ĸ����жϱ�־λ
  TIM_ClearITPendingBit(TIM1, TIM_IT_Update);

  
  // ���ö�ʱ��1�����ж�
  TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);


  // �����ж���ԴΪTIM1�����ж�
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
  // ���������ȼ�Ϊ0
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  // ���������ȼ�Ϊ1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  // ʹ���ж�
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  // ��ʼ��NVIC
  NVIC_Init(&NVIC_InitStructure);

  // ʹ�ܶ�ʱ��1
  TIM_Cmd(TIM1, ENABLE);

}


// ��ʱ���жϺ���,���Ը��Ƶ�ʹ�����ĵط�
// void TIM1_IRQHandler(void)
// {
// 	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
// 	{


// 		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
// 	}
// }


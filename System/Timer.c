
#include "stm32f10x.h"  
#include "Timer.h"


/**
  * 函    数:定时中断初始化
  * 参    数:无
  * 返 回 值:无
  */
void timer1_Init(void)
{
  // 使能定时器1时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

  // 定义定时器初始化结构体
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

  NVIC_InitTypeDef NVIC_InitStructure;

  // 配置定时器1的基本参数
  TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;        // 时钟分频, 不分频
  TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;    // 向上计数模式
  TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;                    // 自动重装载值,计数到100时溢出
  TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;                // 预分频器,72MHz / 7200 = 10kHz
  TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;               // 重复计数器,仅适用于高级定时器

  // 初始化定时器1
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

  // 清除定时器1的更新中断标志位
  TIM_ClearITPendingBit(TIM1, TIM_IT_Update);

  
  // 配置定时器1更新中断
  TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);


  // 设置中断来源为TIM1更新中断
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
  // 设置主优先级为0
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  // 设置子优先级为1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  // 使能中断
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  // 初始化NVIC
  NVIC_Init(&NVIC_InitStructure);

  // 使能定时器1
  TIM_Cmd(TIM1, ENABLE);

}


// 定时器中断函数,可以复制到使用它的地方
// void TIM1_IRQHandler(void)
// {
// 	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
// 	{


// 		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
// 	}
// }


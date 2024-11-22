#include "stm32f10x.h" 
#include "menu.h"
#include "OLED.h"
#include "Delay.h"  
#include "Serial2.h"  
// #include "RF_Ev1527.h"
#include "CH7800play.h"
#include "Calculator.h"
#include "ASR_su_21t.h"

extern u8 POWER_OFF;
u8 Voicelogo = 0; 		//被语音识别唤醒标志位
u8 Voice = 0;
u8 VoiceAPP = 0;		//需要打开应用
u16 Delay_await = 0;	//等待时间


void ASR_dispose(void)
{
	Voice = 1;

	CH7800_Unicast_reply();

	OLED_Clear();

	while (Voice)   //等待处理完毕	会在串口2中断被处理
	{

		OLED_ShowASCII(0, 56, "...?", OLED_6X8);
		OLED_Update();

		if (Delay_await > 500 || POWER_OFF == 0)	//如果还没处理,或者被唤醒了,不等了
		{
			Delay_await = 0;
			Voicelogo = 0;
			Voice = 0;
			return;
		}
		Delay_await++;
		delay_ms(1);
	}

	CH7800_Unicast_return();


	if (VoiceAPP)
	{
		OLED_UpdateArea(0, 56, 24, 8);
		OLED_ShowASCII(0, 56, ".OK!", OLED_6X8);
		delay_ms(300);

		switch (VoiceAPP)
		{
		case 1: flashlight(); break; 			// 开灯
		case 2: WeChat(); break;				// 微信收款码
		case 3: Calculator(); break;			// 计算器
		case 4: volt(); break;					// 电压表
		case 5: CH7800_TelltheTime(); break;	// 报时
		default: break;
		}
		VoiceAPP = 0;

		OLED_Clear();
		OLED_Update();
	}

	Voicelogo = 0;
	Delay_await = 0;
}


















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
u8 Voicelogo = 0; 		//������ʶ���ѱ�־λ
u8 Voice = 0;
u8 VoiceAPP = 0;		//��Ҫ��Ӧ��
u16 Delay_await = 0;	//�ȴ�ʱ��


void ASR_dispose(void)
{
	Voice = 1;

	CH7800_Unicast_reply();

	OLED_Clear();

	while (Voice)   //�ȴ��������	���ڴ���2�жϱ�����
	{

		OLED_ShowASCII(0, 56, "...?", OLED_6X8);
		OLED_Update();

		if (Delay_await > 500 || POWER_OFF == 0)	//�����û����,���߱�������,������
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
		case 1: flashlight(); break; 			// ����
		case 2: WeChat(); break;				// ΢���տ���
		case 3: Calculator(); break;			// ������
		case 4: volt(); break;					// ��ѹ��
		case 5: CH7800_TelltheTime(); break;	// ��ʱ
		default: break;
		}
		VoiceAPP = 0;

		OLED_Clear();
		OLED_Update();
	}

	Voicelogo = 0;
	Delay_await = 0;
}


















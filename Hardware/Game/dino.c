#include "dino.h"
#include "stm32f10x.h"
#include "OLED.h"
#include "key.h"
#include "Delay.h"
#include <stdlib.h>
#include "menu.h"
#include "Timer.h"
#include "sys.h"

/* ����;�ٶ�;λ�� */
u32 num = 0;
u32 score = 0;
u8 speed = 5;
s8 cactus_speed = 3;
s8 cloud_flag = 2;
s8 pos = 0;
s8 cactus_pos = -128;
s8 cloud_pos = -128;
s8 dino_jump_pos = 42;
s8 crow_pos = 0;
s8 high_crow_speed = 5;
s8 high_crow_pos = -128; // ��λ��

/* ��Ծ�仯 */
s8 jump_speed[11] = { 10,16,20,23,25,26,20,13,4,0 };

/* flag�����жϸ����ٽ�������״̬ */
int y_pos = 0;
int y_pos1;
int y_pos2;
int dino_jump_flag = 1; // ��Ծ��־
// int dino_jump_flag_flag = 0; // ��Ծ��־
int dino_jump_finish_flag = 0; // ��Ծ������־
int run_flag = 1; // ���б�־
// int key_flag = 0; // ������־
u8 over_flag = 0; // ��Ϸ������־
int barrier_flag = 0; // �ϰ�����ֱ�־
u8 barrier_flag_num = 0; // �ϰ�����ִ���
int crow_fly_flag = 1;	// �񳲷��б�־
int oled_reverse_flag = 1; // OLED��ת��־
int power_flag = 0; // �������ٱ�־
int invincibl_flag = 0; // �޵б�־
int power_timer_flag = 32; // �������ٶ�ʱ����־
int barrier2_end_flag = 0; // �ϰ���2������־

/* ������� */
int KeyNum3 = 0;

/* �������� */
u16 ShowDinoJump(void);
void ShowScore(void);
void ShowGameCore(void);
void ShowGroud(void);
void ShowCloud(void);
void ShowDinoRun(void);
void ShowBigDinoRun(void);
void ShowDinoDownRun(void);
void ShowGameAnimate(void);
// u8 KeyStatus(void);
void ShowCrowFly(void);


// ����EXTI��0�ж�
extern void EXTI0_Disable(void);

// ����ʹ��EXTI��0�ж�
extern void EXTI0_Enable(void);

u8 game_exit_flag = 0;

u8 F_Key_GetNum(void)
{

	// static u8 flag = 0;
	if (PCin(13) == 1) return  1;
	if (PAin(0) == 1) return  2;

	if (menu_Enter_event() == 2)
	{
		game_exit_flag = 1;
	}

	return 0;
}


/* ��ʾ���ֶ��� */
void ShowGameAnimate(void)
{
	EXTI0_Disable();

	timer1_Init();
	while (1)
	{
		OLED_Clear();
		if (num > 100) score++;
		ShowGroud();
		ShowCloud();
		ShowGameCore();	//������ĺ���
		ShowScore();
		OLED_Update();
		OLED_transition(1);
		if (game_exit_flag)
		{
			game_exit_flag = 0;
			break;
		}

		delay_ms(2);
		OLED_WriteCommand(0x40);	//������ʾ��ʼ��,0x40~0x7F
		OLED_WriteCommand(0xA1);	//�������ҷ���,0xA1����,0xA0���ҷ���
		OLED_WriteCommand(0xC8);	//�������·���,0xC8����,0xC0���·���
		delay_ms(50 - 3);


	}
	TIM_Cmd(TIM1, DISABLE);
}

void ShowScore(void)
{
	OLED_ShowNum(1, 2, score, 5, OLED_6X8);				//��ʾ����,��ʱ��仯(�ж�������)
	OLED_DrawRectangle(0, 0, 32, 10, OLED_UNFILLED);	//��ʾ���η���
	/* black -> white */
	if (oled_reverse_flag == -1)OLED_Reverse();			//��ɫ��ת	
	if (score % 100 == 0 && score > 50)oled_reverse_flag = -oled_reverse_flag;
}

void ShowGroud(void)
{
	OLED_ShowNum(1, 2, score, 5, OLED_6X8);			//�ظ���ʾ�����Ա�֤gameover��Ȼ��ʾ
	OLED_DrawRectangle(0, 0, 32, 10, OLED_UNFILLED);
	OLED_ShowGround(0, 56, 128, 8, GroundImg, pos, speed);	//��ʾ����,�䶯ground��������	
	pos = pos + speed;
}
void ShowCloud(void)
{
	if (cloud_pos <= 0)cloud_pos = -128;
	OLED_ShowCloud(cloud_pos, 8, 26, 8, CloudImg);
	if (cloud_pos <= 36)OLED_ClearArea(0, 0, 6, 8);	//������������ڸǷ�����ʾ 
}

void ShowDinoRun(void)
{
	if (run_flag == 1)OLED_ShowDinoRun(16, 42, 15, 16, DinoFoot2);
	if (run_flag == -2)OLED_ShowDinoRun(16, 42, 15, 16, DinoFoot1);
	run_flag = ~run_flag;
}

void ShowBigDinoRun(void)
{
	if (run_flag == 1)OLED_ShowDinoRun(16, 26, 29, 32, BigDinoFoot2);
	if (run_flag == -2)OLED_ShowDinoRun(16, 26, 29, 32, BigDinoFoot1);
	run_flag = ~run_flag;
}

void ShowDinoDownRun(void)
{
	OLED_ClearArea(16, 42, 15, 16);				//������������ڸ�BigDino	
	if (run_flag == 1)OLED_ShowDinoRun(16, 48, 22, 10, DinoDown3);
	if (run_flag == -2)OLED_ShowDinoRun(16, 48, 22, 10, DinoDown4);
	//	run_flag = ~run_flag;	
}

/* ����jump_speed[]�Ա䶯Y������ */
u16 ShowDinoJump(void)
{
	dino_jump_pos = 42 - jump_speed[dino_jump_flag];
	switch (dino_jump_flag)
	{
	case 1: OLED_ShowDinoJump(16, 42 - jump_speed[0], 14, 16, DinoJump);break;
	case 2:	OLED_ShowDinoJump(16, 42 - jump_speed[1], 14, 16, DinoJump);break;
	case 3: OLED_ShowDinoJump(16, 42 - jump_speed[2], 14, 16, DinoJump);break;
	case 4: OLED_ShowDinoJump(16, 42 - jump_speed[3], 14, 16, DinoJump);break;
	case 5: OLED_ShowDinoJump(16, 42 - jump_speed[4], 14, 16, DinoJump);break;
	case 6: OLED_ShowDinoJump(16, 42 - jump_speed[5], 14, 16, DinoJump);break;
	case 7: OLED_ShowDinoJump(16, 42 - jump_speed[6], 14, 16, DinoJump);break;
	case 8: OLED_ShowDinoJump(16, 42 - jump_speed[7], 14, 16, DinoJump);break;
	case 9: OLED_ShowDinoJump(16, 42 - jump_speed[8], 14, 16, DinoJump);break;
	case 10: OLED_ShowDinoJump(16, 42 - jump_speed[9], 14, 16, DinoJump);break;
	case 11: OLED_ShowDinoJump(16, 42 - jump_speed[10], 14, 16, DinoJump);dino_jump_flag = 0;break;
	}
	dino_jump_flag++;
	return dino_jump_pos;
}

/* ���ĺ���:����ϰ���ʾ;����״̬�䶯����״̬(����,��Ծ,����,���);��ײ���;Power״̬ */
void ShowGameCore(void)
{
	/* power */
	if (score % 100 == 0 && score > 50) power_flag = 1;		//����power_flag

	cactus_pos = 128 - pos;		//�ϰ�λ��
	crow_pos = 128 - pos;
	//crow flag
	crow_fly_flag = ~crow_fly_flag;  //��ѻ����flag
	//key detect
	if (F_Key_GetNum() == 2)
	{
		dino_jump_finish_flag = 1;	//������Ծ״̬
	}
	/* run status */
	if (dino_jump_finish_flag == 0)	//���뱼��״̬
	{
		barrier_flag_num = pos;
		if (barrier_flag_num > 128)	//�����ʾ�ϰ�
		{
			barrier_flag_num = 0;
			pos = 0;
			barrier_flag = rand() % 4;
		}

		if (barrier_flag == 2)barrier2_end_flag = 1;		//�߿���ѻ���н���flag
		else barrier2_end_flag = 0;

		switch (barrier_flag)		//��ʾ�ϰ�
		{
		case 0: OLED_ShowCactus(cactus_pos, 42, 8, 17, Cactus1);
			break;
		case 1: OLED_ShowCactus(cactus_pos, 42, 15, 16, Cactus3);
			break;
		case 2:
			high_crow_pos = 128 - pos;
			if (crow_fly_flag == 1)OLED_ShowCrowFly(high_crow_pos, 29, 22, 16, CrowFly1);
			if (crow_fly_flag == -2)OLED_ShowCrowFly(high_crow_pos, 29, 22, 16, CrowFly2);
			break;
		case 3:
			if (crow_fly_flag == 1)OLED_ShowCrowFly(crow_pos, 42, 22, 16, CrowFly1);
			if (crow_fly_flag == -2)OLED_ShowCrowFly(crow_pos, 42, 22, 16, CrowFly2);
			break;
		}
		ShowDinoRun();				//����
	}

	/* down run status */
	if (F_Key_GetNum() == 1 && dino_jump_finish_flag != 1)		//������ſ����״̬
	{
		OLED_ClearArea(16, 42, 15, 16);							//�������״̬Dino
		barrier_flag_num = pos;
		if (barrier_flag_num > 128)
		{
			barrier_flag_num = 0;
			barrier_flag = rand() % 4;
		}
		switch (barrier_flag)
		{
		case 0: OLED_ShowCactus(cactus_pos, 42, 8, 17, Cactus1);
			break;
		case 1: OLED_ShowCactus(cactus_pos, 42, 15, 16, Cactus3);
			break;
		case 2:
			high_crow_pos = 128 - pos;
			if (crow_fly_flag == 1)OLED_ShowCrowFly(high_crow_pos, 29, 22, 16, CrowFly1);
			if (crow_fly_flag == -2)OLED_ShowCrowFly(high_crow_pos, 29, 22, 16, CrowFly2);
			break;
		case 3:
			if (crow_fly_flag == 1)OLED_ShowCrowFly(crow_pos, 42, 22, 16, CrowFly1);
			if (crow_fly_flag == -2)OLED_ShowCrowFly(crow_pos, 42, 22, 16, CrowFly2);
			break;
		}
		ShowDinoDownRun();			//��ſ����
	}

	/* game over �ж��ٽ�״̬ ��Ϸ���� */
	if (barrier_flag != 2 && dino_jump_finish_flag != 1 && barrier2_end_flag != 1 && invincibl_flag == 0) //�Ǹ߿��ϰ�
	{
		if ((cactus_pos >= 8 && cactus_pos <= 16) || (crow_pos >= 8 && crow_pos <= 16))
		{
			while (1)
			{
				switch (over_flag)
				{
				case 1: OLED_ShowGameOver(46, 16, 36, 32, GameOver1, over_flag);break;
				case 2: OLED_ShowGameOver(46, 16, 36, 32, GameOver2, over_flag);break;
				case 3: OLED_ShowGameOver(46, 16, 36, 32, GameOver3, over_flag);break;
				case 4: OLED_ShowGameOver(46, 16, 36, 32, GameOver4, over_flag);break;
				case 5: OLED_ShowGameOver(46, 16, 36, 32, GameOver5, over_flag);break;
				case 6: OLED_ShowGameOver(46, 16, 36, 32, GameOver6, over_flag);break;
				}
				over_flag++;
				delay_ms(25);
				if (F_Key_GetNum()) { score = 0;break; }
				OLED_Update();

			}
		}
	}

	if (barrier_flag == 2 && dino_jump_finish_flag != 1 && invincibl_flag == 0)	//�߿��ϰ�
	{
		if ((high_crow_pos >= 8 && high_crow_pos <= 16) && F_Key_GetNum() != 1)
		{
			while (1)
			{
				switch (over_flag)
				{
				case 1: OLED_ShowGameOver(46, 16, 36, 32, GameOver1, over_flag);break;
				case 2: OLED_ShowGameOver(46, 16, 36, 32, GameOver2, over_flag);break;
				case 3: OLED_ShowGameOver(46, 16, 36, 32, GameOver3, over_flag);break;
				case 4: OLED_ShowGameOver(46, 16, 36, 32, GameOver4, over_flag);break;
				case 5: OLED_ShowGameOver(46, 16, 36, 32, GameOver5, over_flag);break;
				case 6: OLED_ShowGameOver(46, 16, 36, 32, GameOver6, over_flag);break;
				}
				over_flag++;
				delay_ms(25);
				if (F_Key_GetNum()) { score = 0;break; }
				OLED_Update();

			}
		}
	}

	if (dino_jump_finish_flag == 1)		//������Ծ״̬
	{
		// ��Ծ��Ϸ����
		if (dino_jump_pos >= 32 && (cactus_pos < 30 && cactus_pos > 10) && invincibl_flag == 0)
		{
			while (1 && dino_jump_finish_flag == 1)
			{
				OLED_ShowNum(1, 2, score, 5, OLED_6X8);
				OLED_DrawRectangle(0, 0, 32, 10, OLED_UNFILLED);
				switch (over_flag)
				{
				case 1: OLED_ShowGameOver(46, 16, 36, 32, GameOver1, over_flag);break;
				case 2: OLED_ShowGameOver(46, 16, 36, 32, GameOver2, over_flag);break;
				case 3: OLED_ShowGameOver(46, 16, 36, 32, GameOver3, over_flag);break;
				case 4: OLED_ShowGameOver(46, 16, 36, 32, GameOver4, over_flag);break;
				case 5: OLED_ShowGameOver(46, 16, 36, 32, GameOver5, over_flag);break;
				case 6: OLED_ShowGameOver(46, 16, 36, 32, GameOver6, over_flag);
				}
				over_flag++;
				delay_ms(25);
				if (F_Key_GetNum()) { score = 0;break; }
				OLED_Update();

			}
		}
		dino_jump_pos = 42 - jump_speed[dino_jump_flag];
		barrier_flag_num = pos;
		if (barrier_flag_num > 144)
		{
			pos = 0;
			barrier_flag_num = 0;
			barrier_flag = rand() % 4;
		}
		/* rand show barrier */
		if (barrier_flag == 0)
		{
			OLED_ShowCactus(cactus_pos, 42, 8, 17, Cactus1);
		}
		if (barrier_flag == 1)OLED_ShowCactus(cactus_pos, 42, 15, 16, Cactus3);
		if (barrier_flag == 2)
		{
			high_crow_pos = 128 - pos;
			if (crow_fly_flag == 1)OLED_ShowCrowFly(high_crow_pos, 29, 22, 16, CrowFly1);
			if (crow_fly_flag == -2)OLED_ShowCrowFly(high_crow_pos, 29, 22, 16, CrowFly2);
		}
		if (barrier_flag == 3)
		{
			if (crow_fly_flag == 1)OLED_ShowCrowFly(crow_pos, 42, 22, 16, CrowFly1);
			if (crow_fly_flag == -2)OLED_ShowCrowFly(crow_pos, 42, 22, 16, CrowFly2);
		}

		switch (dino_jump_flag)	//��ʾ��Ծ����
		{
		case 1: OLED_ShowDinoJump(16, 42 - jump_speed[0], 14, 16, DinoJump);break;
		case 2:	OLED_ShowDinoJump(16, 42 - jump_speed[1], 14, 16, DinoJump);break;
		case 3: OLED_ShowDinoJump(16, 42 - jump_speed[2], 14, 16, DinoJump);break;
		case 4: OLED_ShowDinoJump(16, 42 - jump_speed[3], 14, 16, DinoJump);break;
		case 5: OLED_ShowDinoJump(16, 42 - jump_speed[4], 14, 16, DinoJump);break;
		case 6: OLED_ShowDinoJump(16, 42 - jump_speed[5], 14, 16, DinoJump);break;
		case 7: OLED_ShowDinoJump(16, 42 - jump_speed[6], 14, 16, DinoJump);break;
		case 8: OLED_ShowDinoJump(16, 42 - jump_speed[7], 14, 16, DinoJump);break;
		case 9: OLED_ShowDinoJump(16, 42 - jump_speed[8], 14, 16, DinoJump);break;
		case 10: OLED_ShowDinoJump(16, 42 - jump_speed[9], 14, 16, DinoJump);break;
		case 11: OLED_ShowDinoJump(16, 42 - jump_speed[10], 14, 16, DinoJump);dino_jump_flag = 0;dino_jump_finish_flag = 0;break;
		}
		dino_jump_flag++;
		if (barrier_flag != 2 && ((42 - jump_speed[dino_jump_flag]) >= 40) && invincibl_flag == 0)
		{
			if ((crow_pos >= 8 && crow_pos <= 16))
			{
				while (1)
				{
					switch (over_flag)
					{
					case 1: OLED_ShowGameOver(46, 16, 36, 32, GameOver1, over_flag);break;
					case 2: OLED_ShowGameOver(46, 16, 36, 32, GameOver2, over_flag);break;
					case 3: OLED_ShowGameOver(46, 16, 36, 32, GameOver3, over_flag);break;
					case 4: OLED_ShowGameOver(46, 16, 36, 32, GameOver4, over_flag);break;
					case 5: OLED_ShowGameOver(46, 16, 36, 32, GameOver5, over_flag);break;
					case 6: OLED_ShowGameOver(46, 16, 36, 32, GameOver6, over_flag);break;
					}
					over_flag++;
					delay_ms(25);
					if (F_Key_GetNum()) { score = 0;break; }
					OLED_Update();

				}
			}
		}

		if (barrier_flag == 2 && dino_jump_finish_flag != 1 && invincibl_flag == 0)
		{
			if ((high_crow_pos >= 8 && high_crow_pos <= 16) && F_Key_GetNum() != 1)
			{
				while (1)
				{
					switch (over_flag)
					{
					case 1: OLED_ShowGameOver(46, 16, 36, 32, GameOver1, over_flag);break;
					case 2: OLED_ShowGameOver(46, 16, 36, 32, GameOver2, over_flag);break;
					case 3: OLED_ShowGameOver(46, 16, 36, 32, GameOver3, over_flag);break;
					case 4: OLED_ShowGameOver(46, 16, 36, 32, GameOver4, over_flag);break;
					case 5: OLED_ShowGameOver(46, 16, 36, 32, GameOver5, over_flag);break;
					case 6: OLED_ShowGameOver(46, 16, 36, 32, GameOver6, over_flag);break;
					}
					over_flag++;
					delay_ms(25);
					if (F_Key_GetNum()) { score = 0;break; }
					OLED_Update();
				}
			}
		}
	}

	/********************* big dino *********************/
	if (power_flag == 1 && F_Key_GetNum() == 3)		//���״̬���
	{
		dino_jump_finish_flag = 3;
		invincibl_flag = 1;		//���п�ʼ��־
		power_timer_flag--;

		barrier_flag_num = pos;
		if (barrier_flag_num > 128)
		{
			barrier_flag_num = 0;
			barrier_flag = rand() % 4;
		}
		switch (barrier_flag)
		{
		case 0: OLED_ShowCactus(cactus_pos, 42, 8, 17, Cactus1);
			break;
		case 1: OLED_ShowCactus(cactus_pos, 42, 15, 16, Cactus3);
			break;
		case 2:
			high_crow_pos = 128 - pos;
			if (crow_fly_flag == 1)OLED_ShowCrowFly(high_crow_pos, 29, 22, 16, CrowFly1);
			if (crow_fly_flag == -2)OLED_ShowCrowFly(high_crow_pos, 29, 22, 16, CrowFly2);
			break;
		case 3:
			if (crow_fly_flag == 1)OLED_ShowCrowFly(crow_pos, 42, 22, 16, CrowFly1);
			if (crow_fly_flag == -2)OLED_ShowCrowFly(crow_pos, 42, 22, 16, CrowFly2);
			break;
		}
		if (power_timer_flag == 0)
		{
			power_timer_flag = 32;
			power_flag = 0;
			KeyNum3 = 0;
			invincibl_flag = 0;			//���н�����־
			dino_jump_finish_flag = 0;	//���뱼��״̬
		}
		if (cactus_pos >= 16 && cactus_pos <= 35 && invincibl_flag != 0)OLED_ClearArea(16, 26, 29, 32); 	//�ٽ�״̬���
		ShowBigDinoRun();			//���������
	}
	pos = pos + cactus_speed;		//λ��=λ��+�ٶ�
}

void ShowCrowFly(void)	//��ѻ����
{
	crow_pos = 128 - pos;

	OLED_ShowCrowFly(crow_pos, 36, 22, 16, CrowFly1);

	pos = pos + cactus_speed;
}

/*  IRQ service program */
void TIM1_UP_IRQHandler(void)	//�жϳ���
{
	/* enter IRQ every 10ms */
	if (TIM_GetFlagStatus(TIM1, TIM_IT_Update) == SET)
	{
		cloud_flag--;					//�Ʋʱ�־λ�Լ��Ի����ƶ�
		if (cloud_flag == 0)
		{
			cloud_pos--;
			cloud_flag = 2;
		}
		num++;
		u8 Dino_Count;
		Dino_Count++;
		if (Dino_Count == 50)			//�����㹻ʱ���������Ծ
		{
			if (dino_jump_finish_flag == 1)
			{
				dino_jump_flag++;
				if (dino_jump_flag == 11)dino_jump_finish_flag = 0;
			}
		}
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}
}


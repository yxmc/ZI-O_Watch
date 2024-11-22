#ifndef __CH7800PLAY_H
#define __CH7800PLAY_H


#include "stm32f10x.h"               
#include "Delay.h"  
#include "Serial3.h" 


#define Play_audio 0x13     //ָ����Ƶ����1-100(100֮����Ԥ����Ƶ)
#define Loudness 0x16       //ָ������0��30



extern u8 CHvolume;

void CH7800_Init(void);

//CH7800����ָ��
void CH7800_Unicast(u8 CMD, u8 DATA);

//��������ѭ������,1--3,3��
void CH7800_Unicast_reply(void);

//�ظ�����ѭ������,4--6,3��
void CH7800_Unicast_return(void);

//��ʱ
void CH7800_TelltheTime(void);

#endif


#ifndef __CH7800PLAY_H
#define __CH7800PLAY_H


#include "stm32f10x.h"               
#include "Delay.h"  
#include "Serial3.h" 


#define Play_audio 0x13     //指定音频播放1-100(100之后是预留音频)
#define Loudness 0x16       //指定音量0—30



extern u8 CHvolume;

void CH7800_Init(void);

//CH7800发送指令
void CH7800_Unicast(u8 CMD, u8 DATA);

//唤醒语音循环播放,1--3,3段
void CH7800_Unicast_reply(void);

//回复语音循环播放,4--6,3段
void CH7800_Unicast_return(void);

//报时
void CH7800_TelltheTime(void);

#endif


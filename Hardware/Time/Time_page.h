#ifndef __TIME_PAGE_H
#define __TIME_PAGE_H

#include "stm32f10x.h"
#include <string.h>           
#include "OLED.h"     
#include "MyRTC.h"
#include "menu.h"
#include "AD.h"
#include "StepCount.h"




extern u8 Time_12_flag;
extern u8 Time_24_flag;


void Time_page(void);
void Time_page_io_init(void);
void Time_page_Timer(u8 x, u8 y);
void Time_page_small_Timer(u8 x, u8 y);
void Time_page_date(u8 x, u8 y);
void Time_page_week(u8 x, u8 y);
void Time_page_BAT(u8 x, u8 y);
void Time_Adjust(void);
void Time_page_figures(u8 x, u8 y, u8 num, u8 size);

// void Time_page_External_sensors(u8 x, u8 y);


#endif

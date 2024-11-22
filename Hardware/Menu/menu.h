#ifndef __MENU_H
#define __MENU_H
#include "stm32f10x.h"                  // Device header
#include <string.h>
#include "OLED.h"


#define		MEHE 		16				//菜单高 Menu height
#define		WORD_H 		6				//字高word height

#define     RETURN      0               //菜单返回

#define		Display 	0				//菜单模式宏定义
#define		Function    1 
#define		ON_OFF      2
#define		Number		3

//Display    仅展示 		  此状态下被执行时,不作任何动作,函数位不填,变量地址位不填
//Function   可进入函数:    此状态下被执行时, 进入一次被选中函数,变量地址位不填
//ON_OFF     开关:         此状态下被执行时, 将传入的地址的被选中变量取反一次,再进入被选中函数 
//Number     变量:		此状态下被执行时, 将进入在菜单内部的 变量调节器,每调节一次就进入被选中函数一次

/**********************************************************/


struct option_class1        //菜单类型1
{
	char* Name;				//选项名字

	void (*func)(void);		//函数指针

	const u8* Image;   	//需要传入的图像32*32

	u8 NameLen;			//由于中文占三个字节,用strlen计算名字宽度不再准确,故需额外储存名字宽度
};

struct option_class2        //菜单类型2
{
	char* Name;				//选项名字

	u8 mode;           		//mode=0,只显示,=1是函数,=2是开关,=3是变量

	void (*func)(void);		//函数指针

	u8* Num;   		//需要传入的变量地址

	u8 NameLen;		//由于中文占三个字节,用strlen计算名字宽度不再准确,故需额外储存名字宽度
};

/**********************************************************/
int8_t menu_Roll_event(void);
int8_t menu_Enter_event(void);

void run_menu1(struct option_class1* option);
void run_menu2(struct option_class2* option);

u8 Get_NameLen(char* String);

/**********************************************************/

void main_menu1(void);
void main_menu2(void);
void OLED_ReverseArea2(u8 X, u8 Y, u8 Width, u8 Height);

void volt(void);
void YAOKON315M(void);
void DID(void);
void WeChat(void);
void Alipay(void);
void Game_Of_Life_Play(void);
void System_Settings(void);
void flashlight(void);
void Stop_Watch(void);
void KAIJIDONGHUA(void);
void error(void);
void System_Brightness(void);
/**********************************************************/

#endif

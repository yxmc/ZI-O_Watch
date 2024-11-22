#ifndef __MENU_H
#define __MENU_H
#include "stm32f10x.h"                  // Device header
#include <string.h>
#include "OLED.h"


#define		MEHE 		16				//�˵��� Menu height
#define		WORD_H 		6				//�ָ�word height

#define     RETURN      0               //�˵�����

#define		Display 	0				//�˵�ģʽ�궨��
#define		Function    1 
#define		ON_OFF      2
#define		Number		3

//Display    ��չʾ 		  ��״̬�±�ִ��ʱ,�����κζ���,����λ����,������ַλ����
//Function   �ɽ��뺯��:    ��״̬�±�ִ��ʱ, ����һ�α�ѡ�к���,������ַλ����
//ON_OFF     ����:         ��״̬�±�ִ��ʱ, ������ĵ�ַ�ı�ѡ�б���ȡ��һ��,�ٽ��뱻ѡ�к��� 
//Number     ����:		��״̬�±�ִ��ʱ, �������ڲ˵��ڲ��� ����������,ÿ����һ�ξͽ��뱻ѡ�к���һ��

/**********************************************************/


struct option_class1        //�˵�����1
{
	char* Name;				//ѡ������

	void (*func)(void);		//����ָ��

	const u8* Image;   	//��Ҫ�����ͼ��32*32

	u8 NameLen;			//��������ռ�����ֽ�,��strlen�������ֿ�Ȳ���׼ȷ,������ⴢ�����ֿ��
};

struct option_class2        //�˵�����2
{
	char* Name;				//ѡ������

	u8 mode;           		//mode=0,ֻ��ʾ,=1�Ǻ���,=2�ǿ���,=3�Ǳ���

	void (*func)(void);		//����ָ��

	u8* Num;   		//��Ҫ����ı�����ַ

	u8 NameLen;		//��������ռ�����ֽ�,��strlen�������ֿ�Ȳ���׼ȷ,������ⴢ�����ֿ��
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

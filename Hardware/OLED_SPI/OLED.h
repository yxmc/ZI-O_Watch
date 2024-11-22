
#ifndef __OLED_H
#define __OLED_H

#include <stdint.h>
#include "OLED_Data.h"

/*�����궨��*********************/

/*FontSize����ȡֵ*/
/*�˲���ֵ���������ж�,�������ڼ�������ַ�ƫ��,Ĭ��ֵΪ�������ؿ��*/
#define OLED_8X16				8
#define OLED_6X8				6

/*IsFilled������ֵ*/
#define OLED_UNFILLED			0
#define OLED_FILLED				1

/*********************�����궨��*/


/*��������*********************/

/*��ʼ������*/
void OLED_Init(void);

/*���º���*/
void OLED_Update(void);
void OLED_UpdateArea(u8 X, u8 Y, u8 Width, u8 Height);

/*�Դ���ƺ���*/
void OLED_Clear(void);
void OLED_ClearArea(u8 X, u8 Y, u8 Width, u8 Height);
void OLED_Reverse(void);
void OLED_ReverseArea(u8 X, u8 Y, u8 Width, u8 Height);

/*��ʾ����*/
void OLED_ShowChar(int8_t X, int8_t Y, char Char, u8 FontSize);
void OLED_ShowASCII(int8_t X, int8_t Y, char* String, u8 FontSize);
void OLED_ShowString(int8_t X, int8_t Y, char* String, u8 FontSize);
void OLED_String_W25Q(int8_t X, int8_t Y, char* String);
void OLED_ShowNum(int8_t X, int8_t Y, u32 Number, u8 Length, u8 FontSize);
void OLED_ShowSignedNum(u8 X, u8 Y, int32_t Number, u8 Length, u8 FontSize);
void OLED_ShowHexNum(u8 X, u8 Y, u32 Number, u8 Length, u8 FontSize);
void OLED_ShowBinNum(u8 X, u8 Y, u32 Number, u8 Length, u8 FontSize);
void OLED_ShowFloatNum(u8 X, u8 Y, double Number, u8 IntLength, u8 FraLength, u8 FontSize);
void OLED_ShowChinese(u8 X, u8 Y, char* Chinese);
void OLED_ShowImage(int8_t X, int8_t Y, u8 Width, u8 Height, const u8* Image);
void OLED_Printf(u8 X, u8 Y, u8 FontSize, char* format, ...);

/*��ͼ����*/
void OLED_DrawPoint(int8_t X, int8_t Y);
u8 OLED_GetPoint(u8 X, u8 Y);
void OLED_DrawLine(u8 X0, u8 Y0, u8 X1, u8 Y1);
void OLED_DrawRectangle(int8_t X, int8_t Y, u8 Width, u8 Height, u8 IsFilled);
void OLED_DrawTriangle(u8 X0, u8 Y0, u8 X1, u8 Y1, u8 X2, u8 Y2, u8 IsFilled);
void OLED_DrawCircle(u8 X, u8 Y, u8 Radius, u8 IsFilled);
void OLED_DrawEllipse(u8 X, u8 Y, u8 A, u8 B, u8 IsFilled);
void OLED_DrawArc(u8 X, u8 Y, u8 Radius, int16_t StartAngle, int16_t EndAngle, u8 IsFilled);


//oledд����
void OLED_WriteCommand(u8 Command);

void OLED_transition(u8 o);


//���ߺ���
void OLED_Power(u8 Data);
u32 OLED_Pow(u32 X, u32 Y);


//
extern u8 liangdu_UP, GUODUspeed, liangdu;





// dino
void OLED_ShowGround(u8 X, u8 Y, u8 Width, u8 Height, const u8* Image, u8 pos, u8 speed);
void OLED_ShowCloud(int8_t X, u8 Y, u8 Width, u8 Height, const u8* Image);
void OLED_ShowDinoRun(u8 X, u8 Y, u8 Width, u8 Height, const u8* Image);
void OLED_ShowDinoJump(u8 X, u8 Y, u8 Width, u8 Height, const u8* Image);
void OLED_ShowCactus(u8 X, u8 Y, u8 Width, u8 Height, const u8* Image);
void OLED_ShowGameOver(u8 X, u8 Y, u8 Width, u8 Height, const u8* Image, u8 over_flag);
void OLED_ShowCrowFly(u8 X, u8 Y, u8 Width, u8 Height, const u8* Image);


void OLED_ShowCurrentMemory(void);



/*********************��������*/

#endif


/*****************��Э�Ƽ�||��Ȩ����****************/
/*****************jiangxiekeji.com*****************/

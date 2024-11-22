#include "stm32f10x.h"
#include "menu.h" 
#include "Delay.h"  
#include "OLED.h"  
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <math.h>


//进行计算
double add(double a, double b) { return a + b; }

double subtract(double a, double b) { return a - b; }

double multiply(double a, double b) { return a * b; }

double divide(double a, double b)
{
	if (b != 0) {
		return a / b;
	}
	else {
		return 0;
	}
}

#define MAX_INPUT_LENGTH 20  

void Change(char* a, char* b, u8 k)
{
	char t;      //两个数组交换
	for (u8 i = 0; i < k; i++)
	{
		t = a[i];
		a[i] = b[i];
		b[i] = t;
	}
}

void Calculator(void)
{
	//懒得动了,直接当矢山好了,能完成功能就行
	s8 p;
	OLED_Clear();
	OLED_Update();

	double PI = 3.141592653589793, E = 2.718281828459;

	double num1 = 0, num2 = 0;

	char input[MAX_INPUT_LENGTH], input2[MAX_INPUT_LENGTH];
	int index = 0, index2 = 0;

	char key;	          //输入值
	u8 vkey = 0;
	u8 KEYstate = 0;  // 1 表示有输入事件

	u8 Computing = 0;//是否需要计算
	u8 uting = 1;//是否需要计算

	while (1)
	{
		OLED_Clear();
		OLED_ShowFloatNum(0, 3, num1, 10, 9, OLED_6X8);
		OLED_ShowFloatNum(0, 14, num2, 10, 9, OLED_6X8);


		if (uting == 1) OLED_DrawLine(0, 12, 127, 12);
		else  OLED_DrawLine(0, 22, 127, 22);
		if (vkey < 10)
		{
			OLED_ShowNum(8, 48, vkey, 2, OLED_8X16); OLED_ShowASCII(0, 48, "+", OLED_8X16);
		}
		else
		{
			switch (vkey)
			{
			case 10: OLED_ShowASCII(0, 48, ".", OLED_8X16); break;
			case 11: OLED_ShowASCII(0, 48, "+PI", OLED_8X16); break;
			case 12: OLED_ShowASCII(0, 48, "+E", OLED_8X16); break;
			case 13: OLED_ShowString(0, 52, "切换", OLED_8X16); break;
			case 14: OLED_ShowString(0, 52, "清除", OLED_8X16); break;
			case 15: OLED_ShowString(0, 52, "交换", OLED_8X16); break;
			case 16: OLED_ShowASCII(0, 48, "n1+n2=>n1", OLED_8X16); break;
			case 17: OLED_ShowASCII(0, 48, "n1-n2=>n1", OLED_8X16); break;
			case 18: OLED_ShowASCII(0, 48, "n1*n2=>n1", OLED_8X16); break;
			case 19: OLED_ShowASCII(0, 48, "n1/n2=>n1", OLED_8X16); break;
			case 20: OLED_ShowASCII(0, 48, "n1 =>n1", OLED_8X16);OLED_ShowASCII(17, 48, "2", OLED_6X8); break;
			case 21: OLED_ShowASCII(3, 48, "/n1=>n1", OLED_8X16);OLED_DrawLine(3, 63, 1, 60);OLED_DrawLine(10, 49, 26, 49); break;
			case 22: OLED_ShowASCII(0, 48, "ln(n1)=>n1", OLED_8X16); break;
			case 23: OLED_ShowASCII(0, 48, "log10(n1)=>n1", OLED_8X16); break;
			case 24: OLED_ShowASCII(0, 48, "log  (n2)=>n1", OLED_8X16);OLED_ShowASCII(26, 56, "n1", OLED_6X8); break;
			case 25: OLED_ShowASCII(0, 48, "sin(n1(RAD)=>n1", OLED_8X16); break;
			case 26: OLED_ShowASCII(0, 48, "cos(n1(RAD)=>n1", OLED_8X16); break;
			case 27: OLED_ShowASCII(0, 48, "tan(n1(RAD)=>n1", OLED_8X16); break;
			case 28: OLED_ShowASCII(0, 48, "asin(n1)=>n1(RAD)", OLED_8X16); break;
			case 29: OLED_ShowASCII(0, 48, "acos(n1)=>n1(RAD)", OLED_8X16); break;
			case 30: OLED_ShowASCII(0, 48, "atan(n1)=>n1(RAD)", OLED_8X16); break;
			case 31: OLED_ShowASCII(0, 48, "n1(RAD)=>n1(DEG)", OLED_8X16); break;
			case 32: OLED_ShowASCII(0, 48, "n1(DEG)=>n1(RAD)", OLED_8X16); break;

			}
		}


		if (Computing)
		{
			if (Computing == 1)//加法
			{
				num1 = add(num1, num2);
			}

			else if (Computing == 2)//减法
			{
				num1 = subtract(num1, num2);
			}

			else if (Computing == 3)//乘法
			{
				num1 = multiply(num1, num2);
			}

			else if (Computing == 4)//除法
			{
				num1 = divide(num1, num2);
			}

			else if (Computing == 5)//求根
			{
				num1 = sqrt(num1);
			}

			else if (Computing == 6)//sin
			{
				num1 = sin(num1);
			}

			else if (Computing == 7)//cos
			{
				num1 = cos(num1);
			}

			else if (Computing == 8)//tan
			{
				num1 = tan(num1);
			}

			else if (Computing == 9)//反sin
			{
				num1 = asin(num1);
			}

			else if (Computing == 10)//反cos
			{
				num1 = acos(num1);
			}

			else if (Computing == 11)//反tan
			{
				num1 = atan(num1);
			}

			else if (Computing == 12)//ln
			{
				num1 = log(num1);
			}

			else if (Computing == 13)//log10
			{
				num1 = log10(num1);
			}

			else if (Computing == 14)//log a(b)
			{
				num1 = log(num2) / log(num1);
			}//换底公式

			else if (Computing == 15)
			{
				num1 = (num1 / PI * 180);
			}//弧度制转换角度制

			else if (Computing == 16)
			{
				num1 = (num1 * PI / 180);
			}//角度制转换弧度制

			else if (Computing == 17)
			{
				num1 = num1 * num1;
			}

			Computing = 0;
			for (u8 ip = 0;ip < MAX_INPUT_LENGTH;ip++) { input[ip] = '0';input2[ip] = '0'; }
			index = 0;index2 = 0;
		}

		if (KEYstate)
		{

			if (key == 'P')
			{
				switch (uting) {
				case 1:	num1 = PI; break;
				case 2: num2 = PI; break;
				}
			}
			else if (key == 'E')
			{
				switch (uting) {
				case 1:	num1 = E; break;
				case 2: num2 = E; break;
				}
			}
			else
			{
				switch (uting)
				{
				case 1:
					if (index < MAX_INPUT_LENGTH - 1)
					{
						input[index++] = key; // 添加小数点到输入数组  
						input[index] = '\0'; // 终止字符串  							
						num1 = atof(input);
					}
					break;
				case 2:
					if (index2 < MAX_INPUT_LENGTH - 1)
					{
						input2[index2++] = key; // 添加小数点到输入数组  
						input2[index2] = '\0'; // 终止字串  							
						num2 = atof(input2);
					}
					break;// 转换为double
				}
			}
			KEYstate = 0;
		}


		OLED_Update();
		OLED_transition(1);
		p = menu_Roll_event();
		if (p == 1)
		{
			if (vkey < 32)vkey++;
		}
		else if (p == -1)
		{
			if (vkey > 0)vkey--;
		}
		p = menu_Enter_event();
		if (p == 1)
		{
			if (vkey < 13)//输入数字
			{
				switch (vkey)
				{
				case 0: key = '0';break;
				case 1: key = '1';break;
				case 2: key = '2';break;
				case 3: key = '3';break;
				case 4: key = '4';break;
				case 5: key = '5';break;
				case 6: key = '6';break;
				case 7: key = '7';break;
				case 8: key = '8';break;
				case 9: key = '9';break;
				case 10: key = '.';break;
				case 11: key = 'P';break; //PI
				case 12: key = 'E';break;//e
				}
				KEYstate = 1;
			}
			else if (vkey >= 13)//输入函数
			{
				double temmp;
				int ttemp;
				switch (vkey) {
				case 13: if (uting == 1)uting = 2;else uting = 1;break;
				case 14: num1 = 0;num2 = 0;index = 0;for (u8 ip = 0;ip < MAX_INPUT_LENGTH;ip++) { input[ip] = '0';input2[ip] = '0'; }break;
				case 15: temmp = num1;num1 = num2;num2 = temmp;  ttemp = index;index = index2;index2 = ttemp;  Change(input, input2, MAX_INPUT_LENGTH);break;
				case 16: Computing = 1;break;
				case 17: Computing = 2;break;
				case 18: Computing = 3;break;
				case 19: Computing = 4;break;
				case 20: Computing = 17;break;
				case 21: Computing = 5;break;
				case 22: Computing = 12;break;
				case 23: Computing = 13;break;
				case 24: Computing = 14;break;
				case 25: Computing = 6;break;
				case 26: Computing = 7;break;
				case 27: Computing = 8;break;
				case 28: Computing = 9;break;
				case 29: Computing = 10;break;
				case 30: Computing = 11;break;
				case 31: Computing = 15;break;
				case 32: Computing = 16;break;
				}

			}

		}
		else if (p == 2)
		{
			OLED_transition(0);
			return;

		}
	}

}

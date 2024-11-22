#include "menu.h"
#include "sys.h"               
#include "stm32f10x.h"
#include <math.h>
#include "Time_page.h"
#include "voltage_mea.h"
#include "AHT20.h"
#include "Calculator.h"
#include "Delay.h" 
#include "dino.h"


//菜单框架借用了B站UP 加油哦大灰狼 的menu菜单修改而来,在此表示感谢

u8 d1 = 15, d2 = 1, yiup = 0, xiup, ytup = 0, xtup, s1;
//上课的时候脑抽想出来的按键识别,居然能用 (需要轮询),实现了点击与长按的识别
//按键识别,返回1为下,-1为上
//如果摇杆下拨
//如果摇杆上拨																		
s8 menu_Roll_event(void)
{
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1)
    {
        if (yiup)
        {
            if (xiup > s1)
            {
                s1 = d2;
                xiup = 0;
                return 1;
            }
            else
            {
                xiup++;
                return 0;
            }
        }
        else
        {
            yiup = 1;
            xiup = 0;
            s1 = d1;
            return 1;
        }
    }
    else
    {
        yiup = 0;
    }
    if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 1)
    {
        if (ytup)
        {
            if (xtup > s1)
            {
                s1 = d2;
                xtup = 0;
                return -1;
            }
            else
            {
                xtup++;return 0;
            }
        }
        else
        {
            ytup = 1;xtup = 0;s1 = d1;return -1;
        }
    }
    else
    {
        ytup = 0;
    }
    return 0;
}

u8 yu = 0, xu, upp = 0, s2;

//按键识别,返回1确定,2长按
s8 menu_Enter_event(void)
{
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 1)
    {
        if (upp == 0)
        {
            if (yu)
            {
                if (xu > s2)
                {
                    upp = 1;
                    return 2;
                }
                else
                {
                    xu++;
                    return 0;
                }
            }
            else
            {
                yu = 1;
                xu = 0;
                s2 = d1;
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        if (yu)
        {
            if (upp)
            {
                yu = 0;
                upp = 0;
                return 0;
            }
            else
            {
                yu = 0;
                return 1;
            }
        }
        return 0;
    }

    // if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 1)
    // {
    //     delay_ms(15);
    //     if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 1)
    //     {
    //         u8 i = 0;
    //         do
    //         {
    //             delay_ms(1);
    //         } while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 1 && --i);

    //         while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 1);
    //         if (i == 0)
    //         {
    //             return 2;
    //         }
    //         else
    //         {
    //             return 1;
    //         }
    //     }
    // }
    // return 0;

}

s8 Speed_Factor = 12;       //光标动画速度系数;
float  Roll_Speed = 1.3;        //滚动动画速度系数;
/**
  * 函    数:主菜单1
  * 参    数:无
  * 返 回 值:无
  * 说    明:按照格式添加选项,不要超过64个,可能会出问题,下次改成链表试试看
  *
  * 格    式:{ 选项名,需要运行的函数(无参数,无返回值),1份32x32的数组图片 }
  **/

void main_menu1(void)
{
    struct option_class1 option_list[] = {      //引用菜单类型1结构体option_class1

        {"返回表盘",     RETURN,              APPBIPAN},
        {"电压测量",     volt, 	              APPADDV},
        {"315Mhz遥控",   YAOKON315M,          APPYK315Mhz},
        {"微信",	     WeChat,              APPWXZF},
        {"支付宝",       Alipay,              APPZFBZF},
        {"Life Play",   Game_Of_Life_Play,   APPGAME1},
        {"Dino Play",   ShowGameAnimate,     APPGAME2},
        {"计算器",       Calculator,          APPCalculator},
        {"设置",         System_Settings,     APPsetting},
        {".."}	//结尾标志,方便计算数量

    };

    run_menu1(option_list);  //将参数载入菜单1函数刷新显示
}

/**
  * 函    数:主菜单2
  * 参    数:无
  * 返 回 值:无
  * 说    明:按照格式添加选项,不要超过64个,可能会出问题,下次改成链表试试看
  *
  * 格    式:{ 选项名 , 选项类型 ,需要运行的函数(无参数,无返回值)(可选) , &变量(可选) }
  *
  *	    选项类型:
  *		Display    仅展示 		  此状态下被执行时,不作任何动作,函数位不填,变量地址位不填
  *		Function   可进入函数:    此状态下被执行时, 进入一次被选中函数,变量地址位不填
  *   	ON_OFF     开关:         此状态下被执行时, 将传入的地址的被选中变量取反一次,再进入被选中函数
  *		Number     变量:		此状态下被执行时, 将进入在菜单内部的 变量调节器,每调节一次就进入被选中函数一次
  *     注意,需要返回功能时类型使用Function,需要运行的函数填RETURN,其他不填
  *     当类型为开关或变量时,RETURN充当空函数作用
  */
void main_menu2(void)
{
    struct option_class2 option_list[] = {	//引用菜单类型2结构体option_class2

        {"- 返回",          Function,     RETURN},
        {"- 手电筒",        Function,     flashlight},
        {"- 屏幕亮度",      Number,       Display,          &liangdu},
        {"- 秒表",          Function,     Stop_Watch},
        // {"- 闹钟",          Function,     error},
        // {"- 定时器",        Function,     error},
        {".."}	//结尾标志,方便计算数量

    };

    run_menu2(option_list);	 //将参数载入菜单2函数刷新显示
    System_Brightness();
}

u8 Cursor = 0;


/**
  * 函 数:菜单运行
  * 参 数:选项列表
  * 说 明:把选项列表显示出来,并根据按键事件执行相应操作
  */
void run_menu2(struct option_class2* option)
{
    s8  Catch_i = 1;		   // 选中下标
    s8  Cursor_i = 0;	       // 光标下标
    s8  Show_i = 0; 		   // 显示起始下标
    s8  Max = 0;			   // 选项数量
    s8  Roll_Event = 0;	       // 按键事件
    u8 Prdl;                  // 进度条单格长度 
    float  mubiao = 0;             // 目标进度条长度
    float  mubiao_up = 0;          //上次进度条长度

    while (option[++Max].Name[0] != '.');    	       //获取条目数量,如果文件名开头为'.'则为结尾;
    Max--;											  //不打印".."

    for (s8 i = 0; i <= Max; i++)				//计算选项名宽度;
    {
        option[i].NameLen = Get_NameLen(option[i].Name);
    }

    static float Cursor_len_d0 = 0, Cursor_len_d1 = 0, Cursor_i_d0 = 0, Cursor_i_d1 = 0;//光标位置和长度的起点终点

    s8 Show_d = 0, Show_i_temp = Max;				        //显示动画相关

    Prdl = 64 / Max;	  // 屏幕高度 / 条目数量

    while (1)
    {
        OLED_WriteCommand(0x40);	//设置显示开始行,0x40~0x7F
        OLED_WriteCommand(0xA1);	//设置左右方向,0xA1正常,0xA0左右反置
        OLED_WriteCommand(0xC8);	//设置上下方向,0xC8正常,0xC0上下反置

        OLED_Clear();

        Roll_Event = menu_Roll_event();				    //获取按键事件
        if (Roll_Event)							        //如果有按键事件
        {
            Cursor_i += Roll_Event;					    //更新下标
            Catch_i += Roll_Event;

            if (Catch_i < 0) { Catch_i = 0; }			//限制选中下标
            if (Catch_i > Max) { Catch_i = Max; }

            if (Cursor_i < 0) { Cursor_i = 0; }         //限制光标位置
            if (Cursor_i > 3) { Cursor_i = 3; }
            if (Cursor_i > Max) { Cursor_i = Max; }
        }


        //*****************************显示相关*****************************//

        Show_i = Catch_i - Cursor_i;            //计算显示起始下标

        if (Show_i - Show_i_temp)               //如果下标有偏移
        {
            Show_d = (Show_i - Show_i_temp) * MEHE;
            Show_i_temp = Show_i;
        }

        if (Show_d) { Show_d /= Roll_Speed; }	//滚动变化速度

        for (s8 i = 0; i < 5; i++)	        //遍历显示选项名和对应参数
        {
            if (Show_i + i > Max) { break; }
            OLED_ShowString(2, (i * MEHE) + Show_d + 2, option[Show_i + i].Name, OLED_6X8);

            if (option[Show_i + i].mode == ON_OFF)              //如果是开关型
            {
                OLED_DrawRectangle(96, (i * MEHE) + Show_d + 2, 12, 12, OLED_UNFILLED);
                if (*(option[Show_i + i].Num))    				//如果开关打开(传入值为真)
                {
                    OLED_DrawRectangle(99, (i * MEHE) + Show_d + 5, 6, 6, OLED_FILLED);
                }
            }
            else if (option[Show_i + i].mode == Number)         //如果是变量型
            {
                //如果百位不为零
                if (*(option[Show_i + i].Num) / OLED_Pow(10, 3 - 0 - 1) % 10)
                    OLED_ShowNum(92, (i * MEHE) + Show_d + 6, *(option[Show_i + i].Num), 3, OLED_6X8);
                //如果十位不为零
                else if (*(option[Show_i + i].Num) / OLED_Pow(10, 3 - 1 - 1) % 10)
                    OLED_ShowNum(95, (i * MEHE) + Show_d + 6, *(option[Show_i + i].Num), 2, OLED_6X8);
                //如果个位不为零
                else if (*(option[Show_i + i].Num) / OLED_Pow(10, 3 - 2 - 1) % 10)
                    OLED_ShowNum(98, (i * MEHE) + Show_d + 6, *(option[Show_i + i].Num), 1, OLED_6X8);

                else
                    OLED_ShowNum(98, (i * MEHE) + Show_d + 6, 0, 1, OLED_6X8);
            }
        }


        //*****************************光标相关*****************************//

        Cursor_i_d1 = Cursor_i * MEHE;						        //轮询光标目标位置
        Cursor_len_d1 = option[Catch_i].NameLen * WORD_H + 4;	    //轮询光标目标宽度

        /*计算此次循环光标位置*///如果当前位置不是目标位置,当前位置向目标位置移动
        if ((Cursor_i_d1 - Cursor_i_d0) > 1) { Cursor_i_d0 += (Cursor_i_d1 - Cursor_i_d0) / Speed_Factor + 1; }
        else if ((Cursor_i_d1 - Cursor_i_d0) < -1) { Cursor_i_d0 += (Cursor_i_d1 - Cursor_i_d0) / Speed_Factor - 1; }
        else { Cursor_i_d0 = Cursor_i_d1; }

        /*计算此次循环光标宽度*/
        if ((Cursor_len_d1 - Cursor_len_d0) > 1) { Cursor_len_d0 += (Cursor_len_d1 - Cursor_len_d0) / Speed_Factor + 1; }
        else if ((Cursor_len_d1 - Cursor_len_d0) < -1) { Cursor_len_d0 += (Cursor_len_d1 - Cursor_len_d0) / Speed_Factor - 1; }
        else { Cursor_len_d0 = Cursor_len_d1; }

        //显示光标//
        if (Cursor == 0) OLED_ReverseArea2(0, Cursor_i_d0, Cursor_len_d0, 16);           //对指定位置取饭

        else if (Cursor == 1) OLED_DrawRectangle(0, Cursor_i_d0, Cursor_len_d0, 16, OLED_UNFILLED);

        else if (Cursor == 2) OLED_ShowString(Cursor_len_d0, Cursor_i_d0 + 2, "<-", OLED_6X8);	//尾巴光标


        //******************************进度条*****************************//

        if (Catch_i == Max) { mubiao = 64; }
        else { mubiao = Prdl * Catch_i; }

        if (mubiao_up < mubiao) { mubiao_up += 1.4; }
        if (mubiao_up > mubiao) { mubiao_up -= 1.4; }

        OLED_DrawLine(123, 0, 127, 0);
        OLED_DrawLine(125, 0, 125, 63);
        OLED_DrawLine(123, 63, 127, 63);

        OLED_DrawRectangle(123, 0, 5, mubiao_up, OLED_FILLED);


        //*****************************刷新屏幕*****************************//

        OLED_Update();

        OLED_transition(1);


        //*****************************获取按键*****************************//

        if (menu_Enter_event())
        {
            if (option[Catch_i].mode == Function)          //如果是可进入函数
            {
                /*如果功能不为空则执行功能,否则返回*/
                if (option[Catch_i].func)
                {
                    OLED_transition(0);

                    option[Catch_i].func();
                }
                else
                {
                    OLED_transition(0);

                    return;
                }
            }
            else if (option[Catch_i].mode == ON_OFF)       //如果是开关
            {

                *(option[Catch_i].Num) = !*(option[Catch_i].Num);        //当前变量取反

                if (option[Catch_i].func) { option[Catch_i].func(); }    //执行功能一次

            }
            else if (option[Catch_i].mode == Number)       //如果是变量
            {

                OLED_transition(0);

                //内部的变量调节器//
                s8 p;
                float NP = 0, NP_target = 96;               //变量框框大小,目标框框大小
                float GP = 0, GP_target;                 //变量条条长度,目标条条长度
                //				float tap = 80/255;	     				 //条条单长	0.31

                while (1)
                {
                    if (NP < NP_target)              //如果变量框框没画完
                    {
                        if (NP_target - NP > 25) NP += 5;
                        else if (NP_target - NP > 5) NP += 2.5;
                        else if (NP_target - NP > 0) NP += 0.5;

                        OLED_DrawRectangle(16, 8, NP, NP / 2, OLED_UNFILLED);
                        OLED_ClearArea(17, 9, NP - 2, NP / 2 - 2);

                        OLED_Update();
                    }
                    else
                    {
                        //						 GP_target = *( option[Catch_i].Num) * tap ;						
                        GP_target = *(option[Catch_i].Num) * 0.31;
                        if (GP < GP_target)
                        {
                            if (GP_target - GP > 25) GP += 5;
                            else if (GP_target - GP > 7) GP += 2.4;
                            else if (GP_target - GP > 0) GP += 0.5;
                        }
                        if (GP > GP_target)
                        {
                            if (GP - GP_target > 25) GP -= 5;
                            else if (GP - GP_target > 7) GP -= 2.4;
                            else if (GP - GP_target > 0) GP -= 0.5;
                        }

                        OLED_ClearArea(24, 43, 80, 3);
                        OLED_DrawRectangle(24, 43, GP, 3, OLED_FILLED);

                        OLED_ClearArea(55, 20, 18, 8);
                        //如果百位不为零
                        if (*(option[Catch_i].Num) / OLED_Pow(10, 3 - 0 - 1) % 10)
                            OLED_ShowNum(55, 20, *(option[Catch_i].Num), 3, OLED_6X8);
                        //如果十位不为零
                        else if (*(option[Catch_i].Num) / OLED_Pow(10, 3 - 1 - 1) % 10)
                            OLED_ShowNum(58, 20, *(option[Catch_i].Num), 2, OLED_6X8);
                        //如果个位不为零
                        else if (*(option[Catch_i].Num) / OLED_Pow(10, 3 - 2 - 1) % 10)
                            OLED_ShowNum(61, 20, *(option[Catch_i].Num), 1, OLED_6X8);
                        else
                            OLED_ShowNum(61, 20, 0, 1, OLED_6X8);

                        OLED_Update();

                    }

                    OLED_transition(1);

                    p = menu_Roll_event();
                    if (p == 1)
                    {
                        *(option[Catch_i].Num) += 1;
                        if (option[Catch_i].func) { option[Catch_i].func(); } //执行功能一次

                    }
                    else if (p == -1)
                    {
                        *(option[Catch_i].Num) -= 1;
                        if (option[Catch_i].func) { option[Catch_i].func(); } //执行功能一次
                    }

                    p = menu_Enter_event();
                    if (p == 1)
                    {
                        OLED_transition(0);
                        OLED_Clear();
                        OLED_Update();
                        break;
                    }
                    else if (p == 2)
                    {

                        OLED_transition(0);
                        OLED_Clear();
                        OLED_Update();


                        break;
                    }
                }

            }

            //如果既不是,变量,开关和函数,一率当做展示处理


        }

    }
}

/**********************************************************/

void run_menu1(struct option_class1* option)
{

    s8  Catch_i = 1;		   //选中下标
    s8  Show_i = 0; 		   //显示起始下标
    s8  Max = 0;			   //选项数量
    s8  Roll_Event = 0;	       //按键事件


    while (option[++Max].Name[0] != '.');       //获取条目数量,如果文件名开头为'.'则为结尾;
    Max--;									    //不打印".."

    for (s8 i = 0; i <= Max; i++)           //计算选项名宽度
    {
        option[i].NameLen = Get_NameLen(option[i].Name);
    }

    s8 Show_d = 127, Show_i_temp = Max, yi = 10;    //显示动画相关



    float yp = 0, ypt = 0;

    OLED_Clear();
    OLED_Update();

    static int humidity, temperature;

    while (1)
    {
        OLED_WriteCommand(0x40);	//设置显示开始行,0x40~0x7F

        OLED_WriteCommand(0xA1);	//设置左右方向,0xA1正常,0xA0左右反置

        OLED_WriteCommand(0xC8);	//设置上下方向,0xC8正常,0xC0上下反置


        OLED_Clear();

        Roll_Event = menu_Roll_event();				      //获取按键事件
        if (Roll_Event)							         //如果有按键事件
        {
            Catch_i += Roll_Event;

            if (Catch_i < 0) { Catch_i = 0; }			//限制选中下标
            if (Catch_i > Max) { Catch_i = Max; }

        }

        /**********************************************************/
        /*显示相关*/

        Show_i = Catch_i;			    //计算显示起始下标


        if (Show_i - Show_i_temp)       //如果下标有偏移
        {
            Show_d = (Show_i - Show_i_temp) * 40;
            Show_i_temp = Show_i;

            yp = option[Catch_i].NameLen * WORD_H;
        }
        if (Show_d) { Show_d /= 1.15; }	//滚动变化速度
        if (yi) { yi /= 1.15; }	        //横向变化速度


        if (ypt < yp)
        {
            if (yp - ypt > 10) ypt += 5;
            else if (yp - ypt > 5) ypt += 2.5;
            else if (yp - ypt > 0) ypt += 0.5;
        }
        if (ypt > yp)
        {
            if (ypt - yp > 10) ypt -= 5;
            else if (ypt - yp > 5) ypt -= 2.5;
            else if (ypt - yp > 0) ypt -= 0.5;
        }


        for (s8 i = -2; i < 3; i++)	  //遍历显示选项名和对应参数
        {
            s8 temp = i * 40 + Show_d + 16;

            if (Show_i + i < 0) { continue; }
            if (Show_i + i > Max) { break; }

            OLED_ShowImage(78 + yi, temp, 32, 32, option[Show_i + i].Image);

            u8 tp = abs(Show_d / 3);

            OLED_ShowString(32 - yp / 2, 30 + tp, option[Catch_i].Name, OLED_6X8);

            OLED_DrawRectangle(32 - (ypt / 2) - 2, 43, ypt + 4, 1, OLED_UNFILLED);

            OLED_ClearArea(0, 44, 78, 4);

        }

        /**********************************************************/

        Time_page_small_Timer(0, 0);
        Time_page_BAT(250, 8);
        // Time_page_BAT(-6, 8);//这里可能会提示警告,可以不管它	

        /**********************************************************/
        int Numtemp1, Numtemp2;

        AHT20_Get_data(&humidity, &temperature);//获取温湿度信息

        // 获取个位数字  
        Numtemp2 = humidity % 10;
        // 获取除个位外的其他数字  
        Numtemp1 = humidity / 10;
        OLED_ShowNum(0, 56, Numtemp1, 2, OLED_6X8);
        OLED_DrawPoint(0 + 12 + 1, 56 + 6);
        OLED_DrawPoint(0 + 12 + 1, 56 + 7);
        OLED_DrawPoint(0 + 12 + 2, 56 + 6);
        OLED_DrawPoint(0 + 12 + 2, 56 + 7);
        OLED_ShowNum(0 + 12 + 3, 56, Numtemp2, 1, OLED_6X8);
        OLED_ShowASCII(0 + 18 + 3, 56, "%", OLED_6X8);

        // 获取个位数字  
        Numtemp2 = temperature % 10;
        // 获取除个位外的其他数字  
        Numtemp1 = temperature / 10;
        OLED_ShowNum(0 + 30 + 2, 56, Numtemp1, 2, OLED_6X8);
        OLED_DrawPoint(0 + 12 + 1 + 30 + 2, 56 + 6);
        OLED_DrawPoint(0 + 12 + 1 + 30 + 2, 56 + 7);
        OLED_DrawPoint(0 + 12 + 2 + 30 + 2, 56 + 6);
        OLED_DrawPoint(0 + 12 + 2 + 30 + 2, 56 + 7);
        OLED_ShowNum(0 + 12 + 3 + 30 + 2, 56, Numtemp2, 1, OLED_6X8);
        OLED_DrawPoint(0 + 18 + 3 + 30 + 1 + 2, 56);
        OLED_ShowASCII(0 + 18 + 3 + 30 + 2 + 2, 56, "C", OLED_6X8);

        /**********************************************************/
           //刷新屏幕//	

        OLED_UpdateArea(78 + yi, 0, 35, 64);
        OLED_UpdateArea(0, 30, 78, 12);
        OLED_UpdateArea(0, 0, 54, 7);
        OLED_UpdateArea(0, 8, 40, 8);
        OLED_UpdateArea(0, 56, 60, 8);

        OLED_transition(1);

        /**********************************************************/
           //获取按键//
        if (menu_Enter_event())
        {
            /*如果功能不为空则执行功能,否则返回*/
            if (option[Catch_i].func)
            {
                OLED_transition(0);
                option[Catch_i].func();
                OLED_Clear();
                OLED_Update();

                yi = 10;
                Show_d = -127;

            }
            else
            {
                OLED_transition(0);
                return;
            }

        }

    }

}

//计算选项名宽度
u8 Get_NameLen(char* String)
{
    u8 i = 0, len = 0;
    while (String[i] != '\0')   //遍历字符串的每个字符
    {
        if (String[i] > '~')
        {
            len += 2;
            i += 2;     //如果不属于英文字符,因为GB2312编码格式长度加 2(i+=2), UTF-8编码格式则加 3
        }
        else
        {
            len += 1; i += 1;   //属于英文字符长度加1
        }
    }
    return len;
}

void OLED_ReverseArea2(u8 X, u8 Y, u8 Width, u8 Height)
{
    OLED_ReverseArea(X + 1, Y, Width - 2, 1);
    OLED_ReverseArea(X, Y + 1, Width, Height - 2);
    OLED_ReverseArea(X + 1, Y + Height - 1, Width - 2, 1);
}

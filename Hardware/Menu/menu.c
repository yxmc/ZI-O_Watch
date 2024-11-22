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


//�˵���ܽ�����BվUP ����Ŷ����� ��menu�˵��޸Ķ���,�ڴ˱�ʾ��л

u8 d1 = 15, d2 = 1, yiup = 0, xiup, ytup = 0, xtup, s1;
//�Ͽε�ʱ���Գ�������İ���ʶ��,��Ȼ���� (��Ҫ��ѯ),ʵ���˵���볤����ʶ��
//����ʶ��,����1Ϊ��,-1Ϊ��
//���ҡ���²�
//���ҡ���ϲ�																		
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

//����ʶ��,����1ȷ��,2����
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

s8 Speed_Factor = 12;       //��궯���ٶ�ϵ��;
float  Roll_Speed = 1.3;        //���������ٶ�ϵ��;
/**
  * ��    ��:���˵�1
  * ��    ��:��
  * �� �� ֵ:��
  * ˵    ��:���ո�ʽ���ѡ��,��Ҫ����64��,���ܻ������,�´θĳ��������Կ�
  *
  * ��    ʽ:{ ѡ����,��Ҫ���еĺ���(�޲���,�޷���ֵ),1��32x32������ͼƬ }
  **/

void main_menu1(void)
{
    struct option_class1 option_list[] = {      //���ò˵�����1�ṹ��option_class1

        {"���ر���",     RETURN,              APPBIPAN},
        {"��ѹ����",     volt, 	              APPADDV},
        {"315Mhzң��",   YAOKON315M,          APPYK315Mhz},
        {"΢��",	     WeChat,              APPWXZF},
        {"֧����",       Alipay,              APPZFBZF},
        {"Life Play",   Game_Of_Life_Play,   APPGAME1},
        {"Dino Play",   ShowGameAnimate,     APPGAME2},
        {"������",       Calculator,          APPCalculator},
        {"����",         System_Settings,     APPsetting},
        {".."}	//��β��־,�����������

    };

    run_menu1(option_list);  //����������˵�1����ˢ����ʾ
}

/**
  * ��    ��:���˵�2
  * ��    ��:��
  * �� �� ֵ:��
  * ˵    ��:���ո�ʽ���ѡ��,��Ҫ����64��,���ܻ������,�´θĳ��������Կ�
  *
  * ��    ʽ:{ ѡ���� , ѡ������ ,��Ҫ���еĺ���(�޲���,�޷���ֵ)(��ѡ) , &����(��ѡ) }
  *
  *	    ѡ������:
  *		Display    ��չʾ 		  ��״̬�±�ִ��ʱ,�����κζ���,����λ����,������ַλ����
  *		Function   �ɽ��뺯��:    ��״̬�±�ִ��ʱ, ����һ�α�ѡ�к���,������ַλ����
  *   	ON_OFF     ����:         ��״̬�±�ִ��ʱ, ������ĵ�ַ�ı�ѡ�б���ȡ��һ��,�ٽ��뱻ѡ�к���
  *		Number     ����:		��״̬�±�ִ��ʱ, �������ڲ˵��ڲ��� ����������,ÿ����һ�ξͽ��뱻ѡ�к���һ��
  *     ע��,��Ҫ���ع���ʱ����ʹ��Function,��Ҫ���еĺ�����RETURN,��������
  *     ������Ϊ���ػ����ʱ,RETURN�䵱�պ�������
  */
void main_menu2(void)
{
    struct option_class2 option_list[] = {	//���ò˵�����2�ṹ��option_class2

        {"- ����",          Function,     RETURN},
        {"- �ֵ�Ͳ",        Function,     flashlight},
        {"- ��Ļ����",      Number,       Display,          &liangdu},
        {"- ���",          Function,     Stop_Watch},
        // {"- ����",          Function,     error},
        // {"- ��ʱ��",        Function,     error},
        {".."}	//��β��־,�����������

    };

    run_menu2(option_list);	 //����������˵�2����ˢ����ʾ
    System_Brightness();
}

u8 Cursor = 0;


/**
  * �� ��:�˵�����
  * �� ��:ѡ���б�
  * ˵ ��:��ѡ���б���ʾ����,�����ݰ����¼�ִ����Ӧ����
  */
void run_menu2(struct option_class2* option)
{
    s8  Catch_i = 1;		   // ѡ���±�
    s8  Cursor_i = 0;	       // ����±�
    s8  Show_i = 0; 		   // ��ʾ��ʼ�±�
    s8  Max = 0;			   // ѡ������
    s8  Roll_Event = 0;	       // �����¼�
    u8 Prdl;                  // ���������񳤶� 
    float  mubiao = 0;             // Ŀ�����������
    float  mubiao_up = 0;          //�ϴν���������

    while (option[++Max].Name[0] != '.');    	       //��ȡ��Ŀ����,����ļ�����ͷΪ'.'��Ϊ��β;
    Max--;											  //����ӡ".."

    for (s8 i = 0; i <= Max; i++)				//����ѡ�������;
    {
        option[i].NameLen = Get_NameLen(option[i].Name);
    }

    static float Cursor_len_d0 = 0, Cursor_len_d1 = 0, Cursor_i_d0 = 0, Cursor_i_d1 = 0;//���λ�úͳ��ȵ�����յ�

    s8 Show_d = 0, Show_i_temp = Max;				        //��ʾ�������

    Prdl = 64 / Max;	  // ��Ļ�߶� / ��Ŀ����

    while (1)
    {
        OLED_WriteCommand(0x40);	//������ʾ��ʼ��,0x40~0x7F
        OLED_WriteCommand(0xA1);	//�������ҷ���,0xA1����,0xA0���ҷ���
        OLED_WriteCommand(0xC8);	//�������·���,0xC8����,0xC0���·���

        OLED_Clear();

        Roll_Event = menu_Roll_event();				    //��ȡ�����¼�
        if (Roll_Event)							        //����а����¼�
        {
            Cursor_i += Roll_Event;					    //�����±�
            Catch_i += Roll_Event;

            if (Catch_i < 0) { Catch_i = 0; }			//����ѡ���±�
            if (Catch_i > Max) { Catch_i = Max; }

            if (Cursor_i < 0) { Cursor_i = 0; }         //���ƹ��λ��
            if (Cursor_i > 3) { Cursor_i = 3; }
            if (Cursor_i > Max) { Cursor_i = Max; }
        }


        //*****************************��ʾ���*****************************//

        Show_i = Catch_i - Cursor_i;            //������ʾ��ʼ�±�

        if (Show_i - Show_i_temp)               //����±���ƫ��
        {
            Show_d = (Show_i - Show_i_temp) * MEHE;
            Show_i_temp = Show_i;
        }

        if (Show_d) { Show_d /= Roll_Speed; }	//�����仯�ٶ�

        for (s8 i = 0; i < 5; i++)	        //������ʾѡ�����Ͷ�Ӧ����
        {
            if (Show_i + i > Max) { break; }
            OLED_ShowString(2, (i * MEHE) + Show_d + 2, option[Show_i + i].Name, OLED_6X8);

            if (option[Show_i + i].mode == ON_OFF)              //����ǿ�����
            {
                OLED_DrawRectangle(96, (i * MEHE) + Show_d + 2, 12, 12, OLED_UNFILLED);
                if (*(option[Show_i + i].Num))    				//������ش�(����ֵΪ��)
                {
                    OLED_DrawRectangle(99, (i * MEHE) + Show_d + 5, 6, 6, OLED_FILLED);
                }
            }
            else if (option[Show_i + i].mode == Number)         //����Ǳ�����
            {
                //�����λ��Ϊ��
                if (*(option[Show_i + i].Num) / OLED_Pow(10, 3 - 0 - 1) % 10)
                    OLED_ShowNum(92, (i * MEHE) + Show_d + 6, *(option[Show_i + i].Num), 3, OLED_6X8);
                //���ʮλ��Ϊ��
                else if (*(option[Show_i + i].Num) / OLED_Pow(10, 3 - 1 - 1) % 10)
                    OLED_ShowNum(95, (i * MEHE) + Show_d + 6, *(option[Show_i + i].Num), 2, OLED_6X8);
                //�����λ��Ϊ��
                else if (*(option[Show_i + i].Num) / OLED_Pow(10, 3 - 2 - 1) % 10)
                    OLED_ShowNum(98, (i * MEHE) + Show_d + 6, *(option[Show_i + i].Num), 1, OLED_6X8);

                else
                    OLED_ShowNum(98, (i * MEHE) + Show_d + 6, 0, 1, OLED_6X8);
            }
        }


        //*****************************������*****************************//

        Cursor_i_d1 = Cursor_i * MEHE;						        //��ѯ���Ŀ��λ��
        Cursor_len_d1 = option[Catch_i].NameLen * WORD_H + 4;	    //��ѯ���Ŀ����

        /*����˴�ѭ�����λ��*///�����ǰλ�ò���Ŀ��λ��,��ǰλ����Ŀ��λ���ƶ�
        if ((Cursor_i_d1 - Cursor_i_d0) > 1) { Cursor_i_d0 += (Cursor_i_d1 - Cursor_i_d0) / Speed_Factor + 1; }
        else if ((Cursor_i_d1 - Cursor_i_d0) < -1) { Cursor_i_d0 += (Cursor_i_d1 - Cursor_i_d0) / Speed_Factor - 1; }
        else { Cursor_i_d0 = Cursor_i_d1; }

        /*����˴�ѭ�������*/
        if ((Cursor_len_d1 - Cursor_len_d0) > 1) { Cursor_len_d0 += (Cursor_len_d1 - Cursor_len_d0) / Speed_Factor + 1; }
        else if ((Cursor_len_d1 - Cursor_len_d0) < -1) { Cursor_len_d0 += (Cursor_len_d1 - Cursor_len_d0) / Speed_Factor - 1; }
        else { Cursor_len_d0 = Cursor_len_d1; }

        //��ʾ���//
        if (Cursor == 0) OLED_ReverseArea2(0, Cursor_i_d0, Cursor_len_d0, 16);           //��ָ��λ��ȡ��

        else if (Cursor == 1) OLED_DrawRectangle(0, Cursor_i_d0, Cursor_len_d0, 16, OLED_UNFILLED);

        else if (Cursor == 2) OLED_ShowString(Cursor_len_d0, Cursor_i_d0 + 2, "<-", OLED_6X8);	//β�͹��


        //******************************������*****************************//

        if (Catch_i == Max) { mubiao = 64; }
        else { mubiao = Prdl * Catch_i; }

        if (mubiao_up < mubiao) { mubiao_up += 1.4; }
        if (mubiao_up > mubiao) { mubiao_up -= 1.4; }

        OLED_DrawLine(123, 0, 127, 0);
        OLED_DrawLine(125, 0, 125, 63);
        OLED_DrawLine(123, 63, 127, 63);

        OLED_DrawRectangle(123, 0, 5, mubiao_up, OLED_FILLED);


        //*****************************ˢ����Ļ*****************************//

        OLED_Update();

        OLED_transition(1);


        //*****************************��ȡ����*****************************//

        if (menu_Enter_event())
        {
            if (option[Catch_i].mode == Function)          //����ǿɽ��뺯��
            {
                /*������ܲ�Ϊ����ִ�й���,���򷵻�*/
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
            else if (option[Catch_i].mode == ON_OFF)       //����ǿ���
            {

                *(option[Catch_i].Num) = !*(option[Catch_i].Num);        //��ǰ����ȡ��

                if (option[Catch_i].func) { option[Catch_i].func(); }    //ִ�й���һ��

            }
            else if (option[Catch_i].mode == Number)       //����Ǳ���
            {

                OLED_transition(0);

                //�ڲ��ı���������//
                s8 p;
                float NP = 0, NP_target = 96;               //��������С,Ŀ�����С
                float GP = 0, GP_target;                 //������������,Ŀ����������
                //				float tap = 80/255;	     				 //��������	0.31

                while (1)
                {
                    if (NP < NP_target)              //����������û����
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
                        //�����λ��Ϊ��
                        if (*(option[Catch_i].Num) / OLED_Pow(10, 3 - 0 - 1) % 10)
                            OLED_ShowNum(55, 20, *(option[Catch_i].Num), 3, OLED_6X8);
                        //���ʮλ��Ϊ��
                        else if (*(option[Catch_i].Num) / OLED_Pow(10, 3 - 1 - 1) % 10)
                            OLED_ShowNum(58, 20, *(option[Catch_i].Num), 2, OLED_6X8);
                        //�����λ��Ϊ��
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
                        if (option[Catch_i].func) { option[Catch_i].func(); } //ִ�й���һ��

                    }
                    else if (p == -1)
                    {
                        *(option[Catch_i].Num) -= 1;
                        if (option[Catch_i].func) { option[Catch_i].func(); } //ִ�й���һ��
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

            //����Ȳ���,����,���غͺ���,һ�ʵ���չʾ����


        }

    }
}

/**********************************************************/

void run_menu1(struct option_class1* option)
{

    s8  Catch_i = 1;		   //ѡ���±�
    s8  Show_i = 0; 		   //��ʾ��ʼ�±�
    s8  Max = 0;			   //ѡ������
    s8  Roll_Event = 0;	       //�����¼�


    while (option[++Max].Name[0] != '.');       //��ȡ��Ŀ����,����ļ�����ͷΪ'.'��Ϊ��β;
    Max--;									    //����ӡ".."

    for (s8 i = 0; i <= Max; i++)           //����ѡ�������
    {
        option[i].NameLen = Get_NameLen(option[i].Name);
    }

    s8 Show_d = 127, Show_i_temp = Max, yi = 10;    //��ʾ�������



    float yp = 0, ypt = 0;

    OLED_Clear();
    OLED_Update();

    static int humidity, temperature;

    while (1)
    {
        OLED_WriteCommand(0x40);	//������ʾ��ʼ��,0x40~0x7F

        OLED_WriteCommand(0xA1);	//�������ҷ���,0xA1����,0xA0���ҷ���

        OLED_WriteCommand(0xC8);	//�������·���,0xC8����,0xC0���·���


        OLED_Clear();

        Roll_Event = menu_Roll_event();				      //��ȡ�����¼�
        if (Roll_Event)							         //����а����¼�
        {
            Catch_i += Roll_Event;

            if (Catch_i < 0) { Catch_i = 0; }			//����ѡ���±�
            if (Catch_i > Max) { Catch_i = Max; }

        }

        /**********************************************************/
        /*��ʾ���*/

        Show_i = Catch_i;			    //������ʾ��ʼ�±�


        if (Show_i - Show_i_temp)       //����±���ƫ��
        {
            Show_d = (Show_i - Show_i_temp) * 40;
            Show_i_temp = Show_i;

            yp = option[Catch_i].NameLen * WORD_H;
        }
        if (Show_d) { Show_d /= 1.15; }	//�����仯�ٶ�
        if (yi) { yi /= 1.15; }	        //����仯�ٶ�


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


        for (s8 i = -2; i < 3; i++)	  //������ʾѡ�����Ͷ�Ӧ����
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
        // Time_page_BAT(-6, 8);//������ܻ���ʾ����,���Բ�����	

        /**********************************************************/
        int Numtemp1, Numtemp2;

        AHT20_Get_data(&humidity, &temperature);//��ȡ��ʪ����Ϣ

        // ��ȡ��λ����  
        Numtemp2 = humidity % 10;
        // ��ȡ����λ�����������  
        Numtemp1 = humidity / 10;
        OLED_ShowNum(0, 56, Numtemp1, 2, OLED_6X8);
        OLED_DrawPoint(0 + 12 + 1, 56 + 6);
        OLED_DrawPoint(0 + 12 + 1, 56 + 7);
        OLED_DrawPoint(0 + 12 + 2, 56 + 6);
        OLED_DrawPoint(0 + 12 + 2, 56 + 7);
        OLED_ShowNum(0 + 12 + 3, 56, Numtemp2, 1, OLED_6X8);
        OLED_ShowASCII(0 + 18 + 3, 56, "%", OLED_6X8);

        // ��ȡ��λ����  
        Numtemp2 = temperature % 10;
        // ��ȡ����λ�����������  
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
           //ˢ����Ļ//	

        OLED_UpdateArea(78 + yi, 0, 35, 64);
        OLED_UpdateArea(0, 30, 78, 12);
        OLED_UpdateArea(0, 0, 54, 7);
        OLED_UpdateArea(0, 8, 40, 8);
        OLED_UpdateArea(0, 56, 60, 8);

        OLED_transition(1);

        /**********************************************************/
           //��ȡ����//
        if (menu_Enter_event())
        {
            /*������ܲ�Ϊ����ִ�й���,���򷵻�*/
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

//����ѡ�������
u8 Get_NameLen(char* String)
{
    u8 i = 0, len = 0;
    while (String[i] != '\0')   //�����ַ�����ÿ���ַ�
    {
        if (String[i] > '~')
        {
            len += 2;
            i += 2;     //���������Ӣ���ַ�,��ΪGB2312�����ʽ���ȼ� 2(i+=2), UTF-8�����ʽ��� 3
        }
        else
        {
            len += 1; i += 1;   //����Ӣ���ַ����ȼ�1
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

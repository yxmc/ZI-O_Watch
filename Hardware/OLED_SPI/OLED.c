
/***************************************************************************************
  * �������ɽ�Э�Ƽ���������ѿ�Դ����
  * ���������鿴��ʹ�ú��޸�,��Ӧ�õ��Լ�����Ŀ֮��
  * �����Ȩ�齭Э�Ƽ�����,�κ��˻���֯���ý����Ϊ����
  *
  * ��������:				0.96��OLED��ʾ����������(7���SPI�ӿ�)
  * ���򴴽�ʱ��:			2023.10.24
  * ��ǰ����汾:			V1.1
  * ��ǰ�汾����ʱ��:		2023.12.8
  *
  * ��Э�Ƽ��ٷ���վ:		jiangxiekeji.com
  * ��Э�Ƽ��ٷ��Ա���:	jiangxiekeji.taobao.com
  * ������ܼ����¶�̬:	jiangxiekeji.com/tutorial/oled.html
  *
  * ����㷢�ֳ����е�©�����߱���,��ͨ���ʼ������Ƿ���:feedback@jiangxiekeji.com
  * �����ʼ�֮ǰ,������ȵ����¶�̬ҳ��鿴���³���,����������Ѿ��޸�,�������ٷ��ʼ�
  ***************************************************************************************
  */

#include "stm32f10x.h"
#include "OLED.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include "Delay.h"


  /**
	* ���ݴ洢��ʽ:
	* ����8��,��λ����,�ȴ�����,�ٴ��ϵ���
	* ÿһ��Bit��Ӧһ�����ص�
	*
	*      B0 B0                  B0 B0
	*      B1 B1                  B1 B1
	*      B2 B2                  B2 B2
	*      B3 B3  ------------->  B3 B3 --
	*      B4 B4                  B4 B4  |
	*      B5 B5                  B5 B5  |
	*      B6 B6                  B6 B6  |
	*      B7 B7                  B7 B7  |
	*                                    |
	*  -----------------------------------
	*  |
	*  |   B0 B0                  B0 B0
	*  |   B1 B1                  B1 B1
	*  |   B2 B2                  B2 B2
	*  --> B3 B3  ------------->  B3 B3
	*      B4 B4                  B4 B4
	*      B5 B5                  B5 B5
	*      B6 B6                  B6 B6
	*      B7 B7                  B7 B7
	*
	* �����ᶨ��:
	* ���Ͻ�Ϊ(0, 0)��
	* ��������ΪX��,ȡֵ��Χ:0~127
	* ��������ΪY��,ȡֵ��Χ:0~63
	*
	*       0             X��           127
	*      .------------------------------->
	*    0 |
	*      |
	*      |
	*      |
	*  Y�� |
	*      |
	*      |
	*      |
	*   63 |
	*      v
	*
	*/


	/*ȫ�ֱ���*********************/

	/**
	  * OLED�Դ�����
	  * ���е���ʾ����,��ֻ�ǶԴ��Դ�������ж�д
	  * ������OLED_Update������OLED_UpdateArea����
	  * �ŻὫ�Դ���������ݷ��͵�OLEDӲ��,������ʾ
	  */
u8 OLED_DisplayBuf[8][128];

/*********************ȫ�ֱ���*/


/*��������*********************/

/**
  * ��    ��:OLEDдD0(CLK)�ߵ͵�ƽ
  * ��    ��:Ҫд��D0�ĵ�ƽֵ,��Χ:0/1
  * �� �� ֵ:��
  * ˵    ��:���ϲ㺯����ҪдD0ʱ,�˺����ᱻ����
  *           �û���Ҫ���ݲ��������ֵ,��D0��Ϊ�ߵ�ƽ���ߵ͵�ƽ
  *           ����������0ʱ,��D0Ϊ�͵�ƽ,����������1ʱ,��D0Ϊ�ߵ�ƽ
  */
void OLED_W_D0(u8 BitValue)
{
	/*����BitValue��ֵ,��D0�øߵ�ƽ���ߵ͵�ƽ*/
	GPIO_WriteBit(GPIOA, GPIO_Pin_8, (BitAction)BitValue);
}

/**
  * ��    ��:OLEDдD1(MOSI)�ߵ͵�ƽ
  * ��    ��:Ҫд��D1�ĵ�ƽֵ,��Χ:0/1
  * �� �� ֵ:��
  * ˵    ��:���ϲ㺯����ҪдD1ʱ,�˺����ᱻ����
  *           �û���Ҫ���ݲ��������ֵ,��D1��Ϊ�ߵ�ƽ���ߵ͵�ƽ
  *           ����������0ʱ,��D1Ϊ�͵�ƽ,����������1ʱ,��D1Ϊ�ߵ�ƽ
  */
void OLED_W_D1(u8 BitValue)
{
	/*����BitValue��ֵ,��D1�øߵ�ƽ���ߵ͵�ƽ*/
	GPIO_WriteBit(GPIOB, GPIO_Pin_15, (BitAction)BitValue);
}

/**
  * ��    ��:OLEDдRES�ߵ͵�ƽ
  * ��    ��:Ҫд��RES�ĵ�ƽֵ,��Χ:0/1
  * �� �� ֵ:��
  * ˵    ��:���ϲ㺯����ҪдRESʱ,�˺����ᱻ����
  *           �û���Ҫ���ݲ��������ֵ,��RES��Ϊ�ߵ�ƽ���ߵ͵�ƽ
  *           ����������0ʱ,��RESΪ�͵�ƽ,����������1ʱ,��RESΪ�ߵ�ƽ
  */
void OLED_W_RES(u8 BitValue)
{
	/*����BitValue��ֵ,��RES�øߵ�ƽ���ߵ͵�ƽ*/
	GPIO_WriteBit(GPIOB, GPIO_Pin_14, (BitAction)BitValue);
}

/**
  * ��    ��:OLEDдDC�ߵ͵�ƽ
  * ��    ��:Ҫд��DC�ĵ�ƽֵ,��Χ:0/1
  * �� �� ֵ:��
  * ˵    ��:���ϲ㺯����ҪдDCʱ,�˺����ᱻ����
  *           �û���Ҫ���ݲ��������ֵ,��DC��Ϊ�ߵ�ƽ���ߵ͵�ƽ
  *           ����������0ʱ,��DCΪ�͵�ƽ,����������1ʱ,��DCΪ�ߵ�ƽ
  */
void OLED_W_DC(u8 BitValue)
{
	/*����BitValue��ֵ,��DC�øߵ�ƽ���ߵ͵�ƽ*/
	GPIO_WriteBit(GPIOB, GPIO_Pin_13, (BitAction)BitValue);
}

/**
  * ��    ��:OLEDдCS�ߵ͵�ƽ
  * ��    ��:Ҫд��CS�ĵ�ƽֵ,��Χ:0/1
  * �� �� ֵ:��
  * ˵    ��:���ϲ㺯����ҪдCSʱ,�˺����ᱻ����
  *           �û���Ҫ���ݲ��������ֵ,��CS��Ϊ�ߵ�ƽ���ߵ͵�ƽ
  *           ����������0ʱ,��CSΪ�͵�ƽ,����������1ʱ,��CSΪ�ߵ�ƽ
  */
void OLED_W_CS(u8 BitValue)
{
	/*����BitValue��ֵ,��CS�øߵ�ƽ���ߵ͵�ƽ*/
	GPIO_WriteBit(GPIOB, GPIO_Pin_12, (BitAction)BitValue);
}

/**
  * ��    ��:OLED���ų�ʼ��
  * ��    ��:��
  * �� �� ֵ:��
  * ˵    ��:���ϲ㺯����Ҫ��ʼ��ʱ,�˺����ᱻ����
  *           �û���Ҫ��D0��D1��RES��DC��CS���ų�ʼ��Ϊ�������ģʽ
  */
void OLED_GPIO_Init(void)
{
	// u32 i, j;

	/*�ڳ�ʼ��ǰ,����������ʱ,��OLED�����ȶ�*/
	// for (i = 0; i < 1000; i++)
	// {
	// 	for (j = 0; j < 1000; j++);
	// }

	delay_ms(1000);

	/*��D0��D1��RES��DC��CS���ų�ʼ��Ϊ�������ģʽ*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/*������Ĭ�ϵ�ƽ*/
	OLED_W_D0(0);
	OLED_W_D1(1);
	OLED_W_RES(1);
	OLED_W_DC(1);
	OLED_W_CS(1);
}

/*********************��������*/


/*ͨ��Э��*********************/

/**
  * ��    ��:SPI����һ���ֽ�
  * ��    ��:Byte Ҫ���͵�һ���ֽ�����,��Χ:0x00~0xFF
  * �� �� ֵ:��
  */
void OLED_SPI_SendByte(u8 Byte)
{
	u8 i;

	/*ѭ��8��,�������η������ݵ�ÿһλ*/
	for (i = 0; i < 8; i++)
	{
		/*ʹ������ķ�ʽȡ��Byte��ָ��һλ���ݲ�д�뵽D1��*/
		/*����!��������,�����з����ֵ��Ϊ1*/
		OLED_W_D1(!!(Byte & (0x80 >> i)));
		OLED_W_D0(1);	//����D0,�ӻ���D0�����ض�ȡSDA
		OLED_W_D0(0);	//����D0,������ʼ������һλ����
	}
}

/**
  * ��    ��:OLEDд����
  * ��    ��:Command Ҫд�������ֵ,��Χ:0x00~0xFF
  * �� �� ֵ:��
  */
void OLED_WriteCommand(u8 Command)
{
	OLED_W_CS(0);					//����CS,��ʼͨ��
	OLED_W_DC(0);					//����DC,��ʾ������������
	OLED_SPI_SendByte(Command);		//д��ָ������
	OLED_W_CS(1);					//����CS,����ͨ��
}

/**
  * ��    ��:OLEDд����
  * ��    ��:Data Ҫд�����ݵ���ʼ��ַ
  * ��    ��:Count Ҫд�����ݵ�����
  * �� �� ֵ:��
  */
void OLED_WriteData(u8* Data, u8 Count)
{
	u8 i;

	OLED_W_CS(0);					//����CS,��ʼͨ��
	OLED_W_DC(1);					//����DC,��ʾ������������
	/*ѭ��Count��,��������������д��*/
	for (i = 0; i < Count; i++)
	{
		OLED_SPI_SendByte(Data[i]);	//���η���Data��ÿһ������
	}
	OLED_W_CS(1);					//����CS,����ͨ��
}

/*********************ͨ��Э��*/


/*Ӳ������*********************/

/**
  * ��    ��:OLED��ʼ��
  * ��    ��:��
  * �� �� ֵ:��
  * ˵    ��:ʹ��ǰ,��Ҫ���ô˳�ʼ������
  */
void OLED_Init(void)
{
	OLED_GPIO_Init();			//�ȵ��õײ�Ķ˿ڳ�ʼ��

	/*д��һϵ�е�����,��OLED���г�ʼ������*/
	OLED_WriteCommand(0xAE);	//������ʾ����/�ر�,0xAE�ر�,0xAF����

	OLED_WriteCommand(0xD5);	//������ʾʱ�ӷ�Ƶ��/����Ƶ��
	OLED_WriteCommand(0xFE);	//0x00~0xFF

	OLED_WriteCommand(0xA8);	//���ö�·������
	OLED_WriteCommand(0x3F);	//0x0E~0x3F

	OLED_WriteCommand(0xD3);	//������ʾƫ��
	OLED_WriteCommand(0x00);	//0x00~0x7F

	OLED_WriteCommand(0x40);	//������ʾ��ʼ��,0x40~0x7F

	OLED_WriteCommand(0xA1);	//�������ҷ���,0xA1����,0xA0���ҷ���

	OLED_WriteCommand(0xC8);	//�������·���,0xC8����,0xC0���·���

	OLED_WriteCommand(0xDA);	//����COM����Ӳ������
	OLED_WriteCommand(0x12);

	OLED_WriteCommand(0x81);	//���öԱȶ�
	// OLED_WriteCommand(0x00);	//0x00~0xFF
	OLED_WriteCommand(0xCF);	//0x00~0xFF

	OLED_WriteCommand(0xD9);	//����Ԥ�������
	OLED_WriteCommand(0xF2);

	OLED_WriteCommand(0xDB);	//����VCOMHȡ��ѡ�񼶱�
	OLED_WriteCommand(0x30);

	OLED_WriteCommand(0xA4);	//����������ʾ��/�ر�

	OLED_WriteCommand(0xA6);	//��������/��ɫ��ʾ,0xA6����,0xA7��ɫ

	OLED_WriteCommand(0xAD);	//���ó���
	OLED_WriteCommand(0x8B);    //0x8BΪ����,0x8AΪ�رճ���

	OLED_WriteCommand(0xAF);	//������ʾ,0xAEΪ����

	OLED_Clear();				//����Դ�����
	OLED_Update();				//������ʾ,����,��ֹ��ʼ����δ��ʾ����ʱ����
}

/**
  * ��    ��:OLED������
  * ��    ��:Data ָ��OLED������ģʽ 0���� 1����
  * �� �� ֵ:��
  * ˵    ��:���ô˺�����OLED��Ļ����͹��Ĵ���ģʽ��Ӵ�������
  */
void OLED_Power(u8 Data)
{
	if (Data)
	{
		OLED_WriteCommand(0xAD);	//���ó���
		OLED_WriteCommand(0x8B);    //0x8B��������	

		OLED_WriteCommand(0xAF);	//0xAF������ʾ
	}
	else
	{
		OLED_WriteCommand(0xAE);	//0xAE�ر���ʾ

		OLED_WriteCommand(0xAD);	//���ó���
		OLED_WriteCommand(0x8A);    //0x8A�رճ���
	}
}

/**
  * ��    ��:OLED������ʾ���λ��
  * ��    ��:Page ָ��������ڵ�ҳ,��Χ:0~7
  * ��    ��:X ָ��������ڵ�X������,��Χ:0~127
  * �� �� ֵ:��
  * ˵    ��:OLEDĬ�ϵ�Y��,ֻ��8��BitΪһ��д��,��1ҳ����8��Y������
  */
void OLED_SetCursor(u8 Page, u8 X)
{
	/*���ʹ�ô˳�������1.3���OLED��ʾ��,����Ҫ�����ע��*/
	/*��Ϊ1.3���OLED����оƬ(SH1106)��132��*/
	/*��Ļ����ʼ�н����˵�2��,�����ǵ�0��*/
	/*������Ҫ��X��2,����������ʾ*/
	X += 2;

	/*ͨ��ָ������ҳ��ַ���е�ַ*/
	OLED_WriteCommand(0xB0 | Page);					//����ҳλ��
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	//����Xλ�ø�4λ
	OLED_WriteCommand(0x00 | (X & 0x0F));			//����Xλ�õ�4λ
}

/*********************Ӳ������*/


/*���ߺ���*********************/

/*���ߺ��������ڲ����ֺ���ʹ��*/

/**
  * ��    ��:�η�����
  * ��    ��:X ����
  * ��    ��:Y ָ��
  * �� �� ֵ:����X��Y�η�
  */
u32 OLED_Pow(u32 X, u32 Y)
{
	u32 Result = 1;	//���Ĭ��Ϊ1
	while (Y--)			//�۳�Y��
	{
		Result *= X;		//ÿ�ΰ�X�۳˵������
	}
	return Result;
}

/**
  * ��    ��:�ж�ָ�����Ƿ���ָ��������ڲ�
  * ��    ��:nvert ����εĶ�����
  * ��    ��:vertx verty ��������ζ����x��y���������
  * ��    ��:testx testy ���Ե��X��y����
  * �� �� ֵ:ָ�����Ƿ���ָ��������ڲ�,1:���ڲ�,0:�����ڲ�
  */
u8 OLED_pnpoly(u8 nvert, int16_t* vertx, int16_t* verty, int16_t testx, int16_t testy)
{
	int16_t i, j, c = 0;

	/*���㷨��W. Randolph Franklin���*/
	/*�ο�����:https://wrfranklin.org/Research/Short_Notes/pnpoly.html*/
	for (i = 0, j = nvert - 1; i < nvert; j = i++)
	{
		if (((verty[i] > testy) != (verty[j] > testy)) &&
			(testx < (vertx[j] - vertx[i]) * (testy - verty[i]) / (verty[j] - verty[i]) + vertx[i]))
		{
			c = !c;
		}
	}
	return c;
}

/**
  * ��    ��:�ж�ָ�����Ƿ���ָ���Ƕ��ڲ�
  * ��    ��:X Y ָ���������
  * ��    ��:StartAngle EndAngle ��ʼ�ǶȺ���ֹ�Ƕ�,��Χ:-180~180
  *           ˮƽ����Ϊ0��,ˮƽ����Ϊ180�Ȼ�-180��,�·�Ϊ����,�Ϸ�Ϊ����,˳ʱ����ת
  * �� �� ֵ:ָ�����Ƿ���ָ���Ƕ��ڲ�,1:���ڲ�,0:�����ڲ�
  */
u8 OLED_IsInAngle(int16_t X, int16_t Y, int16_t StartAngle, int16_t EndAngle)
{
	int16_t PointAngle;
	PointAngle = atan2(Y, X) / 3.14 * 180;	//����ָ����Ļ���,��ת��Ϊ�Ƕȱ�ʾ
	if (StartAngle < EndAngle)	//��ʼ�Ƕ�С����ֹ�Ƕȵ����
	{
		/*���ָ���Ƕ�����ʼ��ֹ�Ƕ�֮��,���ж�ָ������ָ���Ƕ�*/
		if (PointAngle >= StartAngle && PointAngle <= EndAngle)
		{
			return 1;
		}
	}
	else			//��ʼ�Ƕȴ�������ֹ�Ƕȵ����
	{
		/*���ָ���Ƕȴ�����ʼ�ǶȻ���С����ֹ�Ƕ�,���ж�ָ������ָ���Ƕ�*/
		if (PointAngle >= StartAngle || PointAngle <= EndAngle)
		{
			return 1;
		}
	}
	return 0;		//��������������,���ж��ж�ָ���㲻��ָ���Ƕ�
}

/*********************���ߺ���*/


/*���ܺ���*********************/

/**
  * ��    ��:��OLED�Դ�������µ�OLED��Ļ
  * ��    ��:��
  * �� �� ֵ:��
  * ˵    ��:���е���ʾ����,��ֻ�Ƕ�OLED�Դ�������ж�д
  *           ������OLED_Update������OLED_UpdateArea����
  *           �ŻὫ�Դ���������ݷ��͵�OLEDӲ��,������ʾ
  *           �ʵ�����ʾ������,Ҫ�������س�������Ļ��,������ø��º���
  */
void OLED_Update(void)
{
	u8 j;
	/*����ÿһҳ*/
	for (j = 0; j < 8; j++)
	{
		/*���ù��λ��Ϊÿһҳ�ĵ�һ��*/
		OLED_SetCursor(j, 0);
		/*����д��128������,���Դ����������д�뵽OLEDӲ��*/
		OLED_WriteData(OLED_DisplayBuf[j], 128);
	}
}

/**
  * ��    ��:��OLED�Դ����鲿�ָ��µ�OLED��Ļ
  * ��    ��:X ָ���������Ͻǵĺ�����,��Χ:0~127
  * ��    ��:Y ָ���������Ͻǵ�������,��Χ:0~63
  * ��    ��:Width ָ������Ŀ���,��Χ:0~128
  * ��    ��:Height ָ������ĸ߶�,��Χ:0~64
  * �� �� ֵ:��
  * ˵    ��:�˺��������ٸ��²���ָ��������
  *           �����������Y��ֻ��������ҳ,��ͬһҳ��ʣ�ಿ�ֻ����һ�����
  * ˵    ��:���е���ʾ����,��ֻ�Ƕ�OLED�Դ�������ж�д
  *           ������OLED_Update������OLED_UpdateArea����
  *           �ŻὫ�Դ���������ݷ��͵�OLEDӲ��,������ʾ
  *           �ʵ�����ʾ������,Ҫ�������س�������Ļ��,������ø��º���
  */
void OLED_UpdateArea(u8 X, u8 Y, u8 Width, u8 Height)
{
	u8 j;

	/*�������,��ָ֤�����򲻻ᳬ����Ļ��Χ*/
	if (X > 127) { return; }
	if (Y > 63) { return; }
	if (X + Width > 128) { Width = 128 - X; }
	if (Y + Height > 64) { Height = 64 - Y; }

	/*����ָ�������漰�����ҳ*/
	/*(Y + Height - 1) / 8 + 1��Ŀ����(Y + Height) / 8������ȡ��*/
	for (j = Y / 8; j < (Y + Height - 1) / 8 + 1; j++)
	{
		/*���ù��λ��Ϊ���ҳ��ָ����*/
		OLED_SetCursor(j, X);
		/*����д��Width������,���Դ����������д�뵽OLEDӲ��*/
		OLED_WriteData(&OLED_DisplayBuf[j][X], Width);
	}
}

/**
  * ��    ��:��OLED�Դ�����ȫ������
  * ��    ��:��
  * �� �� ֵ:��
  * ˵    ��:���ô˺�����,Ҫ�������س�������Ļ��,������ø��º���
  */
void OLED_Clear(void)
{
	u8 i, j;
	for (j = 0; j < 8; j++)				//����8ҳ
	{
		for (i = 0; i < 128; i++)			//����128��
		{
			OLED_DisplayBuf[j][i] = 0x00;	//���Դ���������ȫ������
		}
	}
}

/**
  * ��    ��:��OLED�Դ����鲿������
  * ��    ��:X ָ���������Ͻǵĺ�����,��Χ:0~127
  * ��    ��:Y ָ���������Ͻǵ�������,��Χ:0~63
  * ��    ��:Width ָ������Ŀ���,��Χ:0~128
  * ��    ��:Height ָ������ĸ߶�,��Χ:0~64
  * �� �� ֵ:��
  * ˵    ��:���ô˺�����,Ҫ�������س�������Ļ��,������ø��º���
  */
void OLED_ClearArea(u8 X, u8 Y, u8 Width, u8 Height)
{
	u8 i, j;

	/*�������,��ָ֤�����򲻻ᳬ����Ļ��Χ*/
	if (X > 127) { return; }
	if (Y > 63) { return; }
	if (X + Width > 128) { Width = 128 - X; }
	if (Y + Height > 64) { Height = 64 - Y; }

	for (j = Y; j < Y + Height; j++)		//����ָ��ҳ
	{
		for (i = X; i < X + Width; i++)	//����ָ����
		{
			OLED_DisplayBuf[j / 8][i] &= ~(0x01 << (j % 8));	//���Դ�����ָ����������
		}
	}
}

/**
  * ��    ��:��OLED�Դ�����ȫ��ȡ��
  * ��    ��:��
  * �� �� ֵ:��
  * ˵    ��:���ô˺�����,Ҫ�������س�������Ļ��,������ø��º���
  */
void OLED_Reverse(void)
{
	u8 i, j;
	for (j = 0; j < 8; j++)				//����8ҳ
	{
		for (i = 0; i < 128; i++)			//����128��
		{
			OLED_DisplayBuf[j][i] ^= 0xFF;	//���Դ���������ȫ��ȡ��
		}
	}
}

/**
  * ��    ��:��OLED�Դ����鲿��ȡ��
  * ��    ��:X ָ���������Ͻǵĺ�����,��Χ:0~127
  * ��    ��:Y ָ���������Ͻǵ�������,��Χ:0~63
  * ��    ��:Width ָ������Ŀ���,��Χ:0~128
  * ��    ��:Height ָ������ĸ߶�,��Χ:0~64
  * �� �� ֵ:��
  * ˵    ��:���ô˺�����,Ҫ�������س�������Ļ��,������ø��º���
  */
void OLED_ReverseArea(u8 X, u8 Y, u8 Width, u8 Height)
{
	u8 i, j;

	/*�������,��ָ֤�����򲻻ᳬ����Ļ��Χ*/
	if (X > 127) { return; }
	if (Y > 63) { return; }
	if (X + Width > 128) { Width = 128 - X; }
	if (Y + Height > 64) { Height = 64 - Y; }

	for (j = Y; j < Y + Height; j++)		//����ָ��ҳ
	{
		for (i = X; i < X + Width; i++)	//����ָ����
		{
			OLED_DisplayBuf[j / 8][i] ^= 0x01 << (j % 8);	//���Դ�����ָ������ȡ��
		}
	}
}

/**
  * ��    ��:OLED��ʾһ���ַ�
  * ��    ��:X ָ���ַ����Ͻǵĺ�����,��Χ:0~127
  * ��    ��:Y ָ���ַ����Ͻǵ�������,��Χ:0~63
  * ��    ��:Char ָ��Ҫ��ʾ���ַ�,��Χ:ASCII��ɼ��ַ�
  * ��    ��:FontSize ָ�������С
  *           ��Χ:OLED_8X16		��8����,��16����
  *                 OLED_6X8		��6����,��8����
  * �� �� ֵ:��
  * ˵    ��:���ô˺�����,Ҫ�������س�������Ļ��,������ø��º���
  */
void OLED_ShowChar(int8_t X, int8_t Y, char Char, u8 FontSize)
{
	if (FontSize == OLED_8X16)		//����Ϊ��8����,��16����
	{
		/*��ASCII��ģ��OLED_F8x16��ָ��������8*16��ͼ���ʽ��ʾ*/
		OLED_ShowImage(X, Y, 8, 16, OLED_F8x16[Char - ' ']);
	}
	else if (FontSize == OLED_6X8)	//����Ϊ��6����,��8����
	{
		/*��ASCII��ģ��OLED_F6x8��ָ��������6*8��ͼ���ʽ��ʾ*/
		OLED_ShowImage(X, Y, 6, 8, OLED_F6x8[Char - ' ']);
	}
}

/**
  * ��    ��:OLED��ʾ���ֵ���
  * ��    ��:X ָ���ַ������Ͻǵĺ�����,��Χ:0~127
  * ��    ��:Y ָ���ַ������Ͻǵ�������,��Χ:0~63
  * ��    ��:Hanzi ָ��Ҫ��ʾ���ַ�,��Χ:�ֿ��ַ�
  * ��    ��:FontSize ָ�������С
  *           ��Χ:OLED_8X16		��8����,��16����
  *                 OLED_6X8		��6����,��8����
  * �� �� ֵ:��
  * ˵    ��:���ô˺�����,Ҫ�������س�������Ļ��,������ø��º���
  */
void OLED_MyShowChinese(int8_t X, int8_t Y, char* Hanzi, u8 FontSize)	//���ֵ��ִ�ӡ;
{
	u8 pIndex;
	for (pIndex = 0; strcmp(OLED_CF12x12[pIndex].Index, "") != 0; pIndex++)
	{
		/*�ҵ�ƥ��ĺ���*/
		if (strcmp(OLED_CF12x12[pIndex].Index, Hanzi) == 0)
		{
			break;		//����ѭ��,��ʱpIndex��ֵΪָ�����ֵ�����
		}
	}
	/*��������ģ��OLED_CF16x16��ָ��������16*16��ͼ���ʽ��ʾ*/
	OLED_ShowImage(X, Y, 12, 12, OLED_CF12x12[pIndex].Data);
}

/**
  * ��    ��:OLED��ʾ�ַ���
  * ��    ��:X ָ���ַ������Ͻǵĺ�����,��Χ:0~127
  * ��    ��:Y ָ���ַ������Ͻǵ�������,��Χ:0~63
  * ��    ��:String ָ��Ҫ��ʾ���ַ���,��Χ:ASCII��ɼ��ַ���ɵ��ַ���
  * ��    ��:FontSize ָ�������С
  *           ��Χ:OLED_8X16		��8����,��16����
  *                 OLED_6X8		��6����,��8����
  * �� �� ֵ:��
  * ˵    ��:���ô˺�����,Ҫ�������س�������Ļ��,������ø��º���
  */
void OLED_ShowASCII(int8_t X, int8_t Y, char* String, u8 FontSize)
{
	u8 i;
	for (i = 0; String[i] != '\0'; i++)		//�����ַ�����ÿ���ַ�
	{
		/*����OLED_ShowChar����,������ʾÿ���ַ�*/
		OLED_ShowChar(X + i * FontSize, Y, String[i], FontSize);
	}
}




/**
  * ��    ��:OLED��ʾ�ַ���(������)
  * ��    ��:X ָ���ַ������Ͻǵĺ�����,��Χ:0~127
  * ��    ��:Y ָ���ַ������Ͻǵ�������,��Χ:0~63
  * ��    ��:String ָ��Ҫ��ʾ���ַ���
  * ��    ��:FontSize ָ�������С
  *           ��Χ:OLED_8X16		��8����,��16����
  *                 OLED_6X8		��6����,��8����
  * �� �� ֵ:��
  * ˵    ��:���ô˺�����,Ҫ�������س�������Ļ��,������ø��º���
  */
void OLED_ShowString(int8_t X, int8_t Y, char* String, u8 FontSize)	//��Ӣ�Ĵ�ӡ;
{

	u8 i, len;
	for (i = 0, len = 0; String[i] != '\0'; i++, len++)		//�����ַ�����ÿ���ַ�
	{
		if (String[i] == '\n') { Y += (FontSize == 8) ? 16 : 8; len = 255; continue; }			//���ݻ��з�
		if (X + (len + 1) * FontSize > 128) { Y += (FontSize == 8) ? 16 : 8; len = 0; }

		if (String[i] > '~')				//���������Ӣ���ַ�
		{
			char SingleChinese[OLED_CHN_CHAR_WIDTH + 1] = { 0 };
			SingleChinese[0] = String[i];
			i++;
			SingleChinese[1] = String[i];
			if (OLED_CHN_CHAR_WIDTH == 3)
			{
				i++;
				SingleChinese[2] = String[i];
			}
			if (FontSize == 8) { OLED_MyShowChinese(X + len * FontSize, Y, SingleChinese, FontSize); }
			else { OLED_MyShowChinese(X + len * FontSize, Y, SingleChinese, FontSize); }
			len += 1;
		}
		else
		{
			/*����OLED_ShowChar����,������ʾÿ���ַ�*/
			OLED_ShowChar(X + len * FontSize, Y + 4, String[i], FontSize);
		}
	}                  //�����4��Ϊ���ú�������ĸ�ײ���ͬһˮƽ��
}




/**
  * ��    ��:OLED��ʾ����(ʮ����,������)
  * ��    ��:X ָ���������Ͻǵĺ�����,��Χ:0~127
  * ��    ��:Y ָ���������Ͻǵ�������,��Χ:0~63
  * ��    ��:Number ָ��Ҫ��ʾ������,��Χ:0~4294967295
  * ��    ��:Length ָ�����ֵĳ���,��Χ:0~10
  * ��    ��:FontSize ָ�������С
  *           ��Χ:OLED_8X16		��8����,��16����
  *                 OLED_6X8		��6����,��8����
  * �� �� ֵ:��
  * ˵    ��:���ô˺�����,Ҫ�������س�������Ļ��,������ø��º���
  */
void OLED_ShowNum(int8_t X, int8_t Y, u32 Number, u8 Length, u8 FontSize)
{
	u8 i;
	if (X > 127) { return; }
	if (Y > 64) { return; }

	for (i = 0; i < Length; i++)		//�������ֵ�ÿһλ							
	{
		/*����OLED_ShowChar����,������ʾÿ������*/
		/*Number / OLED_Pow(10, Length - i - 1) % 10 ����ʮ������ȡ���ֵ�ÿһλ*/
		/*+ '0' �ɽ�����ת��Ϊ�ַ���ʽ*/
		OLED_ShowChar(X + i * FontSize, Y, Number / OLED_Pow(10, Length - i - 1) % 10 + '0', FontSize);
	}
}

/**
  * ��    ��:OLED��ʾ�з�������(ʮ����,����)
  * ��    ��:X ָ���������Ͻǵĺ�����,��Χ:0~127
  * ��    ��:Y ָ���������Ͻǵ�������,��Χ:0~63
  * ��    ��:Number ָ��Ҫ��ʾ������,��Χ:-2147483648~2147483647
  * ��    ��:Length ָ�����ֵĳ���,��Χ:0~10
  * ��    ��:FontSize ָ�������С
  *           ��Χ:OLED_8X16		��8����,��16����
  *                 OLED_6X8		��6����,��8����
  * �� �� ֵ:��
  * ˵    ��:���ô˺�����,Ҫ�������س�������Ļ��,������ø��º���
  */
void OLED_ShowSignedNum(u8 X, u8 Y, int32_t Number, u8 Length, u8 FontSize)
{
	u8 i;
	u32 Number1;

	if (Number >= 0)						//���ִ��ڵ���0
	{
		OLED_ShowChar(X, Y, '+', FontSize);	//��ʾ+��
		Number1 = Number;					//Number1ֱ�ӵ���Number
	}
	else									//����С��0
	{
		OLED_ShowChar(X, Y, '-', FontSize);	//��ʾ-��
		Number1 = -Number;					//Number1����Numberȡ��
	}

	for (i = 0; i < Length; i++)			//�������ֵ�ÿһλ								
	{
		/*����OLED_ShowChar����,������ʾÿ������*/
		/*Number1 / OLED_Pow(10, Length - i - 1) % 10 ����ʮ������ȡ���ֵ�ÿһλ*/
		/*+ '0' �ɽ�����ת��Ϊ�ַ���ʽ*/
		OLED_ShowChar(X + (i + 1) * FontSize, Y, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0', FontSize);
	}
}

/**
  * ��    ��:OLED��ʾʮ����������(ʮ������,������)
  * ��    ��:X ָ���������Ͻǵĺ�����,��Χ:0~127
  * ��    ��:Y ָ���������Ͻǵ�������,��Χ:0~63
  * ��    ��:Number ָ��Ҫ��ʾ������,��Χ:0x00000000~0xFFFFFFFF
  * ��    ��:Length ָ�����ֵĳ���,��Χ:0~8
  * ��    ��:FontSize ָ�������С
  *           ��Χ:OLED_8X16		��8����,��16����
  *                 OLED_6X8		��6����,��8����
  * �� �� ֵ:��
  * ˵    ��:���ô˺�����,Ҫ�������س�������Ļ��,������ø��º���
  */
void OLED_ShowHexNum(u8 X, u8 Y, u32 Number, u8 Length, u8 FontSize)
{
	u8 i, SingleNumber;
	for (i = 0; i < Length; i++)		//�������ֵ�ÿһλ
	{
		/*��ʮ��������ȡ���ֵ�ÿһλ*/
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;

		if (SingleNumber < 10)			//��������С��10
		{
			/*����OLED_ShowChar����,��ʾ������*/
			/*+ '0' �ɽ�����ת��Ϊ�ַ���ʽ*/
			OLED_ShowChar(X + i * FontSize, Y, SingleNumber + '0', FontSize);
		}
		else							//�������ִ���10
		{
			/*����OLED_ShowChar����,��ʾ������*/
			/*+ 'A' �ɽ�����ת��Ϊ��A��ʼ��ʮ�������ַ�*/
			OLED_ShowChar(X + i * FontSize, Y, SingleNumber - 10 + 'A', FontSize);
		}
	}
}

/**
  * ��    ��:OLED��ʾ����������(������,������)
  * ��    ��:X ָ���������Ͻǵĺ�����,��Χ:0~127
  * ��    ��:Y ָ���������Ͻǵ�������,��Χ:0~63
  * ��    ��:Number ָ��Ҫ��ʾ������,��Χ:0x00000000~0xFFFFFFFF
  * ��    ��:Length ָ�����ֵĳ���,��Χ:0~16
  * ��    ��:FontSize ָ�������С
  *           ��Χ:OLED_8X16		��8����,��16����
  *                 OLED_6X8		��6����,��8����
  * �� �� ֵ:��
  * ˵    ��:���ô˺�����,Ҫ�������س�������Ļ��,������ø��º���
  */
void OLED_ShowBinNum(u8 X, u8 Y, u32 Number, u8 Length, u8 FontSize)
{
	u8 i;
	for (i = 0; i < Length; i++)		//�������ֵ�ÿһλ	
	{
		/*����OLED_ShowChar����,������ʾÿ������*/
		/*Number / OLED_Pow(2, Length - i - 1) % 2 ���Զ�������ȡ���ֵ�ÿһλ*/
		/*+ '0' �ɽ�����ת��Ϊ�ַ���ʽ*/
		OLED_ShowChar(X + i * FontSize, Y, Number / OLED_Pow(2, Length - i - 1) % 2 + '0', FontSize);
	}
}

/**
  * ��    ��:OLED��ʾ��������(ʮ����,С��)
  * ��    ��:X ָ���������Ͻǵĺ�����,��Χ:0~127
  * ��    ��:Y ָ���������Ͻǵ�������,��Χ:0~63
  * ��    ��:Number ָ��Ҫ��ʾ������,��Χ:-4294967295.0~4294967295.0
  * ��    ��:IntLength ָ�����ֵ�����λ����,��Χ:0~10
  * ��    ��:FraLength ָ�����ֵ�С��λ����,��Χ:0~9,С����������������ʾ
  * ��    ��:FontSize ָ�������С
  *           ��Χ:OLED_8X16		��8����,��16����
  *                 OLED_6X8		��6����,��8����
  * �� �� ֵ:��
  * ˵    ��:���ô˺�����,Ҫ�������س�������Ļ��,������ø��º���
  */
void OLED_ShowFloatNum(u8 X, u8 Y, double Number, u8 IntLength, u8 FraLength, u8 FontSize)
{
	u32 PowNum, IntNum, FraNum;

	if (Number >= 0)						//���ִ��ڵ���0
	{
		OLED_ShowChar(X, Y, '+', FontSize);	//��ʾ+��
	}
	else									//����С��0
	{
		OLED_ShowChar(X, Y, '-', FontSize);	//��ʾ-��
		Number = -Number;					//Numberȡ��
	}

	/*��ȡ�������ֺ�С������*/
	IntNum = Number;						//ֱ�Ӹ�ֵ�����ͱ���,��ȡ����
	Number -= IntNum;						//��Number����������,��ֹ֮��С���˵�����ʱ����������ɴ���
	PowNum = OLED_Pow(10, FraLength);		//����ָ��С����λ��,ȷ������
	FraNum = round(Number * PowNum);		//��С���˵�����,ͬʱ��������,������ʾ���
	IntNum += FraNum / PowNum;				//��������������˽�λ,����Ҫ�ټӸ�����

	/*��ʾ��������*/
	OLED_ShowNum(X + FontSize, Y, IntNum, IntLength, FontSize);

	/*��ʾС����*/
	OLED_ShowChar(X + (IntLength + 1) * FontSize, Y, '.', FontSize);

	/*��ʾС������*/
	OLED_ShowNum(X + (IntLength + 2) * FontSize, Y, FraNum, FraLength, FontSize);
}

/**
  * ��    ��:OLED��ʾ���ִ�
  * ��    ��:X ָ�����ִ����Ͻǵĺ�����,��Χ:0~127
  * ��    ��:Y ָ�����ִ����Ͻǵ�������,��Χ:0~63
  * ��    ��:Chinese ָ��Ҫ��ʾ�ĺ��ִ�,��Χ:����ȫ��Ϊ���ֻ���ȫ���ַ�,��Ҫ�����κΰ���ַ�
  *           ��ʾ�ĺ�����Ҫ��OLED_Data.c���OLED_CF16x16���鶨��
  *           δ�ҵ�ָ������ʱ,����ʾĬ��ͼ��(һ������,�ڲ�һ���ʺ�)
  * �� �� ֵ:��
  * ˵    ��:���ô˺�����,Ҫ�������س�������Ļ��,������ø��º���
  */
void OLED_ShowChinese(u8 X, u8 Y, char* Chinese)
{
	u8 pChinese = 0;
	u8 pIndex;
	u8 i;
	char SingleChinese[OLED_CHN_CHAR_WIDTH + 1] = { 0 };

	for (i = 0; Chinese[i] != '\0'; i++)		//�������ִ�
	{
		SingleChinese[pChinese] = Chinese[i];	//��ȡ���ִ����ݵ�������������
		pChinese++;							//�ƴ�����

		/*����ȡ��������OLED_CHN_CHAR_WIDTHʱ,��������ȡ����һ�������ĺ���*/
		if (pChinese >= OLED_CHN_CHAR_WIDTH)
		{
			pChinese = 0;		//�ƴι���

			/*��������������ģ��,Ѱ��ƥ��ĺ���*/
			/*����ҵ����һ������(����Ϊ���ַ���),���ʾ����δ����ģ�ⶨ��,ֹͣѰ��*/
			for (pIndex = 0; strcmp(OLED_CF12x12[pIndex].Index, "") != 0; pIndex++)
			{
				/*�ҵ�ƥ��ĺ���*/
				if (strcmp(OLED_CF12x12[pIndex].Index, SingleChinese) == 0)
				{
					break;		//����ѭ��,��ʱpIndex��ֵΪָ�����ֵ�����
				}
			}

			/*��������ģ��OLED_CF16x16��ָ��������16*16��ͼ���ʽ��ʾ*/
			OLED_ShowImage(X + ((i + 1) / OLED_CHN_CHAR_WIDTH - 1) * 16, Y, 12, 12, OLED_CF12x12[pIndex].Data);
		}
	}
}

/**
  * ��    ��:OLED��ʾͼ��
  * ��    ��:X ָ��ͼ�����Ͻǵĺ�����,��Χ:0~127
  * ��    ��:Y ָ��ͼ�����Ͻǵ�������,��Χ:0~63
  * ��    ��:Width ָ��ͼ��Ŀ���,��Χ:0~128
  * ��    ��:Height ָ��ͼ��ĸ߶�,��Χ:0~64
  * ��    ��:Image ָ��Ҫ��ʾ��ͼ��
  * �� �� ֵ:��
  * ˵    ��:���ô˺�����,Ҫ�������س�������Ļ��,������ø��º���
  */
void OLED_ShowImage(int8_t X, int8_t Y0, u8 Width, u8 Height, const u8* Image)
{
	int8_t Y = (int8_t)Y0; // XYֵΪ����ʱ,ֻҪͼƬ���в�������Ļ����ʾ,�ͻ����������;

	u8  i, j, p = 1;
	/*�������,��ָ֤��ͼ�񲻻ᳬ����Ļ��Χ*/
	if (X + Width <= 0) { return; }        // ���û��ͼƬ����Ļ��Χ�����ӡ������,�������ӡ	
	else if (X > 127) { return; }            // ���X����127���ӡ������,�������ӡ

	u8 k = 0, l = 0, temp_Height; //(����YΪ������ر���)
	if (Y < 0)						   // ���YС��0, �����ӡͼƬλ����Ļ�ڵĲ���;
	{
		temp_Height = Height; // ��¼ͼƬ�߶�
		Height += Y;		  // ͼƬ�ĸ� ��ȥY�����ֵ
		k = (-Y - 1) / 8 + 1; // ͼƬ��Byte�е��±�ƫ����;
		l = (-Y - 1) % 8 + 1; // Byteλ�ƻ�	//(8 + Y % 8)%8;//		OLED_ShowNum(0, 56, l, 3, 6);
		Y = 0;				  // ͼƬ��Ȼ����ĻY0��ʼ��ӡ
		if ((int8_t)Height < 1)
		{
			return;
		} // ���ӡ��ͼƬ��С��1ʱ,˵������Ҫ��ӡ;
	}
	else if (Y > 63)
	{
		return;
	} // ���Y����63���ӡ������,�������ӡ

	/*��ͼ�������������*/
	OLED_ClearArea(X, Y, Width, Height);

	/*����ָ��ͼ���漰�����ҳ*/
	/*(Height - 1) / 8 + 1��Ŀ����Height / 8������ȡ��*/
	u8 Height_ceil = (Height - 1) / 8 + 1;

	for (j = 0; j < Height_ceil; j++) // ��OLED_DisplayBuf���±����,(ͼ���漰�����ҳ)
	{
		p = 1;
		/*����ָ��ͼ���漰�������*/
		for (i = 0; i < Width; i++)
		{
			if (p)
			{
				if (X < 0)						   // ���XС��0, �����ӡͼƬλ����Ļ�ڵĲ���;
				{
					i = i + (0 - X);                     //ֱ������Ҫ��ʾ����
					p = 0;
				}
			}

			/*�����߽�,��������ʾ*/
			if (X + i > 127)
			{
				break;
			}
			if (Y / 8 + j > 7)
			{
				return;
			}

			if (k) // ��һ��OLED_DisplayBuf���±�Ϊ����, ȡ����ͼ��Byte, ����λ�ƻ�����;
			{
				/*��ʾ��һByteͼ���ڵ�ǰҳ������*/
				OLED_DisplayBuf[Y / 8 + j][X + i] |= Image[(j + k - 1) * Width + i] >> ((l));

				/*�����߽�,��������ʾ*/
				/*ʹ��continue��Ŀ����,��ǰByte����ͼƬʱ,��һByte�ĺ������ݻ���Ҫ������ʾ*/
				if ((j + k) * 8 >= temp_Height)
				{
					continue;
				} // �����ǰ��Byte�Ѿ�������ͼ��,�򲻷���;

				/*��ʾ��ǰByteͼ���ڵ�ǰҳ������*/
				OLED_DisplayBuf[Y / 8 + j][X + i] |= Image[(j + k) * Width + i] << (8 - (l));
			}

			else // ��һ��ͼ��ByteΪ����, ��������OLED_DisplayBuf��֮��;
			{
				/*��ʾͼ���ڵ�ǰҳ������*/
				OLED_DisplayBuf[Y / 8 + j][X + i] |= Image[(j)*Width + i] << ((Y) % 8);

				/*�����߽�,��������ʾ*/
				/*ʹ��continue��Ŀ����,��һҳ�����߽�ʱ,��һҳ�ĺ������ݻ���Ҫ������ʾ*/
				if (Y / 8 + j + 1 > 7)
				{
					continue;
				}

				/*��ʾͼ������һҳ������*/
				OLED_DisplayBuf[Y / 8 + j + 1][X + i] |= Image[(j)*Width + i] >> (8 - (Y) % 8);
			}
		}
	}
}

/**
  * ��    ��:OLEDʹ��printf������ӡ��ʽ���ַ���
  * ��    ��:X ָ����ʽ���ַ������Ͻǵĺ�����,��Χ:0~127
  * ��    ��:Y ָ����ʽ���ַ������Ͻǵ�������,��Χ:0~63
  * ��    ��:FontSize ָ�������С
  *           ��Χ:OLED_8X16		��8����,��16����
  *                 OLED_6X8		��6����,��8����
  * ��    ��:format ָ��Ҫ��ʾ�ĸ�ʽ���ַ���,��Χ:ASCII��ɼ��ַ���ɵ��ַ���
  * ��    ��:... ��ʽ���ַ��������б�
  * �� �� ֵ:��
  * ˵    ��:���ô˺�����,Ҫ�������س�������Ļ��,������ø��º���
  */
void OLED_Printf(u8 X, u8 Y, u8 FontSize, char* format, ...)
{
	char String[30];						//�����ַ�����
	va_list arg;							//����ɱ�����б��������͵ı���arg
	va_start(arg, format);					//��format��ʼ,���ղ����б���arg����
	vsprintf(String, format, arg);			//ʹ��vsprintf��ӡ��ʽ���ַ����Ͳ����б����ַ�������
	va_end(arg);							//��������arg
	OLED_ShowString(X, Y, String, FontSize);//OLED��ʾ�ַ�����(�ַ���)
}

/**
  * ��    ��:OLED��ָ��λ�û�һ����
  * ��    ��:X ָ����ĺ�����,��Χ:0~127
  * ��    ��:Y ָ�����������,��Χ:0~63
  * �� �� ֵ:��
  * ˵    ��:���ô˺�����,Ҫ�������س�������Ļ��,������ø��º���
  */
void OLED_DrawPoint(int8_t X, int8_t Y)
{
	/*�������,��ָ֤��λ�ò��ᳬ����Ļ��Χ*/
	if (X > 127) { return; }
	if (Y > 63) { return; }
	if (X < 0) { return; }
	if (Y < 0) { return; }


	/*���Դ�����ָ��λ�õ�һ��Bit������1*/
	OLED_DisplayBuf[Y / 8][X] |= 0x01 << (Y % 8);
}

/**
  * ��    ��:OLED��ȡָ��λ�õ��ֵ
  * ��    ��:X ָ����ĺ�����,��Χ:0~127
  * ��    ��:Y ָ�����������,��Χ:0~63
  * �� �� ֵ:ָ��λ�õ��Ƿ��ڵ���״̬,1:����,0:Ϩ��
  */
u8 OLED_GetPoint(u8 X, u8 Y)
{
	/*�������,��ָ֤��λ�ò��ᳬ����Ļ��Χ*/
	if (X > 127) { return 0; }
	if (Y > 63) { return 0; }

	/*�ж�ָ��λ�õ�����*/
	if (OLED_DisplayBuf[Y / 8][X] & 0x01 << (Y % 8))
	{
		return 1;	//Ϊ1,����1
	}

	return 0;		//����,����0
}

/**
  * ��    ��:OLED����
  * ��    ��:X0 ָ��һ���˵�ĺ�����,��Χ:0~127
  * ��    ��:Y0 ָ��һ���˵��������,��Χ:0~63
  * ��    ��:X1 ָ����һ���˵�ĺ�����,��Χ:0~127
  * ��    ��:Y1 ָ����һ���˵��������,��Χ:0~63
  * �� �� ֵ:��
  * ˵    ��:���ô˺�����,Ҫ�������س�������Ļ��,������ø��º���
  */
void OLED_DrawLine(u8 X0, u8 Y0, u8 X1, u8 Y1)
{
	int16_t x, y, dx, dy, d, incrE, incrNE, temp;
	int16_t x0 = X0, y0 = Y0, x1 = X1, y1 = Y1;
	u8 yflag = 0, xyflag = 0;

	if (y0 == y1)		//���ߵ�������
	{
		/*0�ŵ�X�������1�ŵ�X����,�򽻻�����X����*/
		if (x0 > x1) { temp = x0; x0 = x1; x1 = temp; }

		/*����X����*/
		for (x = x0; x <= x1; x++)
		{
			OLED_DrawPoint(x, y0);	//���λ���
		}
	}
	else if (x0 == x1)	//���ߵ�������
	{
		/*0�ŵ�Y�������1�ŵ�Y����,�򽻻�����Y����*/
		if (y0 > y1) { temp = y0; y0 = y1; y1 = temp; }

		/*����Y����*/
		for (y = y0; y <= y1; y++)
		{
			OLED_DrawPoint(x0, y);	//���λ���
		}
	}
	else				//б��
	{
		/*ʹ��Bresenham�㷨��ֱ��,���Ա����ʱ�ĸ�������,Ч�ʸ���*/
		/*�ο��ĵ�:https://www.cs.montana.edu/courses/spring2009/425/dslectures/Bresenham.pdf*/
		/*�ο��̳�:https://www.bilibili.com/video/BV1364y1d7Lo*/

		if (x0 > x1)	//0�ŵ�X�������1�ŵ�X����
		{
			/*������������*/
			/*������Ӱ�컭��,���ǻ��߷����ɵ�һ���������������ޱ�Ϊ��һ��������*/
			temp = x0; x0 = x1; x1 = temp;
			temp = y0; y0 = y1; y1 = temp;
		}

		if (y0 > y1)	//0�ŵ�Y�������1�ŵ�Y����
		{
			/*��Y����ȡ��*/
			/*ȡ����Ӱ�컭��,���ǻ��߷����ɵ�һ�������ޱ�Ϊ��һ����*/
			y0 = -y0;
			y1 = -y1;

			/*�ñ�־λyflag,��ס��ǰ�任,�ں���ʵ�ʻ���ʱ,�ٽ����껻����*/
			yflag = 1;
		}

		if (y1 - y0 > x1 - x0)	//����б�ʴ���1
		{
			/*��X������Y���껥��*/
			/*������Ӱ�컭��,���ǻ��߷����ɵ�һ����0~90�ȷ�Χ��Ϊ��һ����0~45�ȷ�Χ*/
			temp = x0; x0 = y0; y0 = temp;
			temp = x1; x1 = y1; y1 = temp;

			/*�ñ�־λxyflag,��ס��ǰ�任,�ں���ʵ�ʻ���ʱ,�ٽ����껻����*/
			xyflag = 1;
		}

		/*����ΪBresenham�㷨��ֱ��*/
		/*�㷨Ҫ��,���߷������Ϊ��һ����0~45�ȷ�Χ*/
		dx = x1 - x0;
		dy = y1 - y0;
		incrE = 2 * dy;
		incrNE = 2 * (dy - dx);
		d = 2 * dy - dx;
		x = x0;
		y = y0;

		/*����ʼ��,ͬʱ�жϱ�־λ,�����껻����*/
		if (yflag && xyflag) { OLED_DrawPoint(y, -x); }
		else if (yflag) { OLED_DrawPoint(x, -y); }
		else if (xyflag) { OLED_DrawPoint(y, x); }
		else { OLED_DrawPoint(x, y); }

		while (x < x1)		//����X���ÿ����
		{
			x++;
			if (d < 0)		//��һ�����ڵ�ǰ�㶫��
			{
				d += incrE;
			}
			else			//��һ�����ڵ�ǰ�㶫����
			{
				y++;
				d += incrNE;
			}

			/*��ÿһ����,ͬʱ�жϱ�־λ,�����껻����*/
			if (yflag && xyflag) { OLED_DrawPoint(y, -x); }
			else if (yflag) { OLED_DrawPoint(x, -y); }
			else if (xyflag) { OLED_DrawPoint(y, x); }
			else { OLED_DrawPoint(x, y); }
		}
	}
}

/**
  * ��    ��:OLED����
  * ��    ��:X ָ���������Ͻǵĺ�����,��Χ:0~127
  * ��    ��:Y ָ���������Ͻǵ�������,��Χ:0~63
  * ��    ��:Width ָ�����εĿ���,��Χ:0~128
  * ��    ��:Height ָ�����εĸ߶�,��Χ:0~64
  * ��    ��:IsFilled ָ�������Ƿ����
  *           ��Χ:OLED_UNFILLED		�����
  *                 OLED_FILLED			���
  * �� �� ֵ:��
  * ˵    ��:���ô˺�����,Ҫ�������س�������Ļ��,������ø��º���
  */
void OLED_DrawRectangle(int8_t X, int8_t Y, u8 Width, u8 Height, u8 IsFilled)
{
	u8 i, j;
	if (!IsFilled)		//ָ�����β����
	{
		/*��������X����,����������������*/
		for (i = X; i < X + Width; i++)
		{
			OLED_DrawPoint(i, Y);
			OLED_DrawPoint(i, Y + Height - 1);
		}
		/*��������Y����,����������������*/
		for (i = Y; i < Y + Height; i++)
		{
			OLED_DrawPoint(X, i);
			OLED_DrawPoint(X + Width - 1, i);
		}
	}
	else				//ָ���������
	{
		/*����X����*/
		for (i = X; i < X + Width; i++)
		{
			/*����Y����*/
			for (j = Y; j < Y + Height; j++)
			{
				/*��ָ�����򻭵�,���������*/
				OLED_DrawPoint(i, j);
			}
		}
	}
}

/**
  * ��    ��:OLED������
  * ��    ��:X0 ָ����һ���˵�ĺ�����,��Χ:0~127
  * ��    ��:Y0 ָ����һ���˵��������,��Χ:0~63
  * ��    ��:X1 ָ���ڶ����˵�ĺ�����,��Χ:0~127
  * ��    ��:Y1 ָ���ڶ����˵��������,��Χ:0~63
  * ��    ��:X2 ָ���������˵�ĺ�����,��Χ:0~127
  * ��    ��:Y2 ָ���������˵��������,��Χ:0~63
  * ��    ��:IsFilled ָ���������Ƿ����
  *           ��Χ:OLED_UNFILLED		�����
  *                 OLED_FILLED			���
  * �� �� ֵ:��
  * ˵    ��:���ô˺�����,Ҫ�������س�������Ļ��,������ø��º���
  */
void OLED_DrawTriangle(u8 X0, u8 Y0, u8 X1, u8 Y1, u8 X2, u8 Y2, u8 IsFilled)
{
	u8 minx = X0, miny = Y0, maxx = X0, maxy = Y0;
	u8 i, j;
	int16_t vx[] = { X0, X1, X2 };
	int16_t vy[] = { Y0, Y1, Y2 };

	if (!IsFilled)			//ָ�������β����
	{
		/*���û��ߺ���,����������ֱ������*/
		OLED_DrawLine(X0, Y0, X1, Y1);
		OLED_DrawLine(X0, Y0, X2, Y2);
		OLED_DrawLine(X1, Y1, X2, Y2);
	}
	else					//ָ�����������
	{
		/*�ҵ���������С��X��Y����*/
		if (X1 < minx) { minx = X1; }
		if (X2 < minx) { minx = X2; }
		if (Y1 < miny) { miny = Y1; }
		if (Y2 < miny) { miny = Y2; }

		/*�ҵ�����������X��Y����*/
		if (X1 > maxx) { maxx = X1; }
		if (X2 > maxx) { maxx = X2; }
		if (Y1 > maxy) { maxy = Y1; }
		if (Y2 > maxy) { maxy = Y2; }

		/*��С�������֮��ľ���Ϊ������Ҫ��������*/
		/*���������������еĵ�*/
		/*����X����*/
		for (i = minx; i <= maxx; i++)
		{
			/*����Y����*/
			for (j = miny; j <= maxy; j++)
			{
				/*����OLED_pnpoly,�ж�ָ�����Ƿ���ָ��������֮��*/
				/*�����,�򻭵�,�������,��������*/
				if (OLED_pnpoly(3, vx, vy, i, j)) { OLED_DrawPoint(i, j); }
			}
		}
	}
}

/**
  * ��    ��:OLED��Բ
  * ��    ��:X ָ��Բ��Բ�ĺ�����,��Χ:0~127
  * ��    ��:Y ָ��Բ��Բ��������,��Χ:0~63
  * ��    ��:Radius ָ��Բ�İ뾶,��Χ:0~255
  * ��    ��:IsFilled ָ��Բ�Ƿ����
  *           ��Χ:OLED_UNFILLED		�����
  *                 OLED_FILLED			���
  * �� �� ֵ:��
  * ˵    ��:���ô˺�����,Ҫ�������س�������Ļ��,������ø��º���
  */
void OLED_DrawCircle(u8 X, u8 Y, u8 Radius, u8 IsFilled)
{
	int16_t x, y, d, j;

	/*ʹ��Bresenham�㷨��Բ,���Ա����ʱ�ĸ�������,Ч�ʸ���*/
	/*�ο��ĵ�:https://www.cs.montana.edu/courses/spring2009/425/dslectures/Bresenham.pdf*/
	/*�ο��̳�:https://www.bilibili.com/video/BV1VM4y1u7wJ*/

	d = 1 - Radius;
	x = 0;
	y = Radius;

	/*��ÿ���˷�֮һԲ������ʼ��*/
	OLED_DrawPoint(X + x, Y + y);
	OLED_DrawPoint(X - x, Y - y);
	OLED_DrawPoint(X + y, Y + x);
	OLED_DrawPoint(X - y, Y - x);

	if (IsFilled)		//ָ��Բ���
	{
		/*������ʼ��Y����*/
		for (j = -y; j < y; j++)
		{
			/*��ָ�����򻭵�,��䲿��Բ*/
			OLED_DrawPoint(X, Y + j);
		}
	}

	while (x < y)		//����X���ÿ����
	{
		x++;
		if (d < 0)		//��һ�����ڵ�ǰ�㶫��
		{
			d += 2 * x + 1;
		}
		else			//��һ�����ڵ�ǰ�㶫�Ϸ�
		{
			y--;
			d += 2 * (x - y) + 1;
		}

		/*��ÿ���˷�֮һԲ���ĵ�*/
		OLED_DrawPoint(X + x, Y + y);
		OLED_DrawPoint(X + y, Y + x);
		OLED_DrawPoint(X - x, Y - y);
		OLED_DrawPoint(X - y, Y - x);
		OLED_DrawPoint(X + x, Y - y);
		OLED_DrawPoint(X + y, Y - x);
		OLED_DrawPoint(X - x, Y + y);
		OLED_DrawPoint(X - y, Y + x);

		if (IsFilled)	//ָ��Բ���
		{
			/*�����м䲿��*/
			for (j = -y; j < y; j++)
			{
				/*��ָ�����򻭵�,��䲿��Բ*/
				OLED_DrawPoint(X + x, Y + j);
				OLED_DrawPoint(X - x, Y + j);
			}

			/*�������ಿ��*/
			for (j = -x; j < x; j++)
			{
				/*��ָ�����򻭵�,��䲿��Բ*/
				OLED_DrawPoint(X - y, Y + j);
				OLED_DrawPoint(X + y, Y + j);
			}
		}
	}
}

/**
  * ��    ��:OLED����Բ
  * ��    ��:X ָ����Բ��Բ�ĺ�����,��Χ:0~127
  * ��    ��:Y ָ����Բ��Բ��������,��Χ:0~63
  * ��    ��:A ָ����Բ�ĺ�����᳤��,��Χ:0~255
  * ��    ��:B ָ����Բ��������᳤��,��Χ:0~255
  * ��    ��:IsFilled ָ����Բ�Ƿ����
  *           ��Χ:OLED_UNFILLED		�����
  *                 OLED_FILLED			���
  * �� �� ֵ:��
  * ˵    ��:���ô˺�����,Ҫ�������س�������Ļ��,������ø��º���
  */
void OLED_DrawEllipse(u8 X, u8 Y, u8 A, u8 B, u8 IsFilled)
{
	int16_t x, y, j;
	int16_t a = A, b = B;
	float d1, d2;

	/*ʹ��Bresenham�㷨����Բ,���Ա��ⲿ�ֺ�ʱ�ĸ�������,Ч�ʸ���*/
	/*�ο�����:https://blog.csdn.net/myf_666/article/details/128167392*/

	x = 0;
	y = b;
	d1 = b * b + a * a * (-b + 0.5);

	if (IsFilled)	//ָ����Բ���
	{
		/*������ʼ��Y����*/
		for (j = -y; j < y; j++)
		{
			/*��ָ�����򻭵�,��䲿����Բ*/
			OLED_DrawPoint(X, Y + j);
			OLED_DrawPoint(X, Y + j);
		}
	}

	/*����Բ������ʼ��*/
	OLED_DrawPoint(X + x, Y + y);
	OLED_DrawPoint(X - x, Y - y);
	OLED_DrawPoint(X - x, Y + y);
	OLED_DrawPoint(X + x, Y - y);

	/*����Բ�м䲿��*/
	while (b * b * (x + 1) < a * a * (y - 0.5))
	{
		if (d1 <= 0)		//��һ�����ڵ�ǰ�㶫��
		{
			d1 += b * b * (2 * x + 3);
		}
		else				//��һ�����ڵ�ǰ�㶫�Ϸ�
		{
			d1 += b * b * (2 * x + 3) + a * a * (-2 * y + 2);
			y--;
		}
		x++;

		if (IsFilled)	//ָ����Բ���
		{
			/*�����м䲿��*/
			for (j = -y; j < y; j++)
			{
				/*��ָ�����򻭵�,��䲿����Բ*/
				OLED_DrawPoint(X + x, Y + j);
				OLED_DrawPoint(X - x, Y + j);
			}
		}

		/*����Բ�м䲿��Բ��*/
		OLED_DrawPoint(X + x, Y + y);
		OLED_DrawPoint(X - x, Y - y);
		OLED_DrawPoint(X - x, Y + y);
		OLED_DrawPoint(X + x, Y - y);
	}

	/*����Բ���ಿ��*/
	d2 = b * b * (x + 0.5) * (x + 0.5) + a * a * (y - 1) * (y - 1) - a * a * b * b;

	while (y > 0)
	{
		if (d2 <= 0)		//��һ�����ڵ�ǰ�㶫��
		{
			d2 += b * b * (2 * x + 2) + a * a * (-2 * y + 3);
			x++;

		}
		else				//��һ�����ڵ�ǰ�㶫�Ϸ�
		{
			d2 += a * a * (-2 * y + 3);
		}
		y--;

		if (IsFilled)	//ָ����Բ���
		{
			/*�������ಿ��*/
			for (j = -y; j < y; j++)
			{
				/*��ָ�����򻭵�,��䲿����Բ*/
				OLED_DrawPoint(X + x, Y + j);
				OLED_DrawPoint(X - x, Y + j);
			}
		}

		/*����Բ���ಿ��Բ��*/
		OLED_DrawPoint(X + x, Y + y);
		OLED_DrawPoint(X - x, Y - y);
		OLED_DrawPoint(X - x, Y + y);
		OLED_DrawPoint(X + x, Y - y);
	}
}

/**
  * ��    ��:OLED��Բ��
  * ��    ��:X ָ��Բ����Բ�ĺ�����,��Χ:0~127
  * ��    ��:Y ָ��Բ����Բ��������,��Χ:0~63
  * ��    ��:Radius ָ��Բ���İ뾶,��Χ:0~255
  * ��    ��:StartAngle ָ��Բ������ʼ�Ƕ�,��Χ:-180~180
  *           ˮƽ����Ϊ0��,ˮƽ����Ϊ180�Ȼ�-180��,�·�Ϊ����,�Ϸ�Ϊ����,˳ʱ����ת
  * ��    ��:EndAngle ָ��Բ������ֹ�Ƕ�,��Χ:-180~180
  *           ˮƽ����Ϊ0��,ˮƽ����Ϊ180�Ȼ�-180��,�·�Ϊ����,�Ϸ�Ϊ����,˳ʱ����ת
  * ��    ��:IsFilled ָ��Բ���Ƿ����,����Ϊ����
  *           ��Χ:OLED_UNFILLED		�����
  *                 OLED_FILLED			���
  * �� �� ֵ:��
  * ˵    ��:���ô˺�����,Ҫ�������س�������Ļ��,������ø��º���
  */
void OLED_DrawArc(u8 X, u8 Y, u8 Radius, int16_t StartAngle, int16_t EndAngle, u8 IsFilled)
{
	int16_t x, y, d, j;

	/*�˺�������Bresenham�㷨��Բ�ķ���*/

	d = 1 - Radius;
	x = 0;
	y = Radius;

	/*�ڻ�Բ��ÿ����ʱ,�ж�ָ�����Ƿ���ָ���Ƕ���,��,�򻭵�,����,��������*/
	if (OLED_IsInAngle(x, y, StartAngle, EndAngle)) { OLED_DrawPoint(X + x, Y + y); }
	if (OLED_IsInAngle(-x, -y, StartAngle, EndAngle)) { OLED_DrawPoint(X - x, Y - y); }
	if (OLED_IsInAngle(y, x, StartAngle, EndAngle)) { OLED_DrawPoint(X + y, Y + x); }
	if (OLED_IsInAngle(-y, -x, StartAngle, EndAngle)) { OLED_DrawPoint(X - y, Y - x); }

	if (IsFilled)	//ָ��Բ�����
	{
		/*������ʼ��Y����*/
		for (j = -y; j < y; j++)
		{
			/*�����Բ��ÿ����ʱ,�ж�ָ�����Ƿ���ָ���Ƕ���,��,�򻭵�,����,��������*/
			if (OLED_IsInAngle(0, j, StartAngle, EndAngle)) { OLED_DrawPoint(X, Y + j); }
		}
	}

	while (x < y)		//����X���ÿ����
	{
		x++;
		if (d < 0)		//��һ�����ڵ�ǰ�㶫��
		{
			d += 2 * x + 1;
		}
		else			//��һ�����ڵ�ǰ�㶫�Ϸ�
		{
			y--;
			d += 2 * (x - y) + 1;
		}

		/*�ڻ�Բ��ÿ����ʱ,�ж�ָ�����Ƿ���ָ���Ƕ���,��,�򻭵�,����,��������*/
		if (OLED_IsInAngle(x, y, StartAngle, EndAngle)) { OLED_DrawPoint(X + x, Y + y); }
		if (OLED_IsInAngle(y, x, StartAngle, EndAngle)) { OLED_DrawPoint(X + y, Y + x); }
		if (OLED_IsInAngle(-x, -y, StartAngle, EndAngle)) { OLED_DrawPoint(X - x, Y - y); }
		if (OLED_IsInAngle(-y, -x, StartAngle, EndAngle)) { OLED_DrawPoint(X - y, Y - x); }
		if (OLED_IsInAngle(x, -y, StartAngle, EndAngle)) { OLED_DrawPoint(X + x, Y - y); }
		if (OLED_IsInAngle(y, -x, StartAngle, EndAngle)) { OLED_DrawPoint(X + y, Y - x); }
		if (OLED_IsInAngle(-x, y, StartAngle, EndAngle)) { OLED_DrawPoint(X - x, Y + y); }
		if (OLED_IsInAngle(-y, x, StartAngle, EndAngle)) { OLED_DrawPoint(X - y, Y + x); }

		if (IsFilled)	//ָ��Բ�����
		{
			/*�����м䲿��*/
			for (j = -y; j < y; j++)
			{
				/*�����Բ��ÿ����ʱ,�ж�ָ�����Ƿ���ָ���Ƕ���,��,�򻭵�,����,��������*/
				if (OLED_IsInAngle(x, j, StartAngle, EndAngle)) { OLED_DrawPoint(X + x, Y + j); }
				if (OLED_IsInAngle(-x, j, StartAngle, EndAngle)) { OLED_DrawPoint(X - x, Y + j); }
			}

			/*�������ಿ��*/
			for (j = -x; j < x; j++)
			{
				/*�����Բ��ÿ����ʱ,�ж�ָ�����Ƿ���ָ���Ƕ���,��,�򻭵�,����,��������*/
				if (OLED_IsInAngle(-y, j, StartAngle, EndAngle)) { OLED_DrawPoint(X - y, Y + j); }
				if (OLED_IsInAngle(y, j, StartAngle, EndAngle)) { OLED_DrawPoint(X + y, Y + j); }
			}
		}
	}
}



/*********************���ܺ��� */
#include "Delay.h"

u8 liangdu = 0xFF; 	//����
u8 liangdu_UP = 0x00;	//�ϴ�����λ
u8 GUODUspeed = 11;	//ִ������
void OLED_transition(u8 flag)  //��Ļ������,�����䴫���ֵ������,����ʱΪ�����ͺ���
{
	if (flag)
	{
		if (liangdu_UP > liangdu - GUODUspeed)
		{
			liangdu_UP = liangdu;
			OLED_WriteCommand(0x81);	//���öԱȶ�
			OLED_WriteCommand(liangdu_UP);	//0x00~0xFF
			return;
		}
		liangdu_UP = liangdu_UP + GUODUspeed;
		OLED_WriteCommand(0x81);	//���öԱȶ�
		OLED_WriteCommand(liangdu_UP);	//0x00~0xFF			
	}
	else
	{
		while (1)
		{
			if (liangdu_UP < GUODUspeed)
			{
				liangdu_UP = 0;
				OLED_WriteCommand(0x81);	//���öԱȶ�
				OLED_WriteCommand(liangdu_UP);	//0x00~0xFF
				break;
			}
			OLED_WriteCommand(0x81);	//���öԱȶ�
			OLED_WriteCommand(liangdu_UP);	//0x00~0xFF
			delay_ms(16);
			liangdu_UP = liangdu_UP - GUODUspeed;
		}
		delay_ms(16);
	}
}

// void OLED_transition(u8 flag)  //��Ļ������,�����䴫���ֵ������,����ʱΪ�����ͺ���
// {

// 	if (flag)
// 	{
// 		for (u8 i = 0; i <= liangdu; i += GUODUspeed)
// 		{
// 			OLED_WriteCommand(0x81);	//���öԱȶ�
// 			OLED_WriteCommand(i);	//0x00~0xFF
// 			delay_ms(16);
// 		}

// 		OLED_WriteCommand(0x81);	//���öԱȶ�
// 		OLED_WriteCommand(liangdu);	//0x00~0xFF			
// 	}
// 	else
// 	{

// 		for (int i = liangdu; i >= 0; i -= GUODUspeed)
// 		{
// 			OLED_WriteCommand(0x81);	//���öԱȶ�
// 			OLED_WriteCommand(i);	//0x00~0xFF
// 			delay_ms(16);
// 		}

// 		OLED_WriteCommand(0x81);	//���öԱȶ�
// 		OLED_WriteCommand(0);	//0x00~0xFF			

// 	}
// }




void OLED_ReadMemory(u8 buffer[8][128])
{
	u8 j;
	/* ����ÿһҳ */
	for (j = 0; j < 8; j++)
	{
		/* ���Դ����ݸ��Ƶ������� */
		for (u8 i = 0; i < 128; i++)
		{
			buffer[j][i] = OLED_DisplayBuf[j][i];
		}
	}
}


void OLED_ShowCurrentMemory(void)
{
	u8 buffer[8][128];

	// ��ȡ��ǰ�Դ�����  
	OLED_ReadMemory(buffer);

	// ����ÿһҳ,��ʾ��ȡ���Դ�����  
	for (u8 page = 0; page < 8; page++)
	{
		// ���ù��λ��Ϊ��ǰҳ�ĵ�һ��  
		OLED_SetCursor(page, 0);

		// ����ȡ���Դ�����д�뵽OLEDӲ����  
		OLED_WriteData(buffer[page], 128);
	}
}








// dino
/* ShowGroud */
void OLED_ShowGround(u8 X, u8 Y, u8 Width, u8 Height, const u8* Image, u8 pos, u8 speed)
{
	/* X:0 Y:56 W:128 H:8 */
	// u8  ground_length = sizeof(GroundImg);    // wo
	u8 i, j;
	/*�������,��ָ֤��ͼ�񲻻ᳬ����Ļ��Χ*/
//	if (X > 127) {return;}
//	if (Y > 63) {return;}

	/*��ͼ�������������*/
	OLED_ClearArea(X, Y, Width, Height);

	/*����ָ��ͼ���漰�����ҳ*/
	/*(Height - 1) / 8 + 1��Ŀ����Height / 8������ȡ��*/
	for (j = 0; j < (Height - 1) / 8 + 1; j++)
	{
		pos = pos + speed;
		/*����ָ��ͼ���漰�������*/
		for (i = 0; i < Width; i++)
		{
			/*�����߽�,��������ʾ*/
			if (X + i > 127) { break; }
			if (Y / 8 + j > 7) { return; }

			/*��ʾͼ���ڵ�ǰҳ������*/
//			OLED_DisplayBuf[Y / 8 + j][X + i] |= Image[(i + pos)%ground_length];// << (Y % 8)
			OLED_DisplayBuf[Y / 8][X + i] |= Image[0 * Width + i + pos] << (Y % 8);  //<<(Y%8��ʾ������)

			/*�����߽�,��������ʾ*/
			/*ʹ��continue��Ŀ����,��һҳ�����߽�ʱ,��һҳ�ĺ������ݻ���Ҫ������ʾ*/
			if (Y / 8 + j + 1 > 7) { continue; }

			/*��ʾͼ������һҳ������*/
			OLED_DisplayBuf[Y / 8 + j + 1 + pos][X + i + pos] |= Image[j * Width + i] >> (8 - Y % 8);
		}
	}

	//	if(pos > ground_length) pos = 0;
}

/* ShowCloud */
void OLED_ShowCloud(int8_t X, u8 Y, u8 Width, u8 Height, const u8* Image)
{
	/* X:100 Y:8 W:26 H:8 */
	u8 i, j;

	/*�������,��ָ֤��ͼ�񲻻ᳬ����Ļ��Χ*/
	if (X > 127) { return; }
	if (Y > 63) { return; }

	/*��ͼ�������������*/
	OLED_ClearArea(X, Y, Width, Height);

	/*����ָ��ͼ���漰�����ҳ*/
	/*(Height - 1) / 8 + 1��Ŀ����Height / 8������ȡ��*/
	for (j = 0; j < (Height - 1) / 8 + 1; j++)
	{
		/*����ָ��ͼ���漰�������*/
		for (i = 0; i < Width; i++)
		{
			/*�����߽�,��������ʾ*/
			if (X + i > 127) { break; }
			if (Y / 8 + j > 7) { return; }

			/*��ʾͼ���ڵ�ǰҳ������*/
			OLED_DisplayBuf[Y / 8 + j][X + i] |= Image[j * Width + i] << (Y % 8);

			/*�����߽�,��������ʾ*/
			/*ʹ��continue��Ŀ����,��һҳ�����߽�ʱ,��һҳ�ĺ������ݻ���Ҫ������ʾ*/
			if (Y / 8 + j + 1 > 7) { continue; }

			/*��ʾͼ������һҳ������*/
			OLED_DisplayBuf[Y / 8 + j + 1][X + i] |= Image[j * Width + i] >> (8 - Y % 8);
		}
	}
}

/* show dionrun */
void OLED_ShowDinoRun(u8 X, u8 Y, u8 Width, u8 Height, const u8* Image)
{
	u8 i, j;

	/*�������,��ָ֤��ͼ�񲻻ᳬ����Ļ��Χ*/
	if (X > 127) { return; }
	if (Y > 63) { return; }

	/*��ͼ�������������*/
	OLED_ClearArea(X, Y, Width, Height);

	/*����ָ��ͼ���漰�����ҳ*/
	/*(Height - 1) / 8 + 1��Ŀ����Height / 8������ȡ��*/
	for (j = 0; j < (Height - 1) / 8 + 1; j++)
	{
		/*����ָ��ͼ���漰�������*/
		for (i = 0; i < Width; i++)
		{
			/*�����߽�,��������ʾ*/
			if (X + i > 127) { break; }
			if (Y / 8 + j > 7) { return; }

			/*��ʾͼ���ڵ�ǰҳ������*/
			OLED_DisplayBuf[Y / 8 + j][X + i] |= Image[j * Width + i] << (Y % 8);

			/*�����߽�,��������ʾ*/
			/*ʹ��continue��Ŀ����,��һҳ�����߽�ʱ,��һҳ�ĺ������ݻ���Ҫ������ʾ*/
			if (Y / 8 + j + 1 > 7) { continue; }

			/*��ʾͼ������һҳ������*/
			OLED_DisplayBuf[Y / 8 + j + 1][X + i] |= Image[j * Width + i] >> (8 - Y % 8);
		}
	}
}

/* show DinoJump */
void OLED_ShowDinoJump(u8 X, u8 Y, u8 Width, u8 Height, const u8* Image)
{
	u8 i, j;

	/*�������,��ָ֤��ͼ�񲻻ᳬ����Ļ��Χ*/
//	if (X > 127) {return;}
//	if (Y > 63) {return;}

	/*��ͼ�������������*/
	OLED_ClearArea(X, Y, Width, Height);

	/*����ָ��ͼ���漰�����ҳ*/
	/*(Height - 1) / 8 + 1��Ŀ����Height / 8������ȡ��*/
	for (j = 0; j < (Height - 1) / 8 + 1; j++)
	{
		/*����ָ��ͼ���漰�������*/
		for (i = 0; i < Width; i++)
		{
			/*�����߽�,��������ʾ*/
			if (X + i > 127) { break; }
			if (Y / 8 + j > 7) { return; }

			/*��ʾͼ���ڵ�ǰҳ������*/
			OLED_DisplayBuf[Y / 8 + j][X + i] |= Image[j * Width + i] << (Y % 8);

			/*�����߽�,��������ʾ*/
			/*ʹ��continue��Ŀ����,��һҳ�����߽�ʱ,��һҳ�ĺ������ݻ���Ҫ������ʾ*/
			if (Y / 8 + j + 1 > 7) { continue; }

			/*��ʾͼ������һҳ������*/
			OLED_DisplayBuf[Y / 8 + j + 1][X + i] |= Image[j * Width + i] >> (8 - Y % 8);
		}
	}
}

/* show cactus */
void OLED_ShowCactus(u8 X, u8 Y, u8 Width, u8 Height, const u8* Image)
{
	u8 i, j;

	/*�������,��ָ֤��ͼ�񲻻ᳬ����Ļ��Χ*/
//	if (X > 127) {return;}
//	if (Y > 63) {return;}

	/*��ͼ�������������*/
	OLED_ClearArea(X, Y, Width, Height);

	/*����ָ��ͼ���漰�����ҳ*/
	/*(Height - 1) / 8 + 1��Ŀ����Height / 8������ȡ��*/
	for (j = 0; j < (Height - 1) / 8 + 1; j++)
	{
		/*����ָ��ͼ���漰�������*/
		for (i = 0; i < Width; i++)
		{
			/*�����߽�,��������ʾ*/
			if (X + i > 127) { break; }
			if (Y / 8 + j > 7) { return; }

			/*��ʾͼ���ڵ�ǰҳ������*/
			OLED_DisplayBuf[Y / 8 + j][X + i] |= Image[j * Width + i] << (Y % 8);

			/*�����߽�,��������ʾ*/
			/*ʹ��continue��Ŀ����,��һҳ�����߽�ʱ,��һҳ�ĺ������ݻ���Ҫ������ʾ*/
			if (Y / 8 + j + 1 > 7) { continue; }

			/*��ʾͼ������һҳ������*/
			OLED_DisplayBuf[Y / 8 + j + 1][X + i] |= Image[j * Width + i] >> (8 - Y % 8);
		}
	}
}

/* show game over */
void OLED_ShowGameOver(u8 X, u8 Y, u8 Width, u8 Height, const u8* Image, u8 over_flag)
{
	u8 i, j;

	/*�������,��ָ֤��ͼ�񲻻ᳬ����Ļ��Χ*/
	if (X > 127) { return; }
	if (Y > 63) { return; }

	/*��ͼ�������������*/
	OLED_ClearArea(X, Y, Width, Height);

	/*����ָ��ͼ���漰�����ҳ*/
	/*(Height - 1) / 8 + 1��Ŀ����Height / 8������ȡ��*/
	for (j = 0; j < (Height - 1) / 8 + 1; j++)
	{
		/*����ָ��ͼ���漰�������*/
		for (i = 0; i < Width; i++)
		{
			/*�����߽�,��������ʾ*/
			if (X + i > 127) { break; }
			if (Y / 8 + j > 7) { return; }

			/*��ʾͼ���ڵ�ǰҳ������*/
			OLED_DisplayBuf[Y / 8 + j][X + i] |= Image[j * Width + i] << (Y % 8);

			/*�����߽�,��������ʾ*/
			/*ʹ��continue��Ŀ����,��һҳ�����߽�ʱ,��һҳ�ĺ������ݻ���Ҫ������ʾ*/
			if (Y / 8 + j + 1 > 7) { continue; }

			/*��ʾͼ������һҳ������*/
			OLED_DisplayBuf[Y / 8 + j + 1][X + i] |= Image[j * Width + i] >> (8 - Y % 8);
		}
	}
}

/* show crow fly */
void OLED_ShowCrowFly(u8 X, u8 Y, u8 Width, u8 Height, const u8* Image)
{
	u8 i, j;

	/*�������,��ָ֤��ͼ�񲻻ᳬ����Ļ��Χ*/
	if (X > 127) { return; }
	if (Y > 63) { return; }

	/*��ͼ�������������*/
	OLED_ClearArea(X, Y, Width, Height);

	/*����ָ��ͼ���漰�����ҳ*/
	/*(Height - 1) / 8 + 1��Ŀ����Height / 8������ȡ��*/
	for (j = 0; j < (Height - 1) / 8 + 1; j++)
	{
		/*����ָ��ͼ���漰�������*/
		for (i = 0; i < Width; i++)
		{
			/*�����߽�,��������ʾ*/
			if (X + i > 127) { break; }
			if (Y / 8 + j > 7) { return; }

			/*��ʾͼ���ڵ�ǰҳ������*/
			OLED_DisplayBuf[Y / 8 + j][X + i] |= Image[j * Width + i] << (Y % 8);

			/*�����߽�,��������ʾ*/
			/*ʹ��continue��Ŀ����,��һҳ�����߽�ʱ,��һҳ�ĺ������ݻ���Ҫ������ʾ*/
			if (Y / 8 + j + 1 > 7) { continue; }

			/*��ʾͼ������һҳ������*/
			OLED_DisplayBuf[Y / 8 + j + 1][X + i] |= Image[j * Width + i] >> (8 - Y % 8);
		}
	}
}


/*****************��Э�Ƽ�|��Ȩ����****************/
/*****************jiangxiekeji.com*****************/

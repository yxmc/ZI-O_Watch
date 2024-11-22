#include "stm32f10x.h"    
#include <stdint.h>  
#include <string.h>  
#include <stdio.h> 
#include "OLED.h"
#include "ASR_su_21t.h"
#include "MPU6050.h"
#include "stm32_power.h"    


u8 Normal_Flag = 1;
u8 PowerSaving_Flag = 0;
u8 SuperSaving_Flag = 0;
u8 POWER_OFF = 0;

Mode WorkingMode = Normal;

extern u8 liangdu;

void BackEnd(void)
{
    if (Voicelogo)  				//��⵽����ָ��
    {
        OLED_Power(1);
        OLED_WriteCommand(0x81);	//���öԱȶ�
        OLED_WriteCommand(liangdu);   //0x00~0xFF

        ASR_dispose();

        OLED_Clear();				//����Դ�����
        OLED_Update();				//������ʾ,����	
        OLED_Power(0);
    }

}



extern u8 RAWA;

void Power_OFF(void)
{

    GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_SET);

    Voicelogo = 0;

    OLED_Clear();				//����Դ�����
    OLED_Update();				//������ʾ,����

    OLED_Power(0);

    if (RAWA) MPU_Write_Byte(MPU_INT_EN_REG, 0X01);  		 //����MPU6050���ݾ����ж�


    while (POWER_OFF)			//�����Ҫʡ��ģʽ,�����ʡ��ģʽ
    {
        if (WorkingMode == PowerSaving)
        {
            // ʹ�ܵ�Դ����ʱ��
            // RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
            PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI); 	//STM32����ֹͣģʽ(�ϵ͹���),�ȴ��ⲿ�жϻ���
        }
        else if (WorkingMode == SuperSaving)
        {
            // RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
            PWR_WakeUpPinCmd(ENABLE);	 //ʹ��λ��PA0��WKUP����,WKUP���������ػ��Ѵ���ģʽ
            PWR_EnterSTANDBYMode();		 //STM32�������ģʽ(���͹���),���ȴ�ָ���Ļ����¼�(WKUP�����ػ�RTC����)
        }
        else
        {

        }
        BackEnd();                       //һ��������,����������֮��Ҫ��ʲô		

    }

    SystemInit();               //��������ʱ��

    if (RAWA) MPU_Write_Byte(MPU_INT_EN_REG, 0X00);     //�ر�MPU6050���ݾ����ж�


    OLED_Power(1);

    // OLED_WriteCommand(0x81);	//���öԱȶ�
    // OLED_WriteCommand(0x00);    //0x00~0xFF

    GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_RESET);
}



extern s8 mubz;
extern u8 Cursor;
extern u8 RAWA;
extern u8 RAWAtri;		//̧�󴥷�ֵ
extern u16 RAWAtrigger;	//ʵ��̧�󴥷�ֵ
extern float RTI;
extern u8 Time_12_flag;
extern u8 Time_24_flag;
extern u8 Step_Flag;

// д���ʼֵ���Ĵ���
void Load_Config(void)
{
    u16 temp;

    /* ���ݼĴ�������ʹ�� */
    PWR_BackupAccessCmd(ENABLE);

    // BKP_DR2
    temp = mubz << 8 | liangdu;
    BKP_WriteBackupRegister(BKP_DR2, temp); //�����ڱ��ݼĴ�������ֵ�Ͳ���� ��Ļ�Աȶ�

    // BKP_DR3
    temp = Cursor << 12 | (Normal_Flag | PowerSaving_Flag | SuperSaving_Flag) << 8 | Step_Flag << 4;
    BKP_WriteBackupRegister(BKP_DR3, temp);	//�����ڱ��ݼĴ�����Ĺ��λ��	����ģʽ��־λ	�Ʋ�����

    PWR_BackupAccessCmd(ENABLE);
    // BKP_DR4
    temp = RAWAtri << 8 | Time_24_flag << 4 | RAWA;
    BKP_WriteBackupRegister(BKP_DR4, temp); //�����ڱ��ݼĴ������̧�󴥷�ֵ  ʱ����ʾ��ʽ	̧���ѱ�־λ

    // BKP_DR6~7
    u16 high, low;
    u32 temp1;
    memcpy(&temp1, &RTI, sizeof(float));
    high = (temp1 >> 16) & 0xFFFF; // ��ȡ��16λ  
    low = temp1 & 0xFFFF;          // ��ȡ��16λ 

    BKP_WriteBackupRegister(BKP_DR6, high);
    BKP_WriteBackupRegister(BKP_DR7, low);	//��RTI����bkp�Ĵ���


}


//�����ʼֵ
void Heavy_Load(void)
{
    u16 temp;

    //BKP_DR1��RTC����


    /* ���ݼĴ�������ʹ�� */
    PWR_BackupAccessCmd(ENABLE);

    //BKP_DR1��RTC����


    // BKP_DR2
    temp = BKP_ReadBackupRegister(BKP_DR2);
    liangdu = temp & 0xff;	//��ȡ��8λ��Ļ�Աȶ�0
    mubz = temp >> 8; 		//��ȡ��8λ�ֵ�Ͳ����

    PWR_BackupAccessCmd(ENABLE);
    // BKP_DR3
    temp = BKP_ReadBackupRegister(BKP_DR3);
    Step_Flag = (temp >> 4) & 0xf;		//��ȡ7-4λ�Ʋ�����

    Normal_Flag = (temp >> 8) & 0x01;   //��ȡ11-8λ����ģʽ��־λ
    PowerSaving_Flag = (temp >> 8) & 0x02;
    SuperSaving_Flag = (temp >> 8) & 0x04;

    if (SuperSaving_Flag) WorkingMode = SuperSaving;
    else if (PowerSaving_Flag) WorkingMode = PowerSaving;
    else WorkingMode = Normal;

    Cursor = temp >> 12;	//��ȡ15-12λ���λ��


    // BKP_DR4
    temp = BKP_ReadBackupRegister(BKP_DR4);

    RAWA = temp & 0x01;			//��ȡ3-0λ̧���ѱ�־λ
    if ((temp & 0xf0) == 0x10) 	//��ȡ7-4ʱ����ʾ��ʽ
    {
        Time_12_flag = 0;
        Time_24_flag = 1;
    }
    else
    {
        Time_12_flag = 1;
        Time_24_flag = 0;
    }
    RAWAtri = temp >> 8;	 //��ȡ��8λ̧�󴥷�ֵ
    RAWAtrigger = RAWAtri * 70;



    // �ϲ�Ϊu32  
    u32 temp1 = ((u32)BKP_ReadBackupRegister(BKP_DR6) << 16) | (u32)BKP_ReadBackupRegister(BKP_DR7);  //��ȡ�����ڱ��ݼĴ������RTIֵ
    memcpy(&RTI, &temp1, sizeof(float));	//ת����float����

}






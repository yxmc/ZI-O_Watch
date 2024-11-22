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
    if (Voicelogo)  				//检测到语音指令
    {
        OLED_Power(1);
        OLED_WriteCommand(0x81);	//设置对比度
        OLED_WriteCommand(liangdu);   //0x00~0xFF

        ASR_dispose();

        OLED_Clear();				//清空显存数组
        OLED_Update();				//更新显示,清屏	
        OLED_Power(0);
    }

}



extern u8 RAWA;

void Power_OFF(void)
{

    GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_SET);

    Voicelogo = 0;

    OLED_Clear();				//清空显存数组
    OLED_Update();				//更新显示,清屏

    OLED_Power(0);

    if (RAWA) MPU_Write_Byte(MPU_INT_EN_REG, 0X01);  		 //开启MPU6050数据就绪中断


    while (POWER_OFF)			//如果需要省电模式,则进入省电模式
    {
        if (WorkingMode == PowerSaving)
        {
            // 使能电源控制时钟
            // RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
            PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI); 	//STM32进入停止模式(较低功耗),等待外部中断唤醒
        }
        else if (WorkingMode == SuperSaving)
        {
            // RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
            PWR_WakeUpPinCmd(ENABLE);	 //使能位于PA0的WKUP引脚,WKUP引脚上升沿唤醒待机模式
            PWR_EnterSTANDBYMode();		 //STM32进入待机模式(极低功耗),并等待指定的唤醒事件(WKUP上升沿或RTC闹钟)
        }
        else
        {

        }
        BackEnd();                       //一个后处理函数,决定被唤醒之后要干什么		

    }

    SystemInit();               //重新配置时钟

    if (RAWA) MPU_Write_Byte(MPU_INT_EN_REG, 0X00);     //关闭MPU6050数据就绪中断


    OLED_Power(1);

    // OLED_WriteCommand(0x81);	//设置对比度
    // OLED_WriteCommand(0x00);    //0x00~0xFF

    GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_RESET);
}



extern s8 mubz;
extern u8 Cursor;
extern u8 RAWA;
extern u8 RAWAtri;		//抬腕触发值
extern u16 RAWAtrigger;	//实际抬腕触发值
extern float RTI;
extern u8 Time_12_flag;
extern u8 Time_24_flag;
extern u8 Step_Flag;

// 写入初始值到寄存器
void Load_Config(void)
{
    u16 temp;

    /* 备份寄存器访问使能 */
    PWR_BackupAccessCmd(ENABLE);

    // BKP_DR2
    temp = mubz << 8 | liangdu;
    BKP_WriteBackupRegister(BKP_DR2, temp); //保存在备份寄存器里的手电筒亮度 屏幕对比度

    // BKP_DR3
    temp = Cursor << 12 | (Normal_Flag | PowerSaving_Flag | SuperSaving_Flag) << 8 | Step_Flag << 4;
    BKP_WriteBackupRegister(BKP_DR3, temp);	//保存在备份寄存器里的光标位置	工作模式标志位	计步开关

    PWR_BackupAccessCmd(ENABLE);
    // BKP_DR4
    temp = RAWAtri << 8 | Time_24_flag << 4 | RAWA;
    BKP_WriteBackupRegister(BKP_DR4, temp); //保存在备份寄存器里的抬腕触发值  时间显示格式	抬腕唤醒标志位

    // BKP_DR6~7
    u16 high, low;
    u32 temp1;
    memcpy(&temp1, &RTI, sizeof(float));
    high = (temp1 >> 16) & 0xFFFF; // 提取高16位  
    low = temp1 & 0xFFFF;          // 提取低16位 

    BKP_WriteBackupRegister(BKP_DR6, high);
    BKP_WriteBackupRegister(BKP_DR7, low);	//将RTI存入bkp寄存器


}


//载入初始值
void Heavy_Load(void)
{
    u16 temp;

    //BKP_DR1被RTC用了


    /* 备份寄存器访问使能 */
    PWR_BackupAccessCmd(ENABLE);

    //BKP_DR1被RTC用了


    // BKP_DR2
    temp = BKP_ReadBackupRegister(BKP_DR2);
    liangdu = temp & 0xff;	//读取低8位屏幕对比度0
    mubz = temp >> 8; 		//读取高8位手电筒亮度

    PWR_BackupAccessCmd(ENABLE);
    // BKP_DR3
    temp = BKP_ReadBackupRegister(BKP_DR3);
    Step_Flag = (temp >> 4) & 0xf;		//读取7-4位计步开关

    Normal_Flag = (temp >> 8) & 0x01;   //读取11-8位工作模式标志位
    PowerSaving_Flag = (temp >> 8) & 0x02;
    SuperSaving_Flag = (temp >> 8) & 0x04;

    if (SuperSaving_Flag) WorkingMode = SuperSaving;
    else if (PowerSaving_Flag) WorkingMode = PowerSaving;
    else WorkingMode = Normal;

    Cursor = temp >> 12;	//读取15-12位光标位置


    // BKP_DR4
    temp = BKP_ReadBackupRegister(BKP_DR4);

    RAWA = temp & 0x01;			//读取3-0位抬腕唤醒标志位
    if ((temp & 0xf0) == 0x10) 	//读取7-4时间显示格式
    {
        Time_12_flag = 0;
        Time_24_flag = 1;
    }
    else
    {
        Time_12_flag = 1;
        Time_24_flag = 0;
    }
    RAWAtri = temp >> 8;	 //读取高8位抬腕触发值
    RAWAtrigger = RAWAtri * 70;



    // 合并为u32  
    u32 temp1 = ((u32)BKP_ReadBackupRegister(BKP_DR6) << 16) | (u32)BKP_ReadBackupRegister(BKP_DR7);  //读取保存在备份寄存器里的RTI值
    memcpy(&RTI, &temp1, sizeof(float));	//转换回float类型

}






/**
  *     版本V2.10
  *        程序写得比较烂,酌情观看哈
  *        参考(Ctrl+C)了许多大佬的程序,在此表示感谢  o(*￣▽￣  )ノo...
  *        如果有电池电压不准的情况请修改Time_page里面的BAT_volt_CAL进行校准,电压比实际多的话调小一点点,反之亦然
  *        如果编译有警告是正常的,因为有一些变量范围超了,不影响
  */

#include "stm32f10x.h" 
#include "Delay.h"  
#include "OLED.h"     
#include "menu.h"
#include "AD.h"
#include "Serial2.h"  
#include "string.h"
#include "key.h"
#include "MyRTC.h"
#include "Time_page.h"
#include "stm32_power.h" 
#include "ASR_su_21t.h" 
  // #include "RF_Ev1527.h"   //取消了
#include "AHT20.h"
#include "MPU6050.h"     
//#include "inv_mpu.h"     //芯片flash不够,暂时不用
//#include "inv_mpu_dmp_motion_driver.h" 
#include "StepCount.h"

#include "CH7800play.h"
#include "StepCount.h"
#include "dino.h"
#include "voltage_mea.h"


void STM32_rouse(void);
// void MPU6050_SelfTest(void);

static s16 GyroX;
//static s16 GyroX, Aacx;
void MPU6050_Get_GXData(s16* GyroX);

void MPU6050_Get_AXData(s16* Aacx);


// 禁用EXTI线0中断
void EXTI0_Disable(void);

// 重新使能EXTI线0中断
void EXTI0_Enable(void);

extern u8 RAWA;

extern u8 Step_Flag;



int main(void)
{
    Key_Init();                                                        //按键初始化

    delay_init();		                                             //延时函数初始化

    OLED_Init();                                                   //oled显示屏初始化	

    {                                                            //开机动画
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);      //开启PWR的时钟
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);
        PWR_BackupAccessCmd(ENABLE);
        if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)           //判断是否是第一次开机 读取RTC备份寄存器
        {
            KAIJIDONGHUA();
            Load_Config();
        }
        else
        {
            Heavy_Load();                    //检查BKP寄存器,载入参数
        }
    }

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);      // 开启AFIO时钟

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//重映射配置,用PB3,PB4,PA15做普通IO,PA13&14用于SWD调试

    Time_page_io_init();                                  //表盘功能需要的检测引脚

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	//配置NVIC为分组2

    AD_Init();				                          //AD转换器初始化

    MyRTC_Init();							        //RTC初始化

    MPU_Init();                                   //MPU6050初始化 
    //    while (mpu_dmp_init())                     //MPU6050初始化稳定后,开始初始化DMP
    //    {
    //        delay_ms(200);
    //    };
    //       EV1527_IO_Init();                   //驱动搭载的EV1527芯片
    // OLED_transition(1);
    // OLED_transition(1);
    // OLED_ShowHexNum(0, 32, MPU_Init(), 2, OLED_8X16);
    // OLED_Update();
    // delay_ms(3000);

    STM32_rouse();                              //唤醒休眠所用的中断配置

    AHT20_Init();						      //AHT20温湿度传感器初始化

    CH7800_Init();					        //ch7800音频芯片初始化

    Serial2_Init();                       //串口2初始化(su21t模块)

    EXTI0_Disable();                    //禁用EXTI线0中断  

    timer4_Init();                    //定时器4初始化(计步用)

    if (!Step_Flag)                 //计步
    {
        TIM_Cmd(TIM4, DISABLE);
    }

    s8 p;                        //存储按键值变量

    u8 XIUMIAN = 0;            //临时的主屏幕休眠时间变量

    u8 a = 2;                //暂缓按键检测变量

    while (a)              //先亮个屏,同时防止按键检测过快
    {
        Time_page();
        delay_ms(4);
        OLED_transition(1);
        a = 0;
    }

    a = 2;



    // float pitch, roll, yaw; 		//欧拉角
    // 俯仰角x(-90°~90°), 横滚角y(-180°~180°), 航向角z(-180°~180°)
    // short aacx, aacy, aacz;			//加速度传感器原始数据
    // short gyrox, gyroy, gyroz;		//陀螺仪原始数据
    // MPU_Write_Byte(MPU_INT_EN_REG, 0X01);




    
    while (1)
    {


        OLED_WriteCommand(0x40);	//设置显示开始行,0x40~0x7F
        OLED_WriteCommand(0xA1);	//设置左右方向,0xA1正常,0xA0左右反置
        OLED_WriteCommand(0xC8);	//设置上下方向,0xC8正常,0xC0上下反置


        // OLED_Clear();
        // MPU6050_Get_GXData(&GyroX);
        // // OLED_ShowNum(0, 16, Aacx, 8, OLED_8X16);
        // // OLED_ShowNum(0, 32, GyroX, 8, OLED_8X16);
        // // OLED_Printf(0, 16, OLED_8X16, "%d", Aacx);
        // OLED_Printf(0, 32, OLED_8X16, "%d", GyroX);
        // OLED_Update();
        // delay_ms(200);
        // OLED_transition(1);

        Time_page();            //刷新表盘
        p = menu_Roll_event();	//获取按键

        if (XIUMIAN > 200)      //轮循等待关机时间
        {
            p = -1;
            XIUMIAN = 0;
        }
        else { XIUMIAN++; }

        if (RAWA)               //落腕熄屏
        {
            MPU6050_Get_GXData(&GyroX);     //读取x轴陀螺仪数据  改
            // MPU_Get_Accelerometer(&aacx, &aacy, &aacz);	    //得到加速度传感器数据
            // MPU_Get_Gyroscope(&gyrox, &gyroy, &gyroz);	    //得到陀螺仪数据
            // if (GyroX > 50 && GyroX < 0 && Aacx < 10000)
            if (GyroX > 50 && GyroX < 0)
            {
                p = -1;
            }
        }

        if (p == 1)             //下拨按键
        {
            XIUMIAN = 0;
            OLED_transition(0);
            main_menu2();		//进入菜单2

        }
        else if (p == -1 && WorkingMode == Normal)       //上拨按键 息屏
        {
            EXTI0_Enable();
            XIUMIAN = 0;
            POWER_OFF = 1;
            Power_OFF();    //进入待机
        }
        else if (p == -1 && WorkingMode == PowerSaving)  //停止
        {
            EXTI0_Enable();
            XIUMIAN = 0;
            // OLED_transition(0);
            POWER_OFF = 1;
            Power_OFF();    //进入待机

        }
        else if (p == -1 && WorkingMode == SuperSaving)  //待机
        {
            XIUMIAN = 0;
            OLED_transition(0);
            POWER_OFF = 1;
            Power_OFF();    //进入待机

        }


        //这一段是为了防止开机后立即就去判断按键,防止误触发
        while (a)
        {
            Time_page();
            delay_ms(4);
            OLED_transition(1);
            a = 0;
        }
        a = 2;

        p = menu_Enter_event();
        if (p)
        {
            XIUMIAN = 0;
            OLED_transition(0);
            main_menu1();   //进入菜单1
        }

    }


}


//中断处理函数
void EXTI0_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line0) == SET)	        //外部中断0号线触发的中断,摇杆下拨
    {
        POWER_OFF = 0;  //取消待机
        // OLED_Power(1);
        // OLED_transition(1);

        EXTI_ClearITPendingBit(EXTI_Line0);             //清除中断标志位
    }
}

extern u8 RAWAtri;          //抬腕触发值
extern u16 RAWAtrigger;     //实际抬腕触发值
u8 Addtrigger = 0;          //手表不动累加值


void EXTI9_5_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line6) == SET)		//外部中断6号线触发的中断,语音识别模块
    {
        if (POWER_OFF)SystemInit();
        Voicelogo = 1;

        EXTI_ClearITPendingBit(EXTI_Line6);		//清除中断标志位
    }


    if (EXTI_GetITStatus(EXTI_Line7) == SET)		//外部中断7号线触发的中断,MPU6050
    {
        // MPU6050_Get_AXData(&Aacx);
        MPU6050_Get_GXData(&GyroX); //读取x轴陀螺仪数据

        if (GyroX > RAWAtrigger)//如果大于触发值
        {
            OLED_Power(1);
            //OLED_transition(1);
            POWER_OFF = 0; //取消待机
            Addtrigger = 0;
            RAWAtrigger = RAWAtri * 70;//恢复触发值
        }
        // else if (Pick_up)
        // {
        //     if (-30 < GyroX && GyroX < 30)//如果手表没有动
        //     {
        //         if (Addtrigger < 80)
        //         {
        //             Addtrigger++;       //累加80次,实际大约5秒
        //         }
        //         else RAWAtrigger = 5;   //变为运动触发模式,一但检测到有点运动,马上唤醒
        //     }
        //     else Addtrigger = 0;
        // }

        EXTI_ClearITPendingBit(EXTI_Line7);		//清除中断标志位
    }

}

//用于唤醒的外部中断,统一在这里配置
void STM32_rouse(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    //PA0在Key_Init();里面已经配置过了,所以直接从PA6开始
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;	//SU-21t模块用
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	//MPU6050用
    GPIO_Init(GPIOB, &GPIO_InitStructure);


    /*AFIO选择中断引脚*/
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0); //将外部中断的0号线映射到GPIOA   按键唤醒引脚
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource6); //将外部中断的6号线映射到GPIOA   SU-21t模块唤醒引脚
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource7); //将外部中断的7号线映射到GPIOB   MPU6050唤醒引脚

    /*EXTI初始化*/
    EXTI_InitTypeDef EXTI_InitStructure;				     //定义结构体变量
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;			    //指定外部中断线使能
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	   //指定外部中断线为中断模式
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//指定外部中断线为上升沿触发

    EXTI_InitStructure.EXTI_Line = EXTI_Line0;			//选择配置外部中断
    EXTI_Init(&EXTI_InitStructure);					   //将结构体变量交给EXTI_Init,配置EXTI外设

    EXTI_InitStructure.EXTI_Line = EXTI_Line6;	     //选择配置外部中断
    EXTI_Init(&EXTI_InitStructure);					//将结构体变量交给EXTI_Init,配置EXTI外设

    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  //指定外部中断线为下降沿触发
    EXTI_InitStructure.EXTI_Line = EXTI_Line7;			    //选择配置外部中断
    EXTI_Init(&EXTI_InitStructure);						   //将结构体变量交给EXTI_Init,配置EXTI外设


    /*NVIC配置*/
    NVIC_InitTypeDef NVIC_InitStructure;				       //定义结构体变量
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  //指定NVIC线路使能
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//指定NVIC线路的抢占优先级为1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	    //指定NVIC线路的响应优先级为1

    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;	    //选择配置NVIC的0线
    NVIC_Init(&NVIC_InitStructure);						    //配置NVIC

    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_Init(&NVIC_InitStructure);
}


// 获取陀螺仪X轴数据
void MPU6050_Get_GXData(s16* GyroX)
{
    u8 DataH, DataL;							 //定义数据高8位和低8位的变量

    DataH = MPU_Read_Byte(MPU_GYRO_XOUTH_REG);  //读取陀螺仪X轴的高8位数据
    DataL = MPU_Read_Byte(MPU_GYRO_XOUTL_REG); //读取陀螺仪X轴的低8位数据
    *GyroX = (DataH << 8) | DataL;				 //数据拼接,通过输出参数返回

}

// 获取加速度计X轴数据
void MPU6050_Get_AXData(s16* Aacx)
{
    u8 DataH, DataL;							 // 定义数据高8位和低8位的变量

    DataH = MPU_Read_Byte(MPU_ACCEL_XOUTH_REG);  // 读取加速度计X轴的高8位数据
    DataL = MPU_Read_Byte(MPU_ACCEL_XOUTL_REG);  // 读取加速度计X轴的低8位数据
    *Aacx = (DataH << 8) | DataL;				 // 数据拼接,通过输出参数返回
}


// 禁用EXTI线0中断
void EXTI0_Disable(void)
{
    // 清除 EXTI 中断使能寄存器中的 EXTI0 位
    EXTI->IMR &= ~(1 << 0); // 0对应EXTI线0
}

// 重新使能EXTI线0中断
void EXTI0_Enable(void)
{
    // 设置 EXTI 中断使能寄存器中的 EXTI0 位
    EXTI->IMR |= (1 << 0); // 0对应EXTI线0
}











// void MPU6050_SelfTest(void)
// {
//     // 开启加速度计自测
//     MPU_Write_Byte(0x1C, 0xE0);  // 开启 X, Y, Z 轴加速度计自测
//     delay_ms(20);                // 等待自测完成
//     // 读取加速度计自测响应
//     int16_t ax, ay, az;
//     MPU_Get_Accelerometer(&ax, &ay, &az);
//     OLED_Clear();
//     // 比较自测响应
//     if (abs(ax) > 0 || abs(ay) > 0 || abs(az) > 0)
//     {
//         OLED_Printf(0, 12, OLED_8X16, "Accelerometer failed!\n");
//     }
//     else
//     {
//         OLED_Printf(0, 12, OLED_8X16, "Accelerometer passed!\n");
//     }
//     OLED_Update();
//     // 关闭加速度计自测
//     MPU_Write_Byte(0x1C, 0x00);
//     delay_ms(3000);
//     OLED_Clear();
//     // 开启陀螺仪自测
//     MPU_Write_Byte(0x1B, 0xE0);  // 开启 X, Y, Z 轴陀螺仪自测
//     delay_ms(20);  // 等待自测完成
//     // 读取陀螺仪自测响应
//     int16_t gx, gy, gz;
//     MPU_Get_Gyroscope(&gx, &gy, &gz);
//     // 比较自测响应
//     if (abs(gx) > 0 || abs(gy) > 0 || abs(gz) > 0)
//     {
//         OLED_Printf(0, 12, OLED_8X16, "Gyroscope failed!\n");
//     }
//     else {
//         OLED_Printf(0, 12, OLED_8X16, "Gyroscope passed!\n");
//     }
//     OLED_Update();
//     // 关闭陀螺仪自测
//     MPU_Write_Byte(0x1B, 0x00);
//     delay_ms(3000);
// }


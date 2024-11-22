/**
  *     �汾V2.10
  *        ����д�ñȽ���,����ۿ���
  *        �ο�(Ctrl+C)�������еĳ���,�ڴ˱�ʾ��л  o(*������  )��o...
  *        ����е�ص�ѹ��׼��������޸�Time_page�����BAT_volt_CAL����У׼,��ѹ��ʵ�ʶ�Ļ���Сһ���,��֮��Ȼ
  *        ��������о�����������,��Ϊ��һЩ������Χ����,��Ӱ��
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
  // #include "RF_Ev1527.h"   //ȡ����
#include "AHT20.h"
#include "MPU6050.h"     
//#include "inv_mpu.h"     //оƬflash����,��ʱ����
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


// ����EXTI��0�ж�
void EXTI0_Disable(void);

// ����ʹ��EXTI��0�ж�
void EXTI0_Enable(void);

extern u8 RAWA;

extern u8 Step_Flag;



int main(void)
{
    Key_Init();                                                        //������ʼ��

    delay_init();		                                             //��ʱ������ʼ��

    OLED_Init();                                                   //oled��ʾ����ʼ��	

    {                                                            //��������
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);      //����PWR��ʱ��
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);
        PWR_BackupAccessCmd(ENABLE);
        if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)           //�ж��Ƿ��ǵ�һ�ο��� ��ȡRTC���ݼĴ���
        {
            KAIJIDONGHUA();
            Load_Config();
        }
        else
        {
            Heavy_Load();                    //���BKP�Ĵ���,�������
        }
    }

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);      // ����AFIOʱ��

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//��ӳ������,��PB3,PB4,PA15����ͨIO,PA13&14����SWD����

    Time_page_io_init();                                  //���̹�����Ҫ�ļ������

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	//����NVICΪ����2

    AD_Init();				                          //ADת������ʼ��

    MyRTC_Init();							        //RTC��ʼ��

    MPU_Init();                                   //MPU6050��ʼ�� 
    //    while (mpu_dmp_init())                     //MPU6050��ʼ���ȶ���,��ʼ��ʼ��DMP
    //    {
    //        delay_ms(200);
    //    };
    //       EV1527_IO_Init();                   //�������ص�EV1527оƬ
    // OLED_transition(1);
    // OLED_transition(1);
    // OLED_ShowHexNum(0, 32, MPU_Init(), 2, OLED_8X16);
    // OLED_Update();
    // delay_ms(3000);

    STM32_rouse();                              //�����������õ��ж�����

    AHT20_Init();						      //AHT20��ʪ�ȴ�������ʼ��

    CH7800_Init();					        //ch7800��ƵоƬ��ʼ��

    Serial2_Init();                       //����2��ʼ��(su21tģ��)

    EXTI0_Disable();                    //����EXTI��0�ж�  

    timer4_Init();                    //��ʱ��4��ʼ��(�Ʋ���)

    if (!Step_Flag)                 //�Ʋ�
    {
        TIM_Cmd(TIM4, DISABLE);
    }

    s8 p;                        //�洢����ֵ����

    u8 XIUMIAN = 0;            //��ʱ������Ļ����ʱ�����

    u8 a = 2;                //�ݻ�����������

    while (a)              //��������,ͬʱ��ֹ����������
    {
        Time_page();
        delay_ms(4);
        OLED_transition(1);
        a = 0;
    }

    a = 2;



    // float pitch, roll, yaw; 		//ŷ����
    // ������x(-90��~90��), �����y(-180��~180��), �����z(-180��~180��)
    // short aacx, aacy, aacz;			//���ٶȴ�����ԭʼ����
    // short gyrox, gyroy, gyroz;		//������ԭʼ����
    // MPU_Write_Byte(MPU_INT_EN_REG, 0X01);




    
    while (1)
    {


        OLED_WriteCommand(0x40);	//������ʾ��ʼ��,0x40~0x7F
        OLED_WriteCommand(0xA1);	//�������ҷ���,0xA1����,0xA0���ҷ���
        OLED_WriteCommand(0xC8);	//�������·���,0xC8����,0xC0���·���


        // OLED_Clear();
        // MPU6050_Get_GXData(&GyroX);
        // // OLED_ShowNum(0, 16, Aacx, 8, OLED_8X16);
        // // OLED_ShowNum(0, 32, GyroX, 8, OLED_8X16);
        // // OLED_Printf(0, 16, OLED_8X16, "%d", Aacx);
        // OLED_Printf(0, 32, OLED_8X16, "%d", GyroX);
        // OLED_Update();
        // delay_ms(200);
        // OLED_transition(1);

        Time_page();            //ˢ�±���
        p = menu_Roll_event();	//��ȡ����

        if (XIUMIAN > 200)      //��ѭ�ȴ��ػ�ʱ��
        {
            p = -1;
            XIUMIAN = 0;
        }
        else { XIUMIAN++; }

        if (RAWA)               //����Ϩ��
        {
            MPU6050_Get_GXData(&GyroX);     //��ȡx������������  ��
            // MPU_Get_Accelerometer(&aacx, &aacy, &aacz);	    //�õ����ٶȴ���������
            // MPU_Get_Gyroscope(&gyrox, &gyroy, &gyroz);	    //�õ�����������
            // if (GyroX > 50 && GyroX < 0 && Aacx < 10000)
            if (GyroX > 50 && GyroX < 0)
            {
                p = -1;
            }
        }

        if (p == 1)             //�²�����
        {
            XIUMIAN = 0;
            OLED_transition(0);
            main_menu2();		//����˵�2

        }
        else if (p == -1 && WorkingMode == Normal)       //�ϲ����� Ϣ��
        {
            EXTI0_Enable();
            XIUMIAN = 0;
            POWER_OFF = 1;
            Power_OFF();    //�������
        }
        else if (p == -1 && WorkingMode == PowerSaving)  //ֹͣ
        {
            EXTI0_Enable();
            XIUMIAN = 0;
            // OLED_transition(0);
            POWER_OFF = 1;
            Power_OFF();    //�������

        }
        else if (p == -1 && WorkingMode == SuperSaving)  //����
        {
            XIUMIAN = 0;
            OLED_transition(0);
            POWER_OFF = 1;
            Power_OFF();    //�������

        }


        //��һ����Ϊ�˷�ֹ������������ȥ�жϰ���,��ֹ�󴥷�
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
            main_menu1();   //����˵�1
        }

    }


}


//�жϴ�����
void EXTI0_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line0) == SET)	        //�ⲿ�ж�0���ߴ������ж�,ҡ���²�
    {
        POWER_OFF = 0;  //ȡ������
        // OLED_Power(1);
        // OLED_transition(1);

        EXTI_ClearITPendingBit(EXTI_Line0);             //����жϱ�־λ
    }
}

extern u8 RAWAtri;          //̧�󴥷�ֵ
extern u16 RAWAtrigger;     //ʵ��̧�󴥷�ֵ
u8 Addtrigger = 0;          //�ֱ����ۼ�ֵ


void EXTI9_5_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line6) == SET)		//�ⲿ�ж�6���ߴ������ж�,����ʶ��ģ��
    {
        if (POWER_OFF)SystemInit();
        Voicelogo = 1;

        EXTI_ClearITPendingBit(EXTI_Line6);		//����жϱ�־λ
    }


    if (EXTI_GetITStatus(EXTI_Line7) == SET)		//�ⲿ�ж�7���ߴ������ж�,MPU6050
    {
        // MPU6050_Get_AXData(&Aacx);
        MPU6050_Get_GXData(&GyroX); //��ȡx������������

        if (GyroX > RAWAtrigger)//������ڴ���ֵ
        {
            OLED_Power(1);
            //OLED_transition(1);
            POWER_OFF = 0; //ȡ������
            Addtrigger = 0;
            RAWAtrigger = RAWAtri * 70;//�ָ�����ֵ
        }
        // else if (Pick_up)
        // {
        //     if (-30 < GyroX && GyroX < 30)//����ֱ�û�ж�
        //     {
        //         if (Addtrigger < 80)
        //         {
        //             Addtrigger++;       //�ۼ�80��,ʵ�ʴ�Լ5��
        //         }
        //         else RAWAtrigger = 5;   //��Ϊ�˶�����ģʽ,һ����⵽�е��˶�,���ϻ���
        //     }
        //     else Addtrigger = 0;
        // }

        EXTI_ClearITPendingBit(EXTI_Line7);		//����жϱ�־λ
    }

}

//���ڻ��ѵ��ⲿ�ж�,ͳһ����������
void STM32_rouse(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    //PA0��Key_Init();�����Ѿ����ù���,����ֱ�Ӵ�PA6��ʼ
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;	//SU-21tģ����
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	//MPU6050��
    GPIO_Init(GPIOB, &GPIO_InitStructure);


    /*AFIOѡ���ж�����*/
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0); //���ⲿ�жϵ�0����ӳ�䵽GPIOA   ������������
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource6); //���ⲿ�жϵ�6����ӳ�䵽GPIOA   SU-21tģ�黽������
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource7); //���ⲿ�жϵ�7����ӳ�䵽GPIOB   MPU6050��������

    /*EXTI��ʼ��*/
    EXTI_InitTypeDef EXTI_InitStructure;				     //����ṹ�����
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;			    //ָ���ⲿ�ж���ʹ��
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	   //ָ���ⲿ�ж���Ϊ�ж�ģʽ
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//ָ���ⲿ�ж���Ϊ�����ش���

    EXTI_InitStructure.EXTI_Line = EXTI_Line0;			//ѡ�������ⲿ�ж�
    EXTI_Init(&EXTI_InitStructure);					   //���ṹ���������EXTI_Init,����EXTI����

    EXTI_InitStructure.EXTI_Line = EXTI_Line6;	     //ѡ�������ⲿ�ж�
    EXTI_Init(&EXTI_InitStructure);					//���ṹ���������EXTI_Init,����EXTI����

    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  //ָ���ⲿ�ж���Ϊ�½��ش���
    EXTI_InitStructure.EXTI_Line = EXTI_Line7;			    //ѡ�������ⲿ�ж�
    EXTI_Init(&EXTI_InitStructure);						   //���ṹ���������EXTI_Init,����EXTI����


    /*NVIC����*/
    NVIC_InitTypeDef NVIC_InitStructure;				       //����ṹ�����
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  //ָ��NVIC��·ʹ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//ָ��NVIC��·����ռ���ȼ�Ϊ1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	    //ָ��NVIC��·����Ӧ���ȼ�Ϊ1

    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;	    //ѡ������NVIC��0��
    NVIC_Init(&NVIC_InitStructure);						    //����NVIC

    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_Init(&NVIC_InitStructure);
}


// ��ȡ������X������
void MPU6050_Get_GXData(s16* GyroX)
{
    u8 DataH, DataL;							 //�������ݸ�8λ�͵�8λ�ı���

    DataH = MPU_Read_Byte(MPU_GYRO_XOUTH_REG);  //��ȡ������X��ĸ�8λ����
    DataL = MPU_Read_Byte(MPU_GYRO_XOUTL_REG); //��ȡ������X��ĵ�8λ����
    *GyroX = (DataH << 8) | DataL;				 //����ƴ��,ͨ�������������

}

// ��ȡ���ٶȼ�X������
void MPU6050_Get_AXData(s16* Aacx)
{
    u8 DataH, DataL;							 // �������ݸ�8λ�͵�8λ�ı���

    DataH = MPU_Read_Byte(MPU_ACCEL_XOUTH_REG);  // ��ȡ���ٶȼ�X��ĸ�8λ����
    DataL = MPU_Read_Byte(MPU_ACCEL_XOUTL_REG);  // ��ȡ���ٶȼ�X��ĵ�8λ����
    *Aacx = (DataH << 8) | DataL;				 // ����ƴ��,ͨ�������������
}


// ����EXTI��0�ж�
void EXTI0_Disable(void)
{
    // ��� EXTI �ж�ʹ�ܼĴ����е� EXTI0 λ
    EXTI->IMR &= ~(1 << 0); // 0��ӦEXTI��0
}

// ����ʹ��EXTI��0�ж�
void EXTI0_Enable(void)
{
    // ���� EXTI �ж�ʹ�ܼĴ����е� EXTI0 λ
    EXTI->IMR |= (1 << 0); // 0��ӦEXTI��0
}











// void MPU6050_SelfTest(void)
// {
//     // �������ٶȼ��Բ�
//     MPU_Write_Byte(0x1C, 0xE0);  // ���� X, Y, Z ����ٶȼ��Բ�
//     delay_ms(20);                // �ȴ��Բ����
//     // ��ȡ���ٶȼ��Բ���Ӧ
//     int16_t ax, ay, az;
//     MPU_Get_Accelerometer(&ax, &ay, &az);
//     OLED_Clear();
//     // �Ƚ��Բ���Ӧ
//     if (abs(ax) > 0 || abs(ay) > 0 || abs(az) > 0)
//     {
//         OLED_Printf(0, 12, OLED_8X16, "Accelerometer failed!\n");
//     }
//     else
//     {
//         OLED_Printf(0, 12, OLED_8X16, "Accelerometer passed!\n");
//     }
//     OLED_Update();
//     // �رռ��ٶȼ��Բ�
//     MPU_Write_Byte(0x1C, 0x00);
//     delay_ms(3000);
//     OLED_Clear();
//     // �����������Բ�
//     MPU_Write_Byte(0x1B, 0xE0);  // ���� X, Y, Z ���������Բ�
//     delay_ms(20);  // �ȴ��Բ����
//     // ��ȡ�������Բ���Ӧ
//     int16_t gx, gy, gz;
//     MPU_Get_Gyroscope(&gx, &gy, &gz);
//     // �Ƚ��Բ���Ӧ
//     if (abs(gx) > 0 || abs(gy) > 0 || abs(gz) > 0)
//     {
//         OLED_Printf(0, 12, OLED_8X16, "Gyroscope failed!\n");
//     }
//     else {
//         OLED_Printf(0, 12, OLED_8X16, "Gyroscope passed!\n");
//     }
//     OLED_Update();
//     // �ر��������Բ�
//     MPU_Write_Byte(0x1B, 0x00);
//     delay_ms(3000);
// }


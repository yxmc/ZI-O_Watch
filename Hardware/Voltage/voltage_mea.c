#include "stm32f10x.h"               
#include "voltage_mea.h"
#include "AD.h"
#include "OLED.h"     
#include "Delay.h"  
#include "menu.h"

#include "Time_page.h"


float RTI = 9.471;  //电阻分压系数
extern  float VIN;

//电压检测
void voltage_mt(void)
{
    s8 p, i;
    float voltage;
    float liangc;                           //量程
    u16 ADValue;
    float GP = 0, GP_target;                //条条长度,目标条条长度
    while (1)
    {

        OLED_WriteCommand(0x40);	//设置显示开始行,0x40~0x7F

        OLED_WriteCommand(0xA1);	//设置左右方向,0xA1正常,0xA0左右反置

        OLED_WriteCommand(0xC8);	//设置上下方向,0xC8正常,0xC0上下反置


        //计算电压

        OLED_Clear();

        Time_page_BAT(90, 0);
        Time_page_small_Timer(0, 0);


        if (i > 25)
        {
            ADValue = AD_GetValue(ADC_Channel_9);
            liangc = (float)ADValue / 4095 * 100;
            voltage = (((float)ADValue * VIN) / 4095) * RTI;
            i = 0;
        }
        else i++;

        OLED_ShowFloatNum(33, 16, voltage, 2, 3, OLED_8X16);
        OLED_ShowASCII(89, 24, "V", OLED_6X8);

        OLED_ShowFloatNum(0, 56, RTI, 1, 3, OLED_6X8);

        OLED_ShowASCII(50, 56, "Critical:   %", OLED_6X8);
        OLED_ShowNum(104, 56, liangc, 3, OLED_6X8);

        GP_target = voltage * 4;
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
        if (GP > 80) GP = 80;


        OLED_ClearArea(24, 42, 80, 3);
        OLED_DrawRectangle(24, 42, GP, 3, OLED_FILLED);

        OLED_DrawLine(24, 49, 104, 49);
        OLED_DrawLine(24, 47, 24, 49);
        OLED_DrawLine(44, 47, 44, 49);
        OLED_DrawLine(64, 47, 64, 49);
        OLED_DrawLine(84, 47, 84, 49);
        OLED_DrawLine(104, 47, 104, 49);

        OLED_Update();
        OLED_transition(1);

        p = menu_Roll_event();
        if (p == 1) { RTI = RTI + 0.001; }
        else if (p == -1) { RTI = RTI - 0.001; }

        p = menu_Enter_event();
        if (p)
        {

            OLED_transition(0);

            u32 temp;

            /*备份寄存器访问使能*/
            PWR_BackupAccessCmd(ENABLE);

            u16 high, low;
            memcpy(&temp, &RTI, sizeof(float));
            high = (temp >> 16) & 0xFFFF; // 提取高16位  
            low = temp & 0xFFFF;          // 提取低16位 

            BKP_WriteBackupRegister(BKP_DR6, high);
            BKP_WriteBackupRegister(BKP_DR7, low);	//将RTI存入bkp寄存器
            return;
        }

    }


}





//示波器//


u8 key = 1, onkey = 0;//功能选择判断标志位
u8 Sampling = 1;//是否开启采样标志位1开启,0停止
u8 Samplingmode = 1;//采样模式,1为continuous Mode连续采样,0为Single Mode单次采样
float RangeUP = 5, RangeDOWH = 0;//电压量程范围
float Threshold_voltage = 2.1;//触发阈值电压
u8 Trigger_Slope = 1;  //触发条件,1为上升沿,0为下降沿
u8 Fre = 4, upFre;   //采样时间选择标志位:1=每格10us,2=每格20us,3=每格50us,4=每格100us,5=每格200us,6=每格500us,7=每格1ms,8=每格2ms,9=每格5ms,10=每格10ms,11=每格20ms,12=每格50ms,13=每格100ms,14=每格200ms,15=每格500ms	
float maxvoltage = 0, littlevoltage = 25;//已检测到的最大最小电压
u8 WaP = 0;   //波形在屏幕上的位置0—128
u8 edge = 1; //开始寻找上升沿或下降沿标志位

u16 WaveForm[250];//存放波形数据的数组(原始AD值)//实际上只用了前200个位置,后面的位置是防止可能采样太快刹不住车?

void Draw_Waveform(void)//根据数组以及电压量程范围绘制波形图
{
    u8 WZ;
    float UPvoltage, voltage;
    float Difference_value = RangeUP - RangeDOWH;

    maxvoltage = 0, littlevoltage = 25;//重置最大最小值

    if (edge)
    {
        UPvoltage = (((float)WaveForm[25] * VIN) / 4095) * RTI;//计算电压
        for (WZ = 26;WZ < 100;WZ++)//寻找上升沿或下降沿
        {

            voltage = (((float)WaveForm[WZ + WaP] * VIN) / 4095) * RTI;//计算电压
            if (Trigger_Slope)
            {
                // 找到第一个上升沿	
                if (UPvoltage <= Threshold_voltage && voltage > Threshold_voltage) {
                    WaP = WZ;
                    edge = 0;

                    break;
                }
            }
            else
            {
                // 找到第一个下降沿  
                if (UPvoltage > Threshold_voltage && voltage <= Threshold_voltage) {
                    WaP = WZ;
                    edge = 0;
                    break;
                }
            }
            UPvoltage = voltage;
        }
        if (!edge)//上升沿或下降沿被找到
        {
            if (WaP > 25)WaP = WaP - 25;
            else WaP = 0;
            if (Samplingmode) Sampling = 1;//如果需要连续采样  
            else Sampling = 0;
        }
    }


    UPvoltage = (((float)WaveForm[0] * VIN) / 4095) * RTI;//计算电压
    if (UPvoltage < littlevoltage)littlevoltage = UPvoltage;//插一个检测最小值
    if (UPvoltage >= RangeUP)UPvoltage = RangeUP;
    else if (UPvoltage <= RangeDOWH)UPvoltage = RangeDOWH;//限幅

    UPvoltage = ((float)(UPvoltage - RangeDOWH) / Difference_value) * 61;//计算该数据在图像中的位置
    UPvoltage = 62 - UPvoltage;//将数据反向,因为显示屏像素计算是从上到下
    for (WZ = 1;WZ < 100;WZ++)
    {

        voltage = (((float)WaveForm[WZ + WaP] * VIN) / 4095) * RTI;//计算电压

        if (voltage < littlevoltage)littlevoltage = voltage;//插一个检测最小值
        if (voltage > maxvoltage)maxvoltage = voltage;//插一个检测最大值

        if (voltage >= RangeUP)voltage = RangeUP;
        else if (voltage <= RangeDOWH)voltage = RangeDOWH;//限幅

        voltage = ((float)(voltage - RangeDOWH) / Difference_value) * 61;//计算该数据在图像中的位置
        voltage = 62 - voltage;//将数据反向,因为显示屏像素计算是从上到下

        OLED_DrawLine(WZ + 25, UPvoltage, WZ + 26, voltage);//显示这段

        UPvoltage = voltage;
    }

}



void voltage_waveform(void)
{
    s8 p;


    for (u8 i = 0;i < 200;i++) { WaveForm[i] = 0; }

    Voltage_Waveform_Init();


    TIM_Cmd(TIM4, DISABLE); // 关闭定时器4, 防止干扰

    // Fre = 10;
    //	Adc_Init();

    OLED_Clear();				//清空显存数组
    OLED_Update();			   //更新显示,清屏

    u8 oledx = 0, oledi = 1; //绘制虚线用

    //	float lower_limit = 1000;   // 下限  
    //    float upper_limit =1145; // 上限
    //	
    //	// 向数组写入一个正弦波形数据测试
    //    for (int i = 0; i < 1200; i++) {  
    //        // 计算正弦波,范围从 0 到 2π  
    //        float normalized_value = ( sin(2 * 3.14 * (i / (float)(200 - 1))) + 1) / 2; // [-1, 1] 转换为 [0, 1]  
    //        WaveForm[i/6] = (u16)(lower_limit + normalized_value * (upper_limit - lower_limit)); // 缩放到 [lower_limit, upper_limit]  
    //    }  


    while (1)
    {
        OLED_WriteCommand(0x40);	//设置显示开始行,0x40~0x7F
        OLED_WriteCommand(0xA1);	//设置左右方向,0xA1正常,0xA0左右反置
        OLED_WriteCommand(0xC8);	//设置上下方向,0xC8正常,0xC0上下反置

        OLED_Clear();


        if (key != 8)
        {
            OLED_ShowFloatNum(22, 55, littlevoltage, 2, 1, OLED_6X8);
            OLED_ClearArea(22, 55, 6, 8);
            OLED_ShowFloatNum(52, 55, maxvoltage, 2, 1, OLED_6X8);
            OLED_ClearArea(52, 55, 6, 8);
            OLED_ShowASCII(52, 55, "-", OLED_6X8);
        }

        OLED_DrawLine(26, 0, 26, 63);OLED_DrawLine(26, 0, 29, 0);OLED_DrawLine(26, 63, 29, 63);
        OLED_DrawLine(127, 0, 127, 63);OLED_DrawLine(124, 0, 127, 0);OLED_DrawLine(124, 63, 127, 63);
        OLED_DrawLine(49, 0, 55, 0);OLED_DrawLine(49, 63, 55, 63);OLED_DrawLine(74, 0, 80, 0);OLED_DrawLine(74, 63, 80, 63);OLED_DrawLine(99, 0, 105, 0);OLED_DrawLine(99, 63, 105, 63);


        for (oledx = 0;oledx < 100;oledx++) { if (oledi) OLED_DrawPoint(27 + oledx, 31);oledi = !oledi; }
        for (oledx = 0;oledx < 64;oledx++) { if (oledi) OLED_DrawPoint(52, oledx); oledi = !oledi; }
        for (oledx = 0;oledx < 64;oledx++) { if (oledi)OLED_DrawPoint(77, oledx);   oledi = !oledi; }
        for (oledx = 0;oledx < 64;oledx++) { if (oledi)OLED_DrawPoint(102, oledx);  oledi = !oledi; }


        OLED_ShowFloatNum(250, 0, RangeUP, 2, 1, OLED_6X8);	//这里可能会提示警告,可以不管它	

        if (Sampling)OLED_ShowASCII(0, 9, "-ON-", OLED_6X8);
        else OLED_ShowASCII(0, 9, "STOP", OLED_6X8);


        if (Samplingmode)OLED_ShowASCII(0, 18, "Co_", OLED_6X8);
        else OLED_ShowASCII(0, 18, "Si_", OLED_6X8);

        OLED_ShowNum(0, 27, WaP, 2, OLED_6X8);

        if (Trigger_Slope)OLED_ShowASCII(0, 38, "-UP-", OLED_6X8);
        else OLED_ShowASCII(0, 38, "DOWH", OLED_6X8);

        OLED_ShowFloatNum(250, 47, Threshold_voltage, 2, 1, OLED_6X8);	//这里可能会提示警告,可以不管它	
        OLED_ShowFloatNum(250, 56, RangeDOWH, 2, 1, OLED_6X8);	//这里可能会提示警告,可以不管它


        if (key != 8)
        {
            switch (Fre)
            {
            case 1:OLED_ShowASCII(103, 55, "50us", OLED_6X8);break;
            case 2:OLED_ShowASCII(97, 55, "100us", OLED_6X8);break;
            case 3:OLED_ShowASCII(97, 55, "200us", OLED_6X8);break;
            case 4:OLED_ShowASCII(97, 55, "500us", OLED_6X8);break;
            case 5:OLED_ShowASCII(109, 55, "1ms", OLED_6X8);break;
            case 6:OLED_ShowASCII(109, 55, "2ms", OLED_6X8);break;
            case 7:OLED_ShowASCII(109, 55, "5ms", OLED_6X8);break;
            case 8:OLED_ShowASCII(103, 55, "10ms", OLED_6X8);break;
            case 9:OLED_ShowASCII(103, 55, "20ms", OLED_6X8);break;
            case 10:OLED_ShowASCII(103, 55, "50ms", OLED_6X8);break;
            case 11:OLED_ShowASCII(97, 55, "100ms", OLED_6X8);break;
            case 12:OLED_ShowASCII(97, 55, "200ms", OLED_6X8);break;
            case 13:OLED_ShowASCII(97, 55, "500ms", OLED_6X8);break;
            }
        }
        if (onkey)
        {
            switch (key)
            {
            case 1:OLED_ReverseArea(97, 55, 30, 8);break;
            case 2:OLED_ReverseArea(0, 0, 25, 8);break;
            case 3:OLED_ReverseArea(0, 9, 25, 8);break;
            case 4:OLED_ReverseArea(0, 18, 19, 8);break;
            case 5:OLED_ReverseArea(0, 38, 25, 8);break;
            case 6:OLED_ReverseArea(0, 47, 25, 8);break;
            case 7:OLED_ReverseArea(0, 56, 25, 8);break;
            case 8:OLED_DrawRectangle(26, 0, 102, 64, OLED_UNFILLED);break;
            }
        }
        else
        {
            switch (key)
            {
            case 1:OLED_DrawRectangle(96, 54, 32, 10, OLED_UNFILLED);break;
            case 2:OLED_DrawRectangle(0, 0, 24, 8, OLED_UNFILLED);break;
            case 3:OLED_DrawRectangle(0, 9, 24, 8, OLED_UNFILLED);break;
            case 4:OLED_DrawRectangle(0, 18, 18, 8, OLED_UNFILLED);break;
            case 5:OLED_DrawRectangle(0, 38, 24, 8, OLED_UNFILLED);break;
            case 6:OLED_DrawRectangle(0, 47, 24, 8, OLED_UNFILLED);break;
            case 7:OLED_DrawRectangle(0, 56, 24, 8, OLED_UNFILLED);break;
            case 8:OLED_DrawRectangle(27, 1, 100, 62, OLED_UNFILLED);break;
            }
        }

        if (Sampling)
        {
            Trigger_sampling();//开始采样

            edge = 1;
            WaP = 0;
        }
        Draw_Waveform();//根据数组以及电压量程范围绘制波形图


        OLED_transition(1);
        OLED_Update();
        p = menu_Roll_event();
        if (p == -1)
        {
            if (onkey)
            {
                switch (key)
                {

                case 1:if (Fre < 13)Fre++;break;
                case 2:if (RangeUP < 25)RangeUP += 0.1;break;
                case 3:Sampling = 0;break;
                case 4:Samplingmode = 0;break;
                case 5:Trigger_Slope = 1;break;
                case 6:if (Threshold_voltage < 25)Threshold_voltage += 0.1;break;
                case 7:if (RangeDOWH < 25)RangeDOWH += 0.1;if (RangeDOWH >= RangeUP)RangeDOWH -= 0.1;break;
                case 8:if (WaP > 0)WaP--;break;
                }
            }
            else	key--; if (key < 1)key = 8;


        }
        else if (p == 1)
        {
            if (onkey)
            {
                switch (key)
                {
                case 1:if (Fre > 1)Fre--;break;
                case 2:if (RangeUP > 0)RangeUP -= 0.1;if (RangeUP <= RangeDOWH)RangeUP += 0.1;break;
                case 3:Sampling = 1;break;
                case 4:Samplingmode = 1;break;
                case 5:Trigger_Slope = 0;break;
                case 6:if (Threshold_voltage > 0)Threshold_voltage -= 0.1;break;
                case 7:if (RangeDOWH > 0.1)RangeDOWH -= 0.1;if (!(RangeDOWH > 0.1))RangeDOWH = 0;break;
                case 8:if (WaP < 100)WaP++;break;
                }
            }
            else	key++; if (key > 8)key = 1;
        }
        p = menu_Enter_event();
        if (p == 1)
        {
            onkey = !onkey;
        }
        else if (p == 2)
        {
            //把该关的关了
            TIM_Cmd(TIM2, DISABLE);
            TIM_CtrlPWMOutputs(TIM2, DISABLE);
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE);

            DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, DISABLE);
            DMA_Cmd(DMA1_Channel1, DISABLE);
            RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, DISABLE);

            ADC_ExternalTrigConvCmd(ADC1, DISABLE);

            //再把该打开的打开
            AD_Init();

            if (!Step_Flag)                 //计步
            {
                TIM_Cmd(TIM4, DISABLE);
            }


            //现在获取一下通道内容,要不然等一下会出问题
            // u16 ADValue;
            // ADValue = AD_GetValue(ADC_Channel_8);
            // ADValue = AD_GetValue(ADC_Channel_9);
            AD_GetValue(ADC_Channel_8);
            AD_GetValue(ADC_Channel_9);

            OLED_transition(0);
            return;
        }




    }


}
//1=每格10us,2=每格20us,3=每格50us,4=每格100us,5=每格200us,6=每格500us,7=每格1ms,8=每格2ms,9=每格5ms,10=每格10ms,11=每格20ms,12=每格50ms,13=每格100ms,14=每格200ms,15=每格500ms	
u8 DMA_accomplish = 1;
u16 TIMPSC = 72 - 1, TIMARR = 30000 - 1, TIMCCR = 1000;

//触发一次采样,会自动根据设置的采样频率来采样
void Trigger_sampling(void)
{

    //如果分频改变
    if (upFre != Fre)
    {
        switch (Fre)
        {
        case 1: TIMPSC = 72 - 1;TIMARR = 2 - 1;TIMCCR = 1;break; //5us
        case 2: TIMPSC = 72 - 1;TIMARR = 4 - 1;TIMCCR = 2;break; //10us
        case 3: TIMPSC = 72 - 1;TIMARR = 8 - 1;TIMCCR = 4;break; //20us
        case 4: TIMPSC = 72 - 1;TIMARR = 20 - 1;TIMCCR = 10;break; //50us
        case 5: TIMPSC = 72 - 1;TIMARR = 40 - 1;TIMCCR = 20;break; //0.1ms 100us
        case 6: TIMPSC = 72 - 1;TIMARR = 80 - 1;TIMCCR = 40;break; //0.2ms 200us
        case 7: TIMPSC = 72 - 1;TIMARR = 200 - 1;TIMCCR = 100;break; //0.5ms 500us
        case 8: TIMPSC = 72 - 1;TIMARR = 400 - 1;TIMCCR = 200;break; //1ms
        case 9: TIMPSC = 72 - 1;TIMARR = 800 - 1;TIMCCR = 400;break; //2ms
        case 10:TIMPSC = 72 - 1;TIMARR = 2000 - 1;TIMCCR = 1000;break; //5ms
        case 11:TIMPSC = 72 - 1;TIMARR = 4000 - 1;TIMCCR = 2000;break; //10ms
        case 12:TIMPSC = 72 - 1;TIMARR = 8000 - 1;TIMCCR = 4000;break; //20ms
        case 13:TIMPSC = 72 - 1;TIMARR = 20000 - 1;TIMCCR = 10000;break; //50ms
        }				//这里本来应该是间隔(微秒)时间对应重装值的,因为72Mhz主频分72后=1Mhz=1微秒,所以100us=100ARR
                       //但是不知道为什么间隔时间不正常,测量出来的都少了60%,变成了100us=40ARR,,还是正比例关系,搞了很久也搞不明白,干脆就直接放它这样好了
        //设置预分频值
        TIM_PrescalerConfig(TIM2, TIMPSC, TIM_PSCReloadMode_Immediate);
        //设置AAR自动重装载值
        TIM_SetAutoreload(TIM2, TIMARR);
        //设置CCR捕获/比较寄存器值
        TIM_SetCompare2(TIM2, TIMCCR);
        upFre = Fre;

    }

    if (DMA_accomplish == 1)
    {
        if (Fre < 10)
        {
            DMA_Cmd(DMA1_Channel1, DISABLE);					//DMA失能,在写入传输计数器之前,需要DMA暂停工作
            DMA_SetCurrDataCounter(DMA1_Channel1, 200);	        //写入传输计数器,指定将要转运的次数
            DMA_Cmd(DMA1_Channel1, ENABLE);						//DMA使能,开始工作	
        }
        else
        {
            DMA_Cmd(DMA1_Channel1, DISABLE);					//DMA失能,在写入传输计数器之前,需要DMA暂停工作
            DMA_SetCurrDataCounter(DMA1_Channel1, 125);	        //写入传输计数器,指定将要转运的次数
            DMA_Cmd(DMA1_Channel1, ENABLE);						//DMA使能,开始工作	
            for (u8 i = 125;i < 200;i++) { WaveForm[i] = 0; }
        }
        DMA_accomplish = 0;		   //清除DMA工作标志位
    }


    if (Fre < 10)
        while (!DMA_accomplish)//等待DMA工作完成
        {
            delay_ms(1);
        }


}

#define ADC1_DR_Address    ((u32)0x4001244C)		//ADC1的地址
//示波器所需外设初始化
void Voltage_Waveform_Init(void)
{
    //***************定时器2初始化****************//	
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 		//时钟使能

    TIM_TimeBaseStructure.TIM_Period = 200; 		//自动重装值,随便先给一个,反正之后要改
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1; 			//预分频值,也随便先给一个,反正之后要改
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 		//设置时钟分割
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 		//TIM向上计数模式
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);			//根据指定的参数初始化TIMx的时间基数单位

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;		//选择定时器模式:TIM脉冲宽度调制模式1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;		//比较输出使能
    TIM_OCInitStructure.TIM_Pulse = 3;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;		//输出极性:TIM输出比较极性低
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);		//初始化外设TIM2_CH2

    TIM_Cmd(TIM2, ENABLE); 			//使能TIMx
    TIM_CtrlPWMOutputs(TIM2, ENABLE);




    //***************DMA初始化****************//	
    DMA_InitTypeDef DMA_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	  			//使能ADC1通道时钟

    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;         //ADC1地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&WaveForm;         //内存地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                  //方向(从外设到内存)
    DMA_InitStructure.DMA_BufferSize = 200;                             //传输内容的大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;    //外设地址固定
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;             //内存地址不固定
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //外设数据单位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;         //内存数据单位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                               //DMA模式:循环传输
    DMA_InitStructure.DMA_Priority = DMA_Priority_High; 		//优先级:高
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;   		//禁止内存到内存的传输
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);  //配置DMA1

    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);		//使能传输完成中断

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DMA_Cmd(DMA1_Channel1, ENABLE);


    //***************ADC1初始化****************//	
    ADC_InitTypeDef ADC_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	  //使能ADC1通道时钟

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; 			//独立ADC模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;  			//关闭扫描方式
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;			//关闭连续转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;   	//使用外部触发模式
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 			//采集数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 1; 			//要转换的通道数目
    ADC_Init(ADC1, &ADC_InitStructure);

    RCC_ADCCLKConfig(RCC_PCLK2_Div6);				//配置ADC时钟,为PCLK2的6分频,即12Hz
    ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 1, ADC_SampleTime_7Cycles5);		//配置ADC1通道6为7.5个采样周期 

    //使能ADC、DMA
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);				//复位校准寄存器
    while (ADC_GetResetCalibrationStatus(ADC1));				//等待校准寄存器复位完成

    ADC_StartCalibration(ADC1);				//ADC校准
    while (ADC_GetCalibrationStatus(ADC1));				//等待校准完成

    ADC_ExternalTrigConvCmd(ADC1, ENABLE);		//设置外部触发模式使能	
}

//中断处理函数
void DMA1_Channel1_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA1_IT_TC1) != RESET)
    {

        DMA_accomplish = 1;	//DMA传输完成

        DMA_ClearITPendingBit(DMA1_IT_TC1);
    }
}


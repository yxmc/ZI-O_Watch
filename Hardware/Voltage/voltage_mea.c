#include "stm32f10x.h"               
#include "voltage_mea.h"
#include "AD.h"
#include "OLED.h"     
#include "Delay.h"  
#include "menu.h"

#include "Time_page.h"


float RTI = 9.471;  //�����ѹϵ��
extern  float VIN;

//��ѹ���
void voltage_mt(void)
{
    s8 p, i;
    float voltage;
    float liangc;                           //����
    u16 ADValue;
    float GP = 0, GP_target;                //��������,Ŀ����������
    while (1)
    {

        OLED_WriteCommand(0x40);	//������ʾ��ʼ��,0x40~0x7F

        OLED_WriteCommand(0xA1);	//�������ҷ���,0xA1����,0xA0���ҷ���

        OLED_WriteCommand(0xC8);	//�������·���,0xC8����,0xC0���·���


        //�����ѹ

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

            /*���ݼĴ�������ʹ��*/
            PWR_BackupAccessCmd(ENABLE);

            u16 high, low;
            memcpy(&temp, &RTI, sizeof(float));
            high = (temp >> 16) & 0xFFFF; // ��ȡ��16λ  
            low = temp & 0xFFFF;          // ��ȡ��16λ 

            BKP_WriteBackupRegister(BKP_DR6, high);
            BKP_WriteBackupRegister(BKP_DR7, low);	//��RTI����bkp�Ĵ���
            return;
        }

    }


}





//ʾ����//


u8 key = 1, onkey = 0;//����ѡ���жϱ�־λ
u8 Sampling = 1;//�Ƿ���������־λ1����,0ֹͣ
u8 Samplingmode = 1;//����ģʽ,1Ϊcontinuous Mode��������,0ΪSingle Mode���β���
float RangeUP = 5, RangeDOWH = 0;//��ѹ���̷�Χ
float Threshold_voltage = 2.1;//������ֵ��ѹ
u8 Trigger_Slope = 1;  //��������,1Ϊ������,0Ϊ�½���
u8 Fre = 4, upFre;   //����ʱ��ѡ���־λ:1=ÿ��10us,2=ÿ��20us,3=ÿ��50us,4=ÿ��100us,5=ÿ��200us,6=ÿ��500us,7=ÿ��1ms,8=ÿ��2ms,9=ÿ��5ms,10=ÿ��10ms,11=ÿ��20ms,12=ÿ��50ms,13=ÿ��100ms,14=ÿ��200ms,15=ÿ��500ms	
float maxvoltage = 0, littlevoltage = 25;//�Ѽ�⵽�������С��ѹ
u8 WaP = 0;   //��������Ļ�ϵ�λ��0��128
u8 edge = 1; //��ʼѰ�������ػ��½��ر�־λ

u16 WaveForm[250];//��Ų������ݵ�����(ԭʼADֵ)//ʵ����ֻ����ǰ200��λ��,�����λ���Ƿ�ֹ���ܲ���̫��ɲ��ס��?

void Draw_Waveform(void)//���������Լ���ѹ���̷�Χ���Ʋ���ͼ
{
    u8 WZ;
    float UPvoltage, voltage;
    float Difference_value = RangeUP - RangeDOWH;

    maxvoltage = 0, littlevoltage = 25;//���������Сֵ

    if (edge)
    {
        UPvoltage = (((float)WaveForm[25] * VIN) / 4095) * RTI;//�����ѹ
        for (WZ = 26;WZ < 100;WZ++)//Ѱ�������ػ��½���
        {

            voltage = (((float)WaveForm[WZ + WaP] * VIN) / 4095) * RTI;//�����ѹ
            if (Trigger_Slope)
            {
                // �ҵ���һ��������	
                if (UPvoltage <= Threshold_voltage && voltage > Threshold_voltage) {
                    WaP = WZ;
                    edge = 0;

                    break;
                }
            }
            else
            {
                // �ҵ���һ���½���  
                if (UPvoltage > Threshold_voltage && voltage <= Threshold_voltage) {
                    WaP = WZ;
                    edge = 0;
                    break;
                }
            }
            UPvoltage = voltage;
        }
        if (!edge)//�����ػ��½��ر��ҵ�
        {
            if (WaP > 25)WaP = WaP - 25;
            else WaP = 0;
            if (Samplingmode) Sampling = 1;//�����Ҫ��������  
            else Sampling = 0;
        }
    }


    UPvoltage = (((float)WaveForm[0] * VIN) / 4095) * RTI;//�����ѹ
    if (UPvoltage < littlevoltage)littlevoltage = UPvoltage;//��һ�������Сֵ
    if (UPvoltage >= RangeUP)UPvoltage = RangeUP;
    else if (UPvoltage <= RangeDOWH)UPvoltage = RangeDOWH;//�޷�

    UPvoltage = ((float)(UPvoltage - RangeDOWH) / Difference_value) * 61;//�����������ͼ���е�λ��
    UPvoltage = 62 - UPvoltage;//�����ݷ���,��Ϊ��ʾ�����ؼ����Ǵ��ϵ���
    for (WZ = 1;WZ < 100;WZ++)
    {

        voltage = (((float)WaveForm[WZ + WaP] * VIN) / 4095) * RTI;//�����ѹ

        if (voltage < littlevoltage)littlevoltage = voltage;//��һ�������Сֵ
        if (voltage > maxvoltage)maxvoltage = voltage;//��һ��������ֵ

        if (voltage >= RangeUP)voltage = RangeUP;
        else if (voltage <= RangeDOWH)voltage = RangeDOWH;//�޷�

        voltage = ((float)(voltage - RangeDOWH) / Difference_value) * 61;//�����������ͼ���е�λ��
        voltage = 62 - voltage;//�����ݷ���,��Ϊ��ʾ�����ؼ����Ǵ��ϵ���

        OLED_DrawLine(WZ + 25, UPvoltage, WZ + 26, voltage);//��ʾ���

        UPvoltage = voltage;
    }

}



void voltage_waveform(void)
{
    s8 p;


    for (u8 i = 0;i < 200;i++) { WaveForm[i] = 0; }

    Voltage_Waveform_Init();


    TIM_Cmd(TIM4, DISABLE); // �رն�ʱ��4, ��ֹ����

    // Fre = 10;
    //	Adc_Init();

    OLED_Clear();				//����Դ�����
    OLED_Update();			   //������ʾ,����

    u8 oledx = 0, oledi = 1; //����������

    //	float lower_limit = 1000;   // ����  
    //    float upper_limit =1145; // ����
    //	
    //	// ������д��һ�����Ҳ������ݲ���
    //    for (int i = 0; i < 1200; i++) {  
    //        // �������Ҳ�,��Χ�� 0 �� 2��  
    //        float normalized_value = ( sin(2 * 3.14 * (i / (float)(200 - 1))) + 1) / 2; // [-1, 1] ת��Ϊ [0, 1]  
    //        WaveForm[i/6] = (u16)(lower_limit + normalized_value * (upper_limit - lower_limit)); // ���ŵ� [lower_limit, upper_limit]  
    //    }  


    while (1)
    {
        OLED_WriteCommand(0x40);	//������ʾ��ʼ��,0x40~0x7F
        OLED_WriteCommand(0xA1);	//�������ҷ���,0xA1����,0xA0���ҷ���
        OLED_WriteCommand(0xC8);	//�������·���,0xC8����,0xC0���·���

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


        OLED_ShowFloatNum(250, 0, RangeUP, 2, 1, OLED_6X8);	//������ܻ���ʾ����,���Բ�����	

        if (Sampling)OLED_ShowASCII(0, 9, "-ON-", OLED_6X8);
        else OLED_ShowASCII(0, 9, "STOP", OLED_6X8);


        if (Samplingmode)OLED_ShowASCII(0, 18, "Co_", OLED_6X8);
        else OLED_ShowASCII(0, 18, "Si_", OLED_6X8);

        OLED_ShowNum(0, 27, WaP, 2, OLED_6X8);

        if (Trigger_Slope)OLED_ShowASCII(0, 38, "-UP-", OLED_6X8);
        else OLED_ShowASCII(0, 38, "DOWH", OLED_6X8);

        OLED_ShowFloatNum(250, 47, Threshold_voltage, 2, 1, OLED_6X8);	//������ܻ���ʾ����,���Բ�����	
        OLED_ShowFloatNum(250, 56, RangeDOWH, 2, 1, OLED_6X8);	//������ܻ���ʾ����,���Բ�����


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
            Trigger_sampling();//��ʼ����

            edge = 1;
            WaP = 0;
        }
        Draw_Waveform();//���������Լ���ѹ���̷�Χ���Ʋ���ͼ


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
            //�ѸùصĹ���
            TIM_Cmd(TIM2, DISABLE);
            TIM_CtrlPWMOutputs(TIM2, DISABLE);
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE);

            DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, DISABLE);
            DMA_Cmd(DMA1_Channel1, DISABLE);
            RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, DISABLE);

            ADC_ExternalTrigConvCmd(ADC1, DISABLE);

            //�ٰѸô򿪵Ĵ�
            AD_Init();

            if (!Step_Flag)                 //�Ʋ�
            {
                TIM_Cmd(TIM4, DISABLE);
            }


            //���ڻ�ȡһ��ͨ������,Ҫ��Ȼ��һ�»������
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
//1=ÿ��10us,2=ÿ��20us,3=ÿ��50us,4=ÿ��100us,5=ÿ��200us,6=ÿ��500us,7=ÿ��1ms,8=ÿ��2ms,9=ÿ��5ms,10=ÿ��10ms,11=ÿ��20ms,12=ÿ��50ms,13=ÿ��100ms,14=ÿ��200ms,15=ÿ��500ms	
u8 DMA_accomplish = 1;
u16 TIMPSC = 72 - 1, TIMARR = 30000 - 1, TIMCCR = 1000;

//����һ�β���,���Զ��������õĲ���Ƶ��������
void Trigger_sampling(void)
{

    //�����Ƶ�ı�
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
        }				//���ﱾ��Ӧ���Ǽ��(΢��)ʱ���Ӧ��װֵ��,��Ϊ72Mhz��Ƶ��72��=1Mhz=1΢��,����100us=100ARR
                       //���ǲ�֪��Ϊʲô���ʱ�䲻����,���������Ķ�����60%,�����100us=40ARR,,������������ϵ,���˺ܾ�Ҳ�㲻����,�ɴ��ֱ�ӷ�����������
        //����Ԥ��Ƶֵ
        TIM_PrescalerConfig(TIM2, TIMPSC, TIM_PSCReloadMode_Immediate);
        //����AAR�Զ���װ��ֵ
        TIM_SetAutoreload(TIM2, TIMARR);
        //����CCR����/�ȽϼĴ���ֵ
        TIM_SetCompare2(TIM2, TIMCCR);
        upFre = Fre;

    }

    if (DMA_accomplish == 1)
    {
        if (Fre < 10)
        {
            DMA_Cmd(DMA1_Channel1, DISABLE);					//DMAʧ��,��д�봫�������֮ǰ,��ҪDMA��ͣ����
            DMA_SetCurrDataCounter(DMA1_Channel1, 200);	        //д�봫�������,ָ����Ҫת�˵Ĵ���
            DMA_Cmd(DMA1_Channel1, ENABLE);						//DMAʹ��,��ʼ����	
        }
        else
        {
            DMA_Cmd(DMA1_Channel1, DISABLE);					//DMAʧ��,��д�봫�������֮ǰ,��ҪDMA��ͣ����
            DMA_SetCurrDataCounter(DMA1_Channel1, 125);	        //д�봫�������,ָ����Ҫת�˵Ĵ���
            DMA_Cmd(DMA1_Channel1, ENABLE);						//DMAʹ��,��ʼ����	
            for (u8 i = 125;i < 200;i++) { WaveForm[i] = 0; }
        }
        DMA_accomplish = 0;		   //���DMA������־λ
    }


    if (Fre < 10)
        while (!DMA_accomplish)//�ȴ�DMA�������
        {
            delay_ms(1);
        }


}

#define ADC1_DR_Address    ((u32)0x4001244C)		//ADC1�ĵ�ַ
//ʾ�������������ʼ��
void Voltage_Waveform_Init(void)
{
    //***************��ʱ��2��ʼ��****************//	
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 		//ʱ��ʹ��

    TIM_TimeBaseStructure.TIM_Period = 200; 		//�Զ���װֵ,����ȸ�һ��,����֮��Ҫ��
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1; 			//Ԥ��Ƶֵ,Ҳ����ȸ�һ��,����֮��Ҫ��
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 		//����ʱ�ӷָ�
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 		//TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);			//����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;		//ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;		//�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_Pulse = 3;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;		//�������:TIM����Ƚϼ��Ե�
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);		//��ʼ������TIM2_CH2

    TIM_Cmd(TIM2, ENABLE); 			//ʹ��TIMx
    TIM_CtrlPWMOutputs(TIM2, ENABLE);




    //***************DMA��ʼ��****************//	
    DMA_InitTypeDef DMA_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	  			//ʹ��ADC1ͨ��ʱ��

    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;         //ADC1��ַ
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&WaveForm;         //�ڴ��ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                  //����(�����赽�ڴ�)
    DMA_InitStructure.DMA_BufferSize = 200;                             //�������ݵĴ�С
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;    //�����ַ�̶�
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;             //�ڴ��ַ���̶�
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //�������ݵ�λ
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;         //�ڴ����ݵ�λ
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                               //DMAģʽ:ѭ������
    DMA_InitStructure.DMA_Priority = DMA_Priority_High; 		//���ȼ�:��
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;   		//��ֹ�ڴ浽�ڴ�Ĵ���
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);  //����DMA1

    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);		//ʹ�ܴ�������ж�

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DMA_Cmd(DMA1_Channel1, ENABLE);


    //***************ADC1��ʼ��****************//	
    ADC_InitTypeDef ADC_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	  //ʹ��ADC1ͨ��ʱ��

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; 			//����ADCģʽ
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;  			//�ر�ɨ�跽ʽ
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;			//�ر�����ת��ģʽ
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;   	//ʹ���ⲿ����ģʽ
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 			//�ɼ������Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel = 1; 			//Ҫת����ͨ����Ŀ
    ADC_Init(ADC1, &ADC_InitStructure);

    RCC_ADCCLKConfig(RCC_PCLK2_Div6);				//����ADCʱ��,ΪPCLK2��6��Ƶ,��12Hz
    ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 1, ADC_SampleTime_7Cycles5);		//����ADC1ͨ��6Ϊ7.5���������� 

    //ʹ��ADC��DMA
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);				//��λУ׼�Ĵ���
    while (ADC_GetResetCalibrationStatus(ADC1));				//�ȴ�У׼�Ĵ�����λ���

    ADC_StartCalibration(ADC1);				//ADCУ׼
    while (ADC_GetCalibrationStatus(ADC1));				//�ȴ�У׼���

    ADC_ExternalTrigConvCmd(ADC1, ENABLE);		//�����ⲿ����ģʽʹ��	
}

//�жϴ�����
void DMA1_Channel1_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA1_IT_TC1) != RESET)
    {

        DMA_accomplish = 1;	//DMA�������

        DMA_ClearITPendingBit(DMA1_IT_TC1);
    }
}


#include "StepCount.h"
#include "Delay.h"   

u8 Step_Flag = 1; //��̬����־λ

long int step_cnt = 0; //����

// filter_avg_t filter;
// axis_info_t sample;
// peak_value_t peak;
// slid_reg_t slid;
// /* USER CODE BEGIN 4 */

// //��ȡxyz���ݴ����ֵ�˲���,�������м���,�˲�����������sample,��ζ�ȡ�����Ͳ���˵��.
// void filter_calculate(filter_avg_t* filter, axis_info_t* sample)
// {
//     unsigned int i = 0;
//     unsigned char j = 0;
//     short x_sum = 0, y_sum = 0, z_sum = 0;
//     filter->count = FILTER_CNT;
//     for (i = 0; i < FILTER_CNT; i++)
//     {
//         while (MPU_Get_Accelerometer(&filter->info[i].x, &filter->info[i].y, &filter->info[i].z))
//         {
//             delay_us(100);
//             j++;
//             if (j > 20)
//                 break;
//         }
//         x_sum += filter->info[i].x;
//         y_sum += filter->info[i].y;
//         z_sum += filter->info[i].z;
//     }
//     sample->x = x_sum / FILTER_CNT;
//     sample->y = y_sum / FILTER_CNT;
//     sample->z = z_sum / FILTER_CNT;
// }

// // void peak_value_init(peak_value_t* peak)
// // {
// //     short x, y, z;

// //     x = peak->newmax.x;
// //     y = peak->newmax.y;
// //     z = peak->newmax.z;

// //     peak->newmax.x = peak->newmin.x;
// //     peak->newmax.y = peak->newmin.y;
// //     peak->newmax.z = peak->newmin.z;

// //     peak->newmin.x = x;
// //     peak->newmin.y = y;
// //     peak->newmin.z = z;
// // }

// //�ڶ�̬��ֵ�ṹ���ʼ��ʱ,һ��Ҫ��max��ֵ����ֵΪ��Сֵ,min��ֵΪ���ֵ,�����������ڶ�̬����.
// void peak_update(peak_value_t* peak, axis_info_t* cur_sample)
// {
//     static unsigned int sample_size = 0;
//     sample_size++;
//     if (sample_size > SAMPLE_SIZE)
//     {
//         /*�����ﵽ50��,����һ��*/
//         sample_size = 1;
//         peak->oldmax = peak->newmax;
//         peak->oldmin = peak->newmin;
//         //��ʼ��
//         // peak_value_init(peak);
//     }
//     peak->newmax.x = MAX(peak->newmax.x, cur_sample->x);
//     peak->newmax.y = MAX(peak->newmax.y, cur_sample->y);
//     peak->newmax.z = MAX(peak->newmax.z, cur_sample->z);

//     peak->newmin.x = MIN(peak->newmin.x, cur_sample->x);
//     peak->newmin.y = MIN(peak->newmin.y, cur_sample->y);
//     peak->newmin.z = MIN(peak->newmin.z, cur_sample->z);
// }

// char slid_update(slid_reg_t* slid, axis_info_t* cur_sample)
// {
//     char res = 0;
//     if (ABS((cur_sample->x - slid->new_sample.x)) > DYNAMIC_PRECISION)
//     {
//         slid->old_sample.x = slid->new_sample.x;
//         slid->new_sample.x = cur_sample->x;
//         res = 1;
//     }
//     else {
//         slid->old_sample.x = slid->new_sample.x;
//     }
//     if (ABS((cur_sample->y - slid->new_sample.y)) > DYNAMIC_PRECISION)
//     {
//         slid->old_sample.y = slid->new_sample.y;
//         slid->new_sample.y = cur_sample->y;
//         res = 1;
//     }
//     else {
//         slid->old_sample.y = slid->new_sample.y;
//     }

//     if (ABS((cur_sample->z - slid->new_sample.z)) > DYNAMIC_PRECISION)
//     {
//         slid->old_sample.z = slid->new_sample.z;
//         slid->new_sample.z = cur_sample->z;
//         res = 1;
//     }
//     else {
//         slid->old_sample.z = slid->new_sample.z;
//     }
//     return res;
// }

// /*�жϵ�ǰ���Ծ��*/
// char is_most_active(peak_value_t* peak)
// {
//     char res = MOST_ACTIVE_NULL;
//     short x_change = ABS((peak->newmax.x - peak->newmin.x));
//     short y_change = ABS((peak->newmax.y - peak->newmin.y));
//     short z_change = ABS((peak->newmax.z - peak->newmin.z));

//     if (x_change > y_change && x_change > z_change && x_change >= ACTIVE_PRECISION)
//     {
//         res = MOST_ACTIVE_X;
//     }
//     else if (y_change > x_change && y_change > z_change && y_change >= ACTIVE_PRECISION)
//     {
//         res = MOST_ACTIVE_Y;
//     }
//     else if (z_change > x_change && z_change > y_change && z_change >= ACTIVE_PRECISION)
//     {
//         res = MOST_ACTIVE_Z;
//     }
//     return res;
// }

// /*�ж��Ƿ��߲�*/
// void detect_step(peak_value_t* peak, slid_reg_t* slid, axis_info_t* cur_sample)
// {
//     char res = is_most_active(peak);
//     switch (res) {
//     case MOST_ACTIVE_NULL:
//     {
//         //fix
//         break;
//     }
//     case MOST_ACTIVE_X:
//     {
//         short threshold_x = (peak->oldmax.x + peak->oldmin.x) / 2;
//         if (slid->old_sample.x > threshold_x && slid->new_sample.x < threshold_x)
//         {
//             step_cnt++;
//         }
//         break;
//     }
//     case MOST_ACTIVE_Y:
//     {
//         short threshold_y = (peak->oldmax.y + peak->oldmin.y) / 2;
//         if (slid->old_sample.y > threshold_y && slid->new_sample.y < threshold_y)
//         {
//             step_cnt++;
//         }
//         break;
//     }
//     case MOST_ACTIVE_Z:
//     {
//         short threshold_z = (peak->oldmax.z + peak->oldmin.z) / 2;
//         if (slid->old_sample.z > threshold_z && slid->new_sample.z < threshold_z)
//         {
//             step_cnt++;
//         }
//         break;
//     }
//     default:
//         break;
//     }
// }



axis_value_t old_ave_GyroValue, ave_GyroValue;
peak_value_t peak_value;
u8 most_active_axis = ACTIVE_NULL;   //��¼���Ծ��

// ����
void Gyro_sample_update(void)
{
    axis_value_t GyroValue;
    axis_value_t change;
    int sum[3] = { 0 };
    u8 success_num = 0;

    //������һ�β�����ԭʼ����
    old_ave_GyroValue.X = ave_GyroValue.X;
    old_ave_GyroValue.Y = ave_GyroValue.Y;
    old_ave_GyroValue.Z = ave_GyroValue.Z;

    //��β���ȡƽ��ֵ
    for (u8 i = 0; i < SAMPLE_NUM; i++)
    {
        if (MPU_Get_Gyroscope(&GyroValue.X, &GyroValue.Y, &GyroValue.Z) == 0)
        {
            sum[0] += GyroValue.X;
            sum[1] += GyroValue.Y;
            sum[2] += GyroValue.Z;
            success_num++;
        }
    }
    ave_GyroValue.X = sum[0] / success_num;
    ave_GyroValue.Y = sum[1] / success_num;
    ave_GyroValue.Z = sum[2] / success_num;

    //ԭʼ���ݱ仯��
    change.X = ABS(ave_GyroValue.X - old_ave_GyroValue.X);
    change.Y = ABS(ave_GyroValue.Y - old_ave_GyroValue.Y);
    change.Z = ABS(ave_GyroValue.Z - old_ave_GyroValue.Z);

    //����仯�������ɽ��ܵı仯ֵ,��ԭʼ�����˻ص���һ�εĴ�С
    if (change.X < MIN_RELIABLE_VARIATION || change.X > MAX_RELIABLE_VARIATION)
    {
        ave_GyroValue.X = old_ave_GyroValue.X;
    }
    if (change.Y < MIN_RELIABLE_VARIATION || change.Y > MAX_RELIABLE_VARIATION)
    {
        ave_GyroValue.Y = old_ave_GyroValue.Y;
    }
    if (change.Z < MIN_RELIABLE_VARIATION || change.Z > MAX_RELIABLE_VARIATION)
    {
        ave_GyroValue.Z = old_ave_GyroValue.Z;
    }

    //�ֱ𱣴�������ٶ�ԭʼ���ݵ����ֵ����Сֵ
    peak_value.max.X = MAX(peak_value.max.X, ave_GyroValue.X);
    peak_value.min.X = MIN(peak_value.min.X, ave_GyroValue.X);
    peak_value.max.Y = MAX(peak_value.max.Y, ave_GyroValue.Y);
    peak_value.min.Y = MIN(peak_value.min.Y, ave_GyroValue.Y);
    peak_value.max.Z = MAX(peak_value.max.Z, ave_GyroValue.Z);
    peak_value.min.Z = MIN(peak_value.min.Z, ave_GyroValue.Z);
}

// ȷ�����Ծ��
void which_is_active(void)
{
    axis_value_t change;
    static axis_value_t active;        //������Ļ�Ծ��Ȩ��
    static u8 active_sample_num;

    Gyro_sample_update();
    active_sample_num++;

    //ÿ��һ��ʱ��,�Ƚ�һ��Ȩ�ش�С,�ж����Ծ��
    if (active_sample_num >= ACTIVE_NUM)
    {
        if (active.X > active.Y && active.X > active.Z)
        {
            most_active_axis = ACTIVE_X;
        }
        else if (active.Y > active.X && active.Y > active.Z)
        {
            most_active_axis = ACTIVE_Y;
        }
        else if (active.Z > active.X && active.Z > active.Y)
        {
            most_active_axis = ACTIVE_Z;
        }
        else
        {
            most_active_axis = ACTIVE_NULL;
        }
        active_sample_num = 0;
        active.X = 0;
        active.Y = 0;
        active.Z = 0;
    }

    //ԭʼ���ݱ仯��
    change.X = ABS(ave_GyroValue.X - old_ave_GyroValue.X);
    change.Y = ABS(ave_GyroValue.Y - old_ave_GyroValue.Y);
    change.Z = ABS(ave_GyroValue.Z - old_ave_GyroValue.Z);

    //���������Ծ��Ȩ��
    if (change.X > change.Y && change.X > change.Z)
    {
        active.X++;
    }
    else if (change.Y > change.X && change.Y > change.Z)
    {
        active.Y++;
    }
    else if (change.Z > change.X && change.Z > change.Y)
    {
        active.Z++;
    }
}

u16 step_count; //����

// �Ʋ�����
void detect_step(void)
{
    s16 mid;
    which_is_active();
    switch (most_active_axis)
    {
    case ACTIVE_NULL:
        break;
        //��׽ԭʼ�����������������
    case ACTIVE_X:
        mid = (peak_value.max.X + peak_value.min.X) / 2;
        if (old_ave_GyroValue.X < mid && ave_GyroValue.X > mid)
        {
            step_count++;
        }
        else if (old_ave_GyroValue.X > mid && ave_GyroValue.X < mid)
        {
            step_count++;
        }
        break;
    case ACTIVE_Y:
        mid = (peak_value.max.Y + peak_value.min.Y) / 2;
        if (old_ave_GyroValue.Y < mid && ave_GyroValue.Y > mid)
        {
            step_count++;
        }
        else if (old_ave_GyroValue.Y > mid && ave_GyroValue.Y < mid)
        {
            step_count++;
        }
        break;
    case ACTIVE_Z:
        mid = (peak_value.max.Z + peak_value.min.Z) / 2;
        if (old_ave_GyroValue.Z < mid && ave_GyroValue.Z > mid)
        {
            step_count++;
        }
        else if (old_ave_GyroValue.Z > mid && ave_GyroValue.Z < mid)
        {
            step_count++;
        }
        break;
    default:
        break;
    }
}

// stm32f103c8t6��ʱ��4��ʼ��
void timer4_Init(void)
{

    // ���ö�ʱ��ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    // ���ö�ʱ��4�ļ�����ģʽ��Ԥ��Ƶ�����Զ���װ�ؼĴ���
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1; // Ԥ��Ƶ������Ϊ72MHz / 1MHz - 1 = 71
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // ���ϼ���ģʽ
    TIM_TimeBaseStructure.TIM_Period = 500 - 1; // �Զ���װ�ؼĴ�������Ϊ50ms / 1us - 1 = 49999
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; // ʱ�ӷ�Ƶ
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    // ���ö�ʱ��4���ж����ȼ�
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // ʹ�ܶ�ʱ��4�ĸ����ж�
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

    // ������ʱ��4
    TIM_Cmd(TIM4, ENABLE);

}


// ��ʱ��4�жϷ�����
void TIM4_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
    {
        // ����жϱ�־λ
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

        static u8 step_time_count = 0;

        if (Step_Flag)
        {
            detect_step();
            step_time_count++;
            if (step_time_count == 6)   //300ms detect_step() ÿ50ms������һ��,��������·1���ڲ��ᳬ��3��,
                // ����ÿ300ms�鿴һ��step_count �ǲ���Ϊ0, ֻҪ����0, �����Ƕ��ֻ����1��, step�������յĲ���.
            {
                // step_time_count = 0;
                if (step_count != 0)
                {
                    step_count = 0;
                    step_cnt++;
                }

            }
        }
    }
}





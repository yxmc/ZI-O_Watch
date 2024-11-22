#include "StepCount.h"
#include "Delay.h"   

u8 Step_Flag = 1; //步态检测标志位

long int step_cnt = 0; //步数

// filter_avg_t filter;
// axis_info_t sample;
// peak_value_t peak;
// slid_reg_t slid;
// /* USER CODE BEGIN 4 */

// //读取xyz数据存入均值滤波器,存满进行计算,滤波后样本存入sample,如何读取存满就不多说了.
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

// //在动态阈值结构体初始化时,一定要将max的值都赋值为最小值,min赋值为最大值,这样才有利于动态更新.
// void peak_update(peak_value_t* peak, axis_info_t* cur_sample)
// {
//     static unsigned int sample_size = 0;
//     sample_size++;
//     if (sample_size > SAMPLE_SIZE)
//     {
//         /*采样达到50个,更新一次*/
//         sample_size = 1;
//         peak->oldmax = peak->newmax;
//         peak->oldmin = peak->newmin;
//         //初始化
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

// /*判断当前最活跃轴*/
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

// /*判断是否走步*/
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
u8 most_active_axis = ACTIVE_NULL;   //记录最活跃轴

// 采样
void Gyro_sample_update(void)
{
    axis_value_t GyroValue;
    axis_value_t change;
    int sum[3] = { 0 };
    u8 success_num = 0;

    //保存上一次测量的原始数据
    old_ave_GyroValue.X = ave_GyroValue.X;
    old_ave_GyroValue.Y = ave_GyroValue.Y;
    old_ave_GyroValue.Z = ave_GyroValue.Z;

    //多次测量取平均值
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

    //原始数据变化量
    change.X = ABS(ave_GyroValue.X - old_ave_GyroValue.X);
    change.Y = ABS(ave_GyroValue.Y - old_ave_GyroValue.Y);
    change.Z = ABS(ave_GyroValue.Z - old_ave_GyroValue.Z);

    //如果变化量超出可接受的变化值,则将原始数据退回到上一次的大小
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

    //分别保存三轴角速度原始数据的最大值和最小值
    peak_value.max.X = MAX(peak_value.max.X, ave_GyroValue.X);
    peak_value.min.X = MIN(peak_value.min.X, ave_GyroValue.X);
    peak_value.max.Y = MAX(peak_value.max.Y, ave_GyroValue.Y);
    peak_value.min.Y = MIN(peak_value.min.Y, ave_GyroValue.Y);
    peak_value.max.Z = MAX(peak_value.max.Z, ave_GyroValue.Z);
    peak_value.min.Z = MIN(peak_value.min.Z, ave_GyroValue.Z);
}

// 确定最活跃轴
void which_is_active(void)
{
    axis_value_t change;
    static axis_value_t active;        //三个轴的活跃度权重
    static u8 active_sample_num;

    Gyro_sample_update();
    active_sample_num++;

    //每隔一段时间,比较一次权重大小,判断最活跃轴
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

    //原始数据变化量
    change.X = ABS(ave_GyroValue.X - old_ave_GyroValue.X);
    change.Y = ABS(ave_GyroValue.Y - old_ave_GyroValue.Y);
    change.Z = ABS(ave_GyroValue.Z - old_ave_GyroValue.Z);

    //增加三轴活跃度权重
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

u16 step_count; //步数

// 计步程序
void detect_step(void)
{
    s16 mid;
    which_is_active();
    switch (most_active_axis)
    {
    case ACTIVE_NULL:
        break;
        //捕捉原始数据骤增和骤减现象
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

// stm32f103c8t6定时器4初始化
void timer4_Init(void)
{

    // 配置定时器时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    // 配置定时器4的计数器模式、预分频器和自动重装载寄存器
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1; // 预分频器设置为72MHz / 1MHz - 1 = 71
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseStructure.TIM_Period = 500 - 1; // 自动重装载寄存器设置为50ms / 1us - 1 = 49999
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; // 时钟分频
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    // 配置定时器4的中断优先级
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // 使能定时器4的更新中断
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

    // 启动定时器4
    TIM_Cmd(TIM4, ENABLE);

}


// 定时器4中断服务函数
void TIM4_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
    {
        // 清除中断标志位
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

        static u8 step_time_count = 0;

        if (Step_Flag)
        {
            detect_step();
            step_time_count++;
            if (step_time_count == 6)   //300ms detect_step() 每50ms被调用一次,正常人走路1秒内不会超过3步,
                // 所以每300ms查看一次step_count 是不是为0, 只要不是0, 无论是多大都只算作1步, step就是最终的步数.
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





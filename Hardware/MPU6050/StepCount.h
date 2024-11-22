#ifndef __StepCount_H__
#define __StepCount_H__

#include "mpu6050.h"
#include "sys.h"
#include "Delay.h"




// #define FILTER_CNT			4

// typedef struct
// {
//     short x;
//     short y;
//     short z;
// }axis_info_t;

// typedef struct filter_avg
// {
//     axis_info_t info[FILTER_CNT];
//     unsigned char count;
// }filter_avg_t;

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
// #define SAMPLE_SIZE   50

// typedef struct
// {
//     axis_info_t newmax;
//     axis_info_t newmin;
//     axis_info_t oldmax;
//     axis_info_t oldmin;

// }peak_value_t;

// #define ABS(a) (0 - (a)) > 0 ? (-(a)) : (a)
// #define DYNAMIC_PRECISION     			30     	 /*动态阈值精度*/


// /*一个线性移位寄存器,用于过滤高频噪声*/
// typedef struct slid_reg
// {
//     axis_info_t new_sample;
//     axis_info_t old_sample;
// }slid_reg_t;

// #define MOST_ACTIVE_NULL      			0      	 /*未找到最活跃轴*/
// #define MOST_ACTIVE_X					1		 /*最活跃轴X*/
// #define MOST_ACTIVE_Y					2        /*最活跃轴Y*/
// #define MOST_ACTIVE_Z					3        /*最活跃轴Z*/

// #define ACTIVE_PRECISION      			60      /*活跃轴最小变化值*/

// void filter_calculate(filter_avg_t* filter, axis_info_t* sample);
// void peak_update(peak_value_t* peak, axis_info_t* cur_sample);
// char slid_update(slid_reg_t* slid, axis_info_t* cur_sample);
// void detect_step(peak_value_t* peak, slid_reg_t* slid, axis_info_t* cur_sample);




extern u8 Step_Flag;
extern long int step_cnt;  //步数
// extern filter_avg_t filter;
// extern axis_info_t sample;
// extern peak_value_t peak;
// extern slid_reg_t slid;




#define ABS(a) (0 - (a)) > 0 ? (-(a)) : (a)   //取a的绝对值
#define SAMPLE_NUM                10          //采样10次取平均值
#define MIN_RELIABLE_VARIATION    500         //最小可信赖变化量
#define MAX_RELIABLE_VARIATION    5000        //最大可信赖变化量

//三轴数据
typedef struct
{
    s16 X;
    s16 Y;
    s16 Z;
}axis_value_t;

extern axis_value_t old_ave_GyroValue, ave_GyroValue;

//极值数据
typedef struct
{
    axis_value_t max;
    axis_value_t min;
}peak_value_t;

extern peak_value_t peak_value;


#define ACTIVE_NUM          30            //最活跃轴更新周期
#define ACTIVE_NULL         0             //最活跃轴未知
#define ACTIVE_X            1             //最活跃轴是X
#define ACTIVE_Y            2             //最活跃轴是Y
#define ACTIVE_Z            3             //最活跃轴是Z

extern u8 most_active_axis;   //记录最活跃轴

void timer4_Init(void);


#endif


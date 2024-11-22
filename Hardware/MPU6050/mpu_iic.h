/*
 * @Author: yxmc 2737647040@qq.com
 * @Date: 2024-10-21 15:18:57
 * @LastEditors: yxmc 2737647040@qq.com
 * @LastEditTime: 2024-11-05 21:25:36
 * @FilePath: \ZHAO-Watch\Hardware\MPU6050\mpu_iic.h
 * @Description:
 *
 * Copyright (c) 2024 by yxmc, All Rights Reserved.
 */
#ifndef __MPU_IIC_H__
#define __MPU_IIC_H__

#include "stm32f10x.h"  
#include "sys.h"
#include "Delay.h"

 //////////////////////////////////////////////////////////////////////////////////	 
 //������ֻ��ѧϰʹ��,δ���������,�������������κ���;
 //ALIENTEK STM32F407������
 //IIC ��������	   
 //����ԭ��@ALIENTEK
 //������̳:www.openedv.com
 //��������:2014/5/6
 //�汾:V1.0
 //��Ȩ����,����ؾ�. 
 //Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
 //All rights reserved									  
 ////////////////////////////////////////////////////////////////////////////////// 	

// IO�������� 
#define MPU_SDA_OUT()      do { \
                            GPIO_InitTypeDef GPIO_InitStructure; \
                            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; \
                            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; \
                            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; \
                            GPIO_Init(GPIOB, &GPIO_InitStructure); \
                        } while(0)

#define MPU_SDA_IN()       do { \
                            GPIO_InitTypeDef GPIO_InitStructure; \
                            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; \
                            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; \
                            GPIO_Init(GPIOB, &GPIO_InitStructure); \
                        } while(0)


//IO��������	 
#define  MPU_IIC_SCL    PBout(9)     //SCL
#define  MPU_IIC_SDA    PBout(8)     //SDA	 
#define  READ_SDA        PBin(8)     //����SDA 

//IIC���в�������
void  MPU_IIC_Init(void);                //��ʼ��IIC��IO��				 
void  MPU_IIC_Start(void);				//����IIC��ʼ�ź�
void  MPU_IIC_Stop(void);	  			//����IICֹͣ�ź�
void  MPU_IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8  MPU_IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8  MPU_IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void  MPU_IIC_Ack(void);					//IIC����ACK�ź�
void  MPU_IIC_NAck(void);				//IIC������ACK�ź�

// void  MPU_IIC_Write_One_Byte(u8 daddr, u8 addr, u8 data);
// u8  MPU_IIC_Read_One_Byte(u8 daddr, u8 addr);
#endif

















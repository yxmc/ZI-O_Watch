#ifndef _AHT20_H_
#define _AHT20_H_

#include "stm32f10x.h"  


void AHT20_Get_data(int *c1,int *t1);
void AHT20_Init(void);
void Delay_N10us(uint32_t t);//��ʱ����
void SensorDelay_us(uint32_t t);//��ʱ����
void Delay_4us(void);		//��ʱ����
void Delay_5us(void);		//��ʱ����
void Delay_1ms(uint32_t t);	
void AHT20_Clock_Init(void);		//��ʱ����
void SDA_Pin_Output_High(void)  ; //��PB15����Ϊ��� , ������Ϊ�ߵ�ƽ, PB15��ΪI2C��SDA
void SDA_Pin_Output_Low(void);  //��P15����Ϊ���  ������Ϊ�͵�ƽ
void SDA_Pin_IN_FLOATING(void);  //SDA����Ϊ��������
void SCL_Pin_Output_High(void); //SCL����ߵ�ƽ,P14��ΪI2C��SCL
void SCL_Pin_Output_Low(void); //SCL����͵�ƽ
void Init_I2C_Sensor_Port(void); //��ʼ��I2C�ӿ�,���Ϊ�ߵ�ƽ
void I2C_Start(void);		 //I2C��������START�ź�
void AHT20_WR_Byte(uint8_t Byte); //��AHT20дһ���ֽ�
uint8_t AHT20_RD_Byte(void);//��AHT20��ȡһ���ֽ�
uint8_t Receive_ACK(void);   //��AHT20�Ƿ��лظ�ACK
void Send_ACK(void)	;	  //�����ظ�ACK�ź�
void Send_NOT_ACK(void);	//�������ظ�ACK
void Stop_I2C(void);	  //һ��Э�����
uint8_t AHT20_Read_Status(void);//��ȡAHT20��״̬�Ĵ���
void AHT20_SendAC(void); //��AHT20����AC����
uint8_t Calc_CRC8(uint8_t *message,uint8_t Num);
void AHT20_Read_CTdata(uint32_t *ct); //û��CRCУ��,ֱ�Ӷ�ȡAHT20���¶Ⱥ�ʪ������
void AHT20_Read_CTdata_crc(uint32_t *ct); //CRCУ���,��ȡAHT20���¶Ⱥ�ʪ������
void JH_Reset_REG(uint8_t addr);///���üĴ���
void AHT20_Start_Init(void);///�ϵ��ʼ��������������״̬

#endif


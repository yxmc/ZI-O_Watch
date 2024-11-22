#ifndef __SERIAL2_H
#define __SERIAL2_H

#include <stdio.h>

extern u8 Serial2_RxPacket[10];
extern u8 Serial2_RxFlag;

void Serial2_Init(void);
void Serial2_Power_OFF(void);

void Serial2_SendByte(u8 Byte);
void Serial2_SendArray(u8* Array, uint16_t Length);
//void Serial2_SendString(char *String);



#endif

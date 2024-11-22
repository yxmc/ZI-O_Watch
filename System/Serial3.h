#ifndef __SERIAL3_H
#define __SERIAL3_H

#include <stdio.h>

extern char Serial3_RxPacket[];
extern u8 Serial3_RxFlag;
extern u8 p0, p1, we;

void Serial3_Init(void);
void Serial3_Power_OFF(void);

void Serial3_SendByte(u8 Byte);
void Serial3_SendArray(u8* Array, uint16_t Length);
//void Serial3_SendString(char *String);



#endif

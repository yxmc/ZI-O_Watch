#ifndef __SERIAL1_H
#define __SERIAL1_H

#include <stdio.h>

extern char Seria1_RxPacket[];
extern u8 Serial_RxFlag;

void Seria1_Init(void);

void Seria1_SendByte(u8 Byte);
void Seria1_SendArray(u8* Array, uint16_t Length);
void Seria1_SendString(char* String);



#endif

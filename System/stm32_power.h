#ifndef __STM32_POWER_H
#define __STM32_POWER_H

typedef enum Working_Mode
{
    Normal = 1,
    PowerSaving = 2,
    SuperSaving = 4,
} Mode;

extern u8 POWER_OFF;
extern Mode WorkingMode;
extern u8 Normal_Flag;
extern u8 PowerSaving_Flag;
extern u8 SuperSaving_Flag;



void Power_OFF(void);
void BackEnd(void);
void Load_Config(void);
void Heavy_Load(void);

#endif

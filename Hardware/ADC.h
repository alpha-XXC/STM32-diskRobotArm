#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"

// 外部变量声明
extern uint16_t Pot_Value[4];

// 函数声明
void ADC4_DMA_Init(void);
uint16_t Get_Pot_Voltage_mV(uint8_t channel);
float Get_Pot_Voltage_Float(uint8_t channel);

#endif

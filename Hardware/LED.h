#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

// 函数声明
void LED_Motion_Init(void);
void LED_Motion_Detect(void);
void LED_On(void);
void LED_Off(void);
void LED_Toggle(void);

#endif

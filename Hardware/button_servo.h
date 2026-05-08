#ifndef __BUTTON_SERVO_H
#define __BUTTON_SERVO_H

#include "stm32f10x.h"

// 按钮控制状态枚举
typedef enum {
    BUTTON_STATE_STOP_INCREASE = 0,  // 停止(增大状态后)
    BUTTON_STATE_INCREASING,         // 增大中
    BUTTON_STATE_STOP_DECREASE,      // 停止(减小状态后)
    BUTTON_STATE_DECREASING          // 减小中
} ButtonControlState;

// 函数声明
void Button_Servo_Init(void);
void Button_Servo_Control(void);
uint8_t Is_Button_Pressed(void);

#endif

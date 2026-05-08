#ifndef __SERVO_H
#define __SERVO_H

#include "stm32f10x.h"

#define SERVO5_STOP_ANGLE    100.0f   // 舵机5实际停止中心
#define SERVO5_FORWARD_ANGLE 110.0f   // 舵机5正转控制值
#define SERVO5_REVERSE_ANGLE 90.0f    // 舵机5反转控制值

// 外部变量声明
extern uint16_t Pot_Value[4];           // 4个电位器的ADC值（来自adc.h）
extern uint16_t Servo_Angle[5];         // 5个舵机的角度值

// 函数声明
void Pot_Servo_Init(void);
void Pot_Servo_Control_Loop(void);
uint16_t Map_Pot_To_Servo(uint16_t pot_value);
void Set_Servo_Angle(uint8_t servo_num, float angle);
void Delay_ms(uint32_t ms);

#endif

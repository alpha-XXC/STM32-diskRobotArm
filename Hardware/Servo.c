#include "servo.h"
#include "pca9685.h"
#include "adc.h"
#include "Delay.h"
#include <stdlib.h>

// 存储4个电位器的ADC值
extern uint16_t Pot_Value[4];  // 从adc.h获取
uint16_t Servo_Angle[5] = {90, 0, 0, 0, 0}; // 舵机初始角度

// 映射函数：将电位器ADC值(0-4095)映射到舵机角度(0-180度)
uint16_t Map_Pot_To_Servo(uint16_t pot_value)
{
    // 限制输入范围
    if(pot_value > 4095) pot_value = 4095;
    
    // 线性映射：0-4095 映射到 0-180度
    uint32_t result = ((uint32_t)pot_value * 180) / 4095;
    
    // 限制输出范围
    if(result > 180) result = 180;
    
    return (uint16_t)result;
}


// 控制舵机角度 (使用PCA9685) - 带延时版本
void Set_Servo_Angle(uint8_t servo_num, float angle)
{
    // 限制角度范围
    if(angle < 0) angle = 0;
    if(angle > 180) angle = 180;
    
    // PCA9685通道对应关系
    // 舵机1-5 对应 PCA9685通道 0-4
    if(servo_num >= 1 && servo_num <= 5)
    {
			uint8_t channel = (servo_num - 1) * 2;  //通道映射改成0，2，4，6，8
			PCA9685_SetServoAngle(channel, angle);
			Servo_Angle[servo_num - 1] = (uint16_t)angle;
			
			// 添加短暂延时让舵机有时间响应
			Delay_ms(20);  // 20ms延时
    }
}


// 初始化电位器和舵机
void Pot_Servo_Init(void)
{
    ADC4_DMA_Init();
    PCA9685_Init(0x80);
    PCA9685_SetPWMFreq(50);   // ← 添加这行，设置 50Hz
    Set_Servo_Angle(1, 90);
    Set_Servo_Angle(2, 0);
    Set_Servo_Angle(3, 160);
    Set_Servo_Angle(4, 90);
    Set_Servo_Angle(5, 100);  // 舵机5为360度连续舵机，90度中心为停止，实际值为100
    Delay_ms(20);
}

// 控制循环：将电位器值映射到舵机角度
void Pot_Servo_Control_Loop(void)
{
    // 将4个电位器的值映射到舵机角度
    float angle1 = Map_Pot_To_Servo(Pot_Value[0]);  // 电位器1 → 舵机1
    float angle2 = Map_Pot_To_Servo(Pot_Value[1]);  // 电位器2 → 舵机2
    float angle3 = Map_Pot_To_Servo(Pot_Value[2]);  // 电位器3 → 舵机3
    float angle4 = Map_Pot_To_Servo(Pot_Value[3]);  // 电位器4 → 舵机4
    
    // 舵机4反向控制：180度减去原始角度实现反向
    float reversed_angle4 = 180.0f - angle4;
    
    // 控制对应的舵机
    Set_Servo_Angle(1, angle1);          // 舵机1
    Set_Servo_Angle(2, angle2);          // 舵机2
    Set_Servo_Angle(3, angle3);          // 舵机3
    Set_Servo_Angle(4, reversed_angle4); // 舵机4（反向）
    
    // 更新角度数组
    Servo_Angle[0] = (uint16_t)angle1;
    Servo_Angle[1] = (uint16_t)angle2;
    Servo_Angle[2] = (uint16_t)angle3;
    Servo_Angle[3] = (uint16_t)reversed_angle4;
}

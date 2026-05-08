#include "led.h"
#include "servo.h"
#include "Delay.h"
#include <stdlib.h>

// 全局变量
static float Last_Servo_Angle[5] = {0};     // 上次舵机角度
static uint8_t Servo_Moving_Flag = 0;       // 舵机运动标志
static uint32_t Led_Blink_Timer = 0;        // LED闪烁计时器

// 自定义abs函数
float my_abs(float x)
{
    return (x > 0) ? x : -x;
}

// LED初始化 (使用PB12)
void LED_Motion_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // 初始化LED为高电平(熄灭状态)
    LED_Off();
    
    // 初始化上次角度值
    for(int i = 0; i < 5; i++)
    {
        Last_Servo_Angle[i] = (float)Servo_Angle[i];
    }
}

// LED控制函数
void LED_Off(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_12);  // 输出低电平，LED点亮
}

void LED_On(void)
{
    GPIO_SetBits(GPIOB, GPIO_Pin_12);    // 输出高电平，LED熄灭
}

void LED_Toggle(void)
{
    GPIOB->ODR ^= GPIO_Pin_12;           // 翻转电平
}

// 检测舵机是否在运动
uint8_t Is_Servo_Moving(void)
{
    uint8_t moving = 0;
    
    // 检查每个舵机角度是否发生变化
    for(int i = 0; i < 5; i++)
    {
        if(my_abs((float)Servo_Angle[i] - Last_Servo_Angle[i]) > 1.0)  // 角度变化超过1度认为在运动
        {
            moving = 1;
            Last_Servo_Angle[i] = (float)Servo_Angle[i];  // 更新上次角度
        }
    }
    
    return moving;
}

// LED运动检测主函数
void LED_Motion_Detect(void)
{
    static uint32_t detect_timer = 0;
    
    // 每10ms检测一次
    if(++detect_timer >= 10)
    {
        detect_timer = 0;
        
        // 检测舵机是否在运动
        if(Is_Servo_Moving())
        {
            Servo_Moving_Flag = 1;
            Led_Blink_Timer = 0;  // 重置闪烁计时器
        }
        else
        {
            if(Servo_Moving_Flag)
            {
                Led_Blink_Timer++;
                // 如果舵机停止运动超过2秒，认为完全停止
                if(Led_Blink_Timer > 200)  // 2000ms (200 * 10ms)
                {
                    Servo_Moving_Flag = 0;
                }
            }
        }
        
        // 根据运动状态控制LED
        if(Servo_Moving_Flag)
        {
            // 舵机运动中 - LED闪烁
            static uint32_t blink_timer = 0;
            if(++blink_timer >= 25)  // 250ms闪烁一次 (25 * 10ms)
            {
                blink_timer = 0;
                LED_Toggle();
            }
        }
        else
        {
            // 舵机静止 - LED常亮
            LED_On();
        }
    }
}

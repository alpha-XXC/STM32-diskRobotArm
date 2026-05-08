/*
引脚	功能	用途
PB8	I2C SCL	OLED显示
PB9	I2C SDA	OLED显示
PB10	Button	按钮输入
PB12	LED	运动指示灯
PA1-PA7	UNUSED	原舵机PWM引脚已废弃
PA4,PB0,PB1	Potentiometers	电位器输入

机械臂关节	舵机编号	PCA9685通道	控制方式	电位器/按键
底部旋转关节	舵机1	通道0	电位器控制	PA4
肩部俯仰关节	舵机2	通道1	电位器控制	PA5
肘部俯仰关节	舵机3	通道2	电位器控制	PB0
腕部旋转关节	舵机4	通道3	电位器控制	PB1
腕部俯仰/夹爪	舵机5	通道4	按键控制	PB10按钮

PCA9685模块:
OUT0 ── 舵机1信号线 (Base)
OUT1 ── 舵机2信号线 (Joint2)  
OUT2 ── 舵机3信号线 (Joint3)
OUT3 ── 舵机4信号线 (Joint4)
OUT4 ── 舵机5信号线 (Gripper)
V+   ── 舵机电源正极(5-6V)
GND  ── 舵机电源负极
SCL  ── STM32 PB8
SDA  ── STM32 PB9

               */

#include "stm32f10x.h"
#include "Delay.h"
#include "led.h"
#include "servo.h"
#include "button_servo.h"
#include "pca9685.h"  // 添加这个
#include "I2C.h"
void Test_Servo_Full_Range(void);

int main(void)
{
    Delay_Init();
    LED_Motion_Init();
    I2C_Master_Init();
    // 添加舵机与电位器初始化
    Pot_Servo_Init();
    Button_Servo_Init();
    
    // 初始化闪烁提示（可选）
    for(int i = 0; i < 10; i++) {
        LED_On();
        Delay_ms(50);
        LED_Off();
        Delay_ms(50);

    }
    /*
    // ==================== PCA9685 I2C通信测试 ====================
    // 读取MODE1寄存器验证I2C通信是否正常
    uint8_t mode1_reg = PCA9685_ReadRegister(PCA9685_MODE1);
    if(mode1_reg != 0xFF) {
        // 通信成功（返回的不是全1，通常复位值是0x11或0x31）
        LED_On();  // 通信成功指示灯常亮
    }
    
    // ==================== 舵机通道测试 ====================
    // 测试通道0（底部旋转关节），你可以修改这里的通道号
    // 在 Test_Servo_Full_Range() 前执行单通道测试
    #define TEST_SERVO_CHANNEL  1  // 修改这个值来测试不同通道
    
    // 先复位到0度位置
    PCA9685_SetServoAngle(TEST_SERVO_CHANNEL, 0);
    Delay_ms(1000);
    PCA9685_SetServoAngle(TEST_SERVO_CHANNEL, 180);
    
    // 测试0-90-180-90-0摆动
    for(int cycle = 0; cycle < 3; cycle++) {
        PCA9685_SetServoAngle(TEST_SERVO_CHANNEL, 90);
        Delay_ms(500);
        PCA9685_SetServoAngle(TEST_SERVO_CHANNEL, 180);
        Delay_ms(500);
        PCA9685_SetServoAngle(TEST_SERVO_CHANNEL, 90);
        Delay_ms(500);
        PCA9685_SetServoAngle(TEST_SERVO_CHANNEL, 0);
        Delay_ms(500);
    }
    */
    // ============================================================

		Test_Servo_Full_Range();
		
    while(1)
    {
        // 读取电位器并控制舵机 1-4
        Pot_Servo_Control_Loop();
        
        // 按键控制舵机 5
        Button_Servo_Control();
        
        // LED 运动指示
        LED_Motion_Detect();
        
        // 适当延时，避免控制频率过高
        Delay_ms(10);
        
    }
		
}

void Test_Servo_Full_Range(void)
{
    for(int servo = 1; servo <= 5; servo++) {
        if(servo == 1) {
            Set_Servo_Angle(servo, 80);
            Delay_ms(500);
            Set_Servo_Angle(servo, 90);
            Delay_ms(500);
            Set_Servo_Angle(servo, 85);
            Delay_ms(1500);
        }

        else if(servo == 2) {
            Set_Servo_Angle(servo, 0);
            Delay_ms(500);
            Set_Servo_Angle(servo, 20);
            Delay_ms(500);
            Set_Servo_Angle(servo, 10);
            Delay_ms(1500);
        }

        else if(servo == 3) {
            Set_Servo_Angle(servo, 180);
            Delay_ms(500);
            Set_Servo_Angle(servo, 160);
            Delay_ms(500);
            Set_Servo_Angle(servo, 170);
            Delay_ms(1500);
        }

        else if(servo == 4) {
            Set_Servo_Angle(servo, 80);
            Delay_ms(500);
            Set_Servo_Angle(servo, 100);
            Delay_ms(500);
            Set_Servo_Angle(servo, 90);
            Delay_ms(1500);
        }

        else {
            Set_Servo_Angle(servo, 90);
            Delay_ms(20);
            Set_Servo_Angle(servo, 110);
            Delay_ms(20);
        }
    }
}

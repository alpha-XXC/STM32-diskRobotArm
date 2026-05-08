#include "button_servo.h"
#include "servo.h"
#include "Delay.h"

// 舵机5相关变量
// 舵机5为SG90 360度连续旋转舵机，90度为停止，0/180度为最大反转/正转
static float Servo5_Angle = 90.0;         // 舵机5当前控制值(初始停止中心)
static ButtonControlState button_state = BUTTON_STATE_STOP_INCREASE;  // 按钮控制状态
static uint8_t last_button_state = 0;    // 上次按键状态

// 按键初始化 (使用PB10)
void Button_Servo_Init(void)
{
    // 使能GPIOB时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    // PB10配置为输入模式(按键输入)
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   // 上拉输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // 初始化舵机5为停止中心位置(连续舵机停止)
    Servo5_Angle = 90.0;
    Set_Servo_Angle(5, Servo5_Angle);
}

// 检测按键是否按下 (使用PB10)
uint8_t Is_Button_Pressed(void)
{
    // 按键按下时PB10为低电平
    // 使用软件消抖
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 0)
    {
        Delay_ms(20);  // 简单消抖延时
        if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 0)
        {
            return 1;  // 按键确实按下
        }
    }
    return 0;  // 按键未按下
}

// 按键控制舵机5主函数
void Button_Servo_Control(void)
{
    uint8_t current_button_state = Is_Button_Pressed();
    
    // 检测按键边缘触发(按下瞬间)
    if(current_button_state == 1 && last_button_state == 0)
    {
        // 按键按下，切换状态
        switch(button_state)
        {
            case BUTTON_STATE_STOP_INCREASE:
                button_state = BUTTON_STATE_INCREASING;      // 切换到增大状态
                break;
                
            case BUTTON_STATE_INCREASING:
                button_state = BUTTON_STATE_STOP_DECREASE;  // 切换到停止状态
                break;
                
            case BUTTON_STATE_STOP_DECREASE:
                button_state = BUTTON_STATE_DECREASING;     // 切换到减小状态
                break;
                
            case BUTTON_STATE_DECREASING:
                button_state = BUTTON_STATE_STOP_INCREASE;  // 切换到停止状态
                break;
        }
    }
    
    last_button_state = current_button_state;
    
    // 根据当前状态控制舵机
    switch(button_state)
    {
        case BUTTON_STATE_INCREASING:
            // 进入正转状态，逐步增加前进速度
            Servo5_Angle = 110.0;
            Set_Servo_Angle(5, Servo5_Angle);
            break;
            
        case BUTTON_STATE_DECREASING:
             Servo5_Angle = 90.0;
            Set_Servo_Angle(5, Servo5_Angle);
            break;
            
        case BUTTON_STATE_STOP_INCREASE:
                    // 停止状态，保持100度停止位置
            if(Servo5_Angle != 100.0)
            {
                Servo5_Angle = 100.0;
                Set_Servo_Angle(5, Servo5_Angle);
            }
            break;
        case BUTTON_STATE_STOP_DECREASE:
            // 停止状态，保持100度停止位置
            if(Servo5_Angle != 100.0)
            {
                Servo5_Angle = 100.0;
                Set_Servo_Angle(5, Servo5_Angle);
            }
            break;
    }
}

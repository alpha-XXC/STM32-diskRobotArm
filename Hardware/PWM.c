
#include "stm32f10x.h"                  // Device header

/*
舵机编号	定时器	通道	GPIO引脚	复用功能
舵机1	TIM2	CH2	PA1	TIM2_CH2
舵机2	TIM2	CH3	PA2	TIM2_CH3
舵机3	TIM2	CH4	PA3	TIM2_CH4
舵机4	TIM3	CH1	PA6	TIM3_CH1
舵机5	TIM3	CH2	PA7	TIM3_CH2
*/
void PWM_Multi_Init(void)
{
    // 使能时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    
    // GPIO配置
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    // 配置PA1, PA2, PA3, PA6, PA7为复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // TIM2配置 (控制舵机1,2,3)
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 20000 - 1;      // 20ms周期 (50Hz)
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;      // 72MHz/72 = 1MHz
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
    
    // TIM3配置 (控制舵机4,5)
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
    
    // TIM2输出比较配置
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 1500;  // 初始1.5ms
    
    // 舵机1 - TIM2_CH2 (PA1)
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    
    // 舵机2 - TIM2_CH3 (PA2)
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
    
    // 舵机3 - TIM2_CH4 (PA3)
    TIM_OC4Init(TIM2, &TIM_OCInitStructure);
    TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
    
    // TIM3输出比较配置
    TIM_OCInitStructure.TIM_Pulse = 1500;  // 初始1.5ms
    
    // 舵机4 - TIM3_CH1 (PA6)
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
    
    // 舵机5 - TIM3_CH2 (PA7)
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
    
    // 启动定时器
    TIM_Cmd(TIM2, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
}


/*使用示例
int main(void)
{
    // 初始化系统时钟等...
    
    PWM_Multi_Init();  // 初始化5路PWM
    
    while(1)
    {
        // 测试：所有舵机转到0度位置
        Set_All_Servos(500, 500, 500, 500, 500);
        Delay_ms(1000);
        
        // 测试：所有舵机转到中间位置
        Set_All_Servos(1500, 1500, 1500, 1500, 1500);
        Delay_ms(1000);
        
        // 测试：所有舵机转到180度位置
        Set_All_Servos(2500, 2500, 2500, 2500, 2500);
        Delay_ms(1000);
        
        // 单独控制某个舵机
        Set_Servo_Angle(1, 1000);  // 舵机1转到45度
        Delay_ms(500);
    }
}

*/
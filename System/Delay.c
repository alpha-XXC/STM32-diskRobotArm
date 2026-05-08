#include "stm32f10x.h"

#include "Delay.h"

// 系统时钟初始化
void Delay_Init(void)
{
    // 如果使用SysTick延时
    // SysTick_Config(SystemCoreClock / 1000);  // 1ms中断
    // 或者为空函数，如果延时函数不需要初始化
    
    // 如果使用软件延时，这个函数可以为空
}

// 毫秒延时函数
void Delay_ms(uint32_t ms)
{
    // 你的毫秒延时实现
    for(uint32_t i = 0; i < ms; i++)
    {
        Delay_us(1000);
    }
}

// 微秒延时函数
void Delay_us(uint32_t us)
{
    // 你的微秒延时实现
    // 根据系统时钟调整循环次数
    for(uint32_t i = 0; i < us * 8; i++)  // 假设72MHz系统时钟
    {
        __NOP();
    }
}


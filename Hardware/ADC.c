#include "stm32f10x.h"

// 存储4个电位器的ADC值
uint16_t Pot_Value[4] = {0};  // [0]-电位器1, [1]-电位器2, [2]-电位器3, [3]-电位器4

// 4路电位器DMA初始化 (使用PA4, PA5, PB0, PB1)
void ADC4_DMA_Init(void)
{
    // 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    
    // GPIO配置
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;  // 模拟输入模式
    
    // PA4, PA5配置为模拟输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // PB0, PB1配置为模拟输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // DMA配置
    DMA_InitTypeDef DMA_InitStructure;
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;     // ADC数据寄存器
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Pot_Value;         // 存储数组地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                  // 外设到内存
    DMA_InitStructure.DMA_BufferSize = 4;                               // 4个数据
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;    // 外设地址不递增
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;             // 内存地址递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // 16位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; // 16位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                     // 循环模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                 // 高优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                        // 禁止内存到内存
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    
    // 使能DMA通道
    DMA_Cmd(DMA1_Channel1, ENABLE);
    
    // ADC配置
    ADC_InitTypeDef ADC_InitStructure;
    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;              // 独立模式
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;                    // 扫描模式(多通道)
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;              // 连续转换
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 软件触发
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;          // 右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 4;                         // 4个通道
    ADC_Init(ADC1, &ADC_InitStructure);
    
    // 配置ADC时钟
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);  // 12MHz
    
    // 配置4个通道的转换顺序和采样时间
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_55Cycles5);  // PA4
    ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 2, ADC_SampleTime_55Cycles5);  // PA5
    ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 3, ADC_SampleTime_55Cycles5);  // PB0
    ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 4, ADC_SampleTime_55Cycles5);  // PB1
    
    // 使能ADC1的DMA
    ADC_DMACmd(ADC1, ENABLE);
    
    // 使能ADC1
    ADC_Cmd(ADC1, ENABLE);
    
    // ADC校准
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
    
    // 启动ADC转换
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

// 获取实际电压值(单位: mV)
uint16_t Get_Pot_Voltage_mV(uint8_t channel)
{
    if(channel < 4)
    {
        return (Pot_Value[channel] * 3300) / 4095;
    }
    return 0;
}

// 获取实际电压值(浮点数)
float Get_Pot_Voltage_Float(uint8_t channel)
{
    if(channel < 4)
    {
        return (Pot_Value[channel] * 3.3f) / 4095.0f;
    }
    return 0.0f;
}

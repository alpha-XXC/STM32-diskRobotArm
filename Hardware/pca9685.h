#ifndef __PCA9685_H
#define __PCA9685_H

#include "stm32f10x.h"

// PCA9685寄存器地址
#define PCA9685_MODE1           0x00    // 模式寄存器1
#define PCA9685_MODE2           0x01    // 模式寄存器2
#define PCA9685_SUBADR1         0x02    // 子地址1
#define PCA9685_SUBADR2         0x03    // 子地址2
#define PCA9685_SUBADR3         0x04    // 子地址3
#define PCA9685_ALLCALLADR      0x05    // 全呼地址
#define LED0_ON_L               0x06    // LED0输出和寄存器低位
#define LED0_ON_H               0x07    // LED0输出和寄存器高位
#define LED0_OFF_L              0x08    // LED0输出关寄存器低位
#define LED0_OFF_H              0x09    // LED0输出关寄存器高位
// LED1-LED15寄存器地址可以通过LED0地址+4*N计算得到

#define PCA9685_ALLLED_ON_L     0xFA    // 所有LED输出和寄存器低位
#define PCA9685_ALLLED_ON_H     0xFB    // 所有LED输出和寄存器高位
#define PCA9685_ALLLED_OFF_L    0xFC    // 所有LED输出关寄存器低位
#define PCA9685_ALLLED_OFF_H    0xFD    // 所有LED输出关寄存器高位
#define PCA9685_PRESCALE        0xFE    // 预分频寄存器
#define PCA9685_TESTMODE        0xFF    // 测试模式寄存器

// PCA9685命令
#define PCA9685_RESTART         0x80
#define PCA9685_SLEEP           0x10
#define PCA9685_ALLCALL         0x01
#define PCA9685_INVRT           0x10
#define PCA9685_OUTDRV          0x04

// PCA9685默认地址(写地址)
#define PCA9685_DEFAULT_ADDR    0x80   // 10000000 (写地址)
#define PCA9685_READ_ADDR       0x81   // 10000001 (读地址)

// 函数声明
void PCA9685_Init(uint8_t write_addr);  // 传入写地址(如0x80)
void PCA9685_SetPWMFreq(float freq);
void PCA9685_SetPWM(uint8_t num, uint16_t on, uint16_t off);
void PCA9685_SetServoAngle(uint8_t channel, float angle);
uint8_t PCA9685_ReadRegister(uint8_t reg_addr);  // 新增读寄存器函数
void PCA9685_WriteRegister(uint8_t reg_addr, uint8_t value);
void PCA9685_Wake(void);

#endif

#include "pca9685.h"
#include "i2c.h"
#include "Delay.h"

// 添加缺失的寄存器定义
#define LED0_ON_L       0x06    // LED0输出和寄存器低位
#define LED0_ON_H       0x07    // LED0输出和寄存器高位
#define LED0_OFF_L      0x08    // LED0输出关寄存器低位
#define LED0_OFF_H      0x09    // LED0输出关寄存器高位

static uint8_t _i2caddr;

// PCA9685初始化
void PCA9685_Init(uint8_t addr_write)  // 直接传入写地址(如0x80)
{
    _i2caddr = addr_write;  // 直接使用写地址
    
    I2C_Master_Init();
    
    Delay_ms(10);
    
    // 复位PCA9685
    I2C_Start();
    I2C_SendByte(_i2caddr);  // 直接使用写地址0x80
    if(!I2C_WaitAck()) {
        I2C_Stop();
        return;  // I2C通信失败
    }
    I2C_SendByte(PCA9685_MODE1);
    I2C_WaitAck();
    I2C_SendByte(0x00);
    I2C_WaitAck();
    I2C_Stop();

    // 初始化后明确设置 MODE1，保证不进入睡眠并启用 OUTDRV
    PCA9685_WriteRegister(PCA9685_MODE1, PCA9685_ALLCALL | PCA9685_OUTDRV);
    Delay_ms(5);
}

// 读取寄存器时使用读地址
uint8_t PCA9685_ReadRegister(uint8_t reg_addr)
{
    uint8_t result;
    
    // 写操作：发送寄存器地址
    I2C_Start();
    I2C_SendByte(_i2caddr);        // 写地址(如0x80)
    I2C_WaitAck();
    I2C_SendByte(reg_addr);
    I2C_WaitAck();
    I2C_Stop();
    
    // 读操作：重新开始并发送读地址
    I2C_Start();
    I2C_SendByte(_i2caddr | 0x01); // 读地址(如0x81)
    I2C_WaitAck();
    result = I2C_ReadByte(0);      // 读取数据，发送NACK
    I2C_Stop();
    
    return result;
}

// 写寄存器函数，方便直接设置 MODE1 等寄存器
void PCA9685_WriteRegister(uint8_t reg_addr, uint8_t value)
{
    I2C_Start();
    I2C_SendByte(_i2caddr);
    if(!I2C_WaitAck()) {
        I2C_Stop();
        return;
    }
    I2C_SendByte(reg_addr);
    I2C_WaitAck();
    I2C_SendByte(value);
    I2C_WaitAck();
    I2C_Stop();
}

// 唤醒 PCA9685：清除 SLEEP 位，确保芯片保持工作
void PCA9685_Wake(void)
{
    uint8_t mode1 = PCA9685_ReadRegister(PCA9685_MODE1);
    if(mode1 & PCA9685_SLEEP) {
        mode1 &= ~PCA9685_SLEEP;
        PCA9685_WriteRegister(PCA9685_MODE1, mode1);
        Delay_ms(1);
    }
}

// 设置PWM频率
void PCA9685_SetPWMFreq(float freq)
{
    // freq *= 0.9;  // 振荡器误差修正，如实测频率偏高可逐步调小此系数
    float prescaleval = 25000000;
    prescaleval /= 4096;
    prescaleval /= freq;
    prescaleval -= 1;
    
    uint8_t prescale = (uint8_t)(prescaleval + 0.5);
    
    // 读取旧的模式寄存器
    I2C_Start();
    I2C_SendByte(_i2caddr);
    I2C_WaitAck();
    I2C_SendByte(PCA9685_MODE1);
    I2C_WaitAck();
    I2C_Start();  // 重新开始
    I2C_SendByte(_i2caddr | 0x01);  // 读模式
    I2C_WaitAck();
    uint8_t oldmode = I2C_ReadByte(0);  // 读取并发送NACK（只读一个字节）
    I2C_Stop();
    
    uint8_t newmode = (oldmode & 0x7F) | 0x10; // 进入睡眠模式（SLEEP位置1）
    
    // 第1步：写入睡眠模式，使能 PRESCALE 修改
    I2C_Start();
    I2C_SendByte(_i2caddr);
    if(!I2C_WaitAck()) { I2C_Stop(); return; }
    I2C_SendByte(PCA9685_MODE1);
    I2C_WaitAck();
    I2C_SendByte(newmode);
    I2C_WaitAck();
    I2C_Stop();
    
    // 第2步：写入预分频值
    I2C_Start();
    I2C_SendByte(_i2caddr);
    if(!I2C_WaitAck()) { I2C_Stop(); return; }
    I2C_SendByte(PCA9685_PRESCALE);
    I2C_WaitAck();
    I2C_SendByte(prescale);
    I2C_WaitAck();
    I2C_Stop();
    
    // 第3步：恢复旧模式（退出睡眠）
    I2C_Start();
    I2C_SendByte(_i2caddr);
    if(!I2C_WaitAck()) { I2C_Stop(); return; }
    I2C_SendByte(PCA9685_MODE1);
    I2C_WaitAck();
    I2C_SendByte(oldmode);
    I2C_WaitAck();
    I2C_Stop();
    
    Delay_ms(5);  // 等待振荡器稳定（datasheet要求至少500us）
    
    // 第4步：使能 AUTO_INCREMENT + ALLCALL，正式启动输出
    I2C_Start();
    I2C_SendByte(_i2caddr);
    if(!I2C_WaitAck()) { I2C_Stop(); return; }
    I2C_SendByte(PCA9685_MODE1);
    I2C_WaitAck();
    I2C_SendByte(oldmode | 0xa1);  // RESTART | AUTO_INCREMENT | ALLCALL
    I2C_WaitAck();
    I2C_Stop();
}

//PCA9685 PWM设置
void PCA9685_SetPWM(uint8_t num, uint16_t on, uint16_t off)
{
    if(num > 15) return;
    
    on &= 0x0FFF;
    off &= 0x0FFF;
    
    I2C_Start();
    
    I2C_SendByte(_i2caddr);
    if(!I2C_WaitAck()) {
        I2C_Stop();
        return;
    }
    
    I2C_SendByte(LED0_ON_L + 4 * num);
    if(!I2C_WaitAck()) {
        I2C_Stop();
        return;
    }
    
    I2C_SendByte(on & 0xFF);
    if(!I2C_WaitAck()) {
        I2C_Stop();
        return;
    }
    
    I2C_SendByte(on >> 8);
    if(!I2C_WaitAck()) {
        I2C_Stop();
        return;
    }
    
    I2C_SendByte(off & 0xFF);
    if(!I2C_WaitAck()) {
        I2C_Stop();
        return;
    }
    
    I2C_SendByte(off >> 8);
    if(!I2C_WaitAck()) {
        I2C_Stop();
        return;
    }
    
    I2C_Stop();
}

// 设置舵机角度 (0-180度)
void PCA9685_SetServoAngle(uint8_t channel, float angle)
{
    PCA9685_Wake();
    if(angle < 0) angle = 0;
    if(angle > 180) angle = 180;
    
    // 舵机脉冲宽度：0.5ms-2.5ms 对应 0°-180°
    // PCA9685在50Hz时，4096个计数周期对应20ms
    // 0.5ms = 4096 * 0.5/20 = 102.4
    // 2.5ms = 4096 * 2.5/20 = 512
    
    float pulse = (angle * (512 - 102.4) / 180) + 102.4;
    uint16_t off = (uint16_t)pulse;
    
    PCA9685_SetPWM(channel, 0, off);
}

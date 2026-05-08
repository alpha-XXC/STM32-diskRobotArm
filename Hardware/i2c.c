#include "i2c.h"
#include "Delay.h"

/*引脚配置*/
#define I2C_SCL_PIN    GPIO_Pin_8
#define I2C_SDA_PIN    GPIO_Pin_9
#define I2C_GPIO_PORT  GPIOB

/*引脚操作函数*/
void I2C_W_SCL(uint8_t x)
{
    GPIO_WriteBit(I2C_GPIO_PORT, I2C_SCL_PIN, (BitAction)x);
}

void I2C_W_SDA(uint8_t x)
{
    GPIO_WriteBit(I2C_GPIO_PORT, I2C_SDA_PIN, (BitAction)x);
}

uint8_t I2C_R_SDA(void)
{
    return GPIO_ReadInputDataBit(I2C_GPIO_PORT, I2C_SDA_PIN);
}

/*引脚初始化*/
void I2C_Master_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;
 	GPIO_Init(I2C_GPIO_PORT, &GPIO_InitStructure);
	
	I2C_W_SCL(1);
	I2C_W_SDA(1);
}

/**
  * @brief  I2C开始
  * @param  无
  * @retval 无
  */
void I2C_Start(void)
{
	I2C_W_SDA(1);
	I2C_W_SCL(1);
	Delay_us(1);
	I2C_W_SDA(0);
	Delay_us(1);
	I2C_W_SCL(0);
	Delay_us(1);
}

/**
  * @brief  I2C停止
  * @param  无
  * @retval 无
  */
void I2C_Stop(void)
{
	I2C_W_SDA(0);
	Delay_us(1);
	I2C_W_SCL(1);
	Delay_us(1);
	I2C_W_SDA(1);
	Delay_us(1);
}

/**
  * @brief  I2C发送一个字节
  * @param  Byte 要发送的一个字节
  * @retval 无
  */
void I2C_SendByte(uint8_t Byte)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        I2C_W_SDA(Byte & 0x80);
        Delay_us(1);
        I2C_W_SCL(1);
        Delay_us(2);
        I2C_W_SCL(0);
        Delay_us(1);
        Byte <<= 1;
    }
    // 注意：这里不再产生第9个时钟
}

/**
  * @brief  I2C读取一个字节
  * @param  ack 1-发送ACK 0-发送NACK
  * @retval 读取的字节
  */
uint8_t I2C_ReadByte(uint8_t ack)
{
	uint8_t i, byte = 0;
	
	// 释放SDA线，准备读取
	I2C_W_SDA(1);
	Delay_us(1);
	
	for (i = 0; i < 8; i++)
	{
		I2C_W_SCL(0);
		Delay_us(2);
		I2C_W_SCL(1);
		Delay_us(1);
		byte <<= 1;
		if (I2C_R_SDA()) byte |= 0x01;
		Delay_us(1);
	}
	
	I2C_W_SCL(0);
	// 发送ACK/NACK
	I2C_W_SDA(ack ? 0 : 1);
	Delay_us(1);
	I2C_W_SCL(1);
	Delay_us(2);
	I2C_W_SCL(0);
	Delay_us(1);
	I2C_W_SDA(1);  // 释放SDA
	
	return byte;
}

/**
  * @brief  I2C等待应答
  * @param  无
  * @retval 1-有应答 0-无应答
  */
uint8_t I2C_WaitAck(void)
{
    uint8_t ack;
    I2C_W_SDA(1);
    Delay_us(1);
    I2C_W_SCL(1);
    Delay_us(1);
    ack = !I2C_R_SDA();
    I2C_W_SCL(0);
    Delay_us(1);
    return ack;
}

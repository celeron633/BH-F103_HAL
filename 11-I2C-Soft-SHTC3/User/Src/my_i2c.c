#include "my_i2c.h"
#include "delay.h"

#include <stdio.h>

// 写SDA
static inline void WriteSDA(uint8_t state)
{
    HAL_GPIO_WritePin(I2C_GPIO, I2C_SDA_PIN , state);
    delay_us(2);
}

// 写CLK
static inline void WriteCLK(uint8_t state)
{
    HAL_GPIO_WritePin(I2C_GPIO, I2C_CLK_PIN , state);
    delay_us(2);
}

// 读SDA
static inline uint8_t ReadSDA()
{
    return HAL_GPIO_ReadPin(I2C_GPIO, I2C_SDA_PIN);
}

#define SDA_H() WriteSDA(1)
#define SDA_L() WriteSDA(0)

#define CLK_H() WriteCLK(1)
#define CLK_L() WriteCLK(0)

void I2C_Init(void)
{
    __HAL_RCC_GPIOF_CLK_ENABLE();

    GPIO_InitTypeDef i2cGpio = {0};
    // 使用开漏模式
    i2cGpio.Mode    = GPIO_MODE_OUTPUT_OD;
    i2cGpio.Pin     = I2C_CLK_PIN | I2C_SDA_PIN;
    i2cGpio.Pull    = GPIO_NOPULL;
    i2cGpio.Speed   = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(I2C_GPIO, &i2cGpio);

    // 先全部释放掉
    HAL_GPIO_WritePin(I2C_GPIO, I2C_SDA_PIN | I2C_CLK_PIN, GPIO_PIN_SET);
}

void I2C_Start(void)
{
    CLK_H();
    SDA_H();

    // CLK空闲时 SDA由1到0表示开始
    // 拉低SDA
    SDA_L();
    // 拉低CLK
    CLK_L();
}

void I2C_Stop(void)
{
    CLK_H();
    // CLK空闲时 SDA从0变成1表示结束
    SDA_L();
    SDA_H();
}

void I2C_WriteByte(uint8_t data)
{
    for (uint8_t i = 0; i < 8; i++) {
        // 依次发送最高位
        if (((data << i) & (uint8_t)0x80)) {
            SDA_H();
        } else {
            SDA_L();
        }
        // 拉高CLK时, 从机会从SDA读取数据
        CLK_H();
        CLK_L();

        // 最后一位后, 释放掉SDA 
        if (i == 7) {
            SDA_H();
        }
    }
}

uint8_t I2C_ReadAck(void)
{
    uint8_t readBit;

    // Write最后一个Bit后已经释放了SDA了, 这里不释放也没有关系
    SDA_H();

    CLK_H();
    readBit = ReadSDA();
    CLK_L();

    return readBit;
}

uint8_t I2C_ReadByte(void)
{
    uint8_t data = 0x0;
    uint8_t readBit;
    
    for (uint8_t i = 0; i < 8; i++) {
        // 释放CLK, 从机会在SDA线上输出顺序bit
        CLK_H();
        readBit = ReadSDA();
        data = data | (readBit << (7-i));
        CLK_L();
    }

    return data;
}

void I2C_WriteNAck()
{
    SDA_H();
    CLK_H();
    CLK_L();

    // release SDA line
    // 这步一定要做, 不然从机拒绝传下个字节
    SDA_H();
}

void I2C_WriteAck()
{
    SDA_L();
    CLK_H();
    CLK_L();

    // release SDA line
    // 这步一定要做, 不然从机拒绝传下个字节
    SDA_H();
}

void I2C_WriteData(uint8_t addr, uint8_t *data, size_t len)
{
    I2C_Start();
    I2C_WriteByte(addr);
    // 必须ACK, 不然从机不认下个字节
    I2C_ReadAck();

    for (size_t i = 0; i < len; i++) {
        I2C_WriteByte(*(data + i));
        // 必须ACK, 不然从机不认下个字节
        I2C_ReadAck();
    }

    I2C_Stop();
}

void I2C_Test(void)
{
    uint32_t i2cAddr = 0x78;

    I2C_Init();
    I2C_Start();
    I2C_WriteByte(i2cAddr);
    uint8_t ret = I2C_ReadAck();
    printf("i2c ack: %d\r\n", ret);
}
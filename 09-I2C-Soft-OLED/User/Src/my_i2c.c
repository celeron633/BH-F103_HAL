#include "my_i2c.h"
#include "delay.h"

int g_i2cInitFlag = 0;

// 写SDA
static inline void WriteSDA(uint8_t state)
{
    HAL_GPIO_WritePin(GPIOB, I2C_SDA_PIN , state);
    delay_us(10);
}

// 写CLK
static inline void WriteCLK(uint8_t state)
{
    HAL_GPIO_WritePin(GPIOB, I2C_CLK_PIN , state);
    delay_us(10);
}

void I2C_Init(void)
{
    if (g_i2cInitFlag) {
        return;
    }

    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef i2cGpio = {0};
    // 使用开漏模式
    i2cGpio.Mode    = GPIO_MODE_OUTPUT_OD;
    i2cGpio.Pin     = I2C_CLK_PIN | I2C_SDA_PIN;
    i2cGpio.Pull    = GPIO_NOPULL;
    i2cGpio.Speed   = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOB, &i2cGpio);

    // 先全部释放掉
    HAL_GPIO_WritePin(GPIOB, I2C_SDA_PIN | I2C_CLK_PIN, GPIO_PIN_SET);
    g_i2cInitFlag = 1;
}

void I2C_Start(void)
{
    WriteCLK(1);
    WriteSDA(1);

    // CLK空闲时 SDA由1到0表示开始
    // 拉低SDA
    WriteSDA(0);
    // 拉低CLK
    WriteCLK(0);
}

void I2C_Stop(void)
{
    WriteCLK(1);
    // CLK空闲时 SDA从0变成1表示结束
    // 拉低SDA
    WriteSDA(0);
    // 释放SDA
    WriteSDA(1);
}

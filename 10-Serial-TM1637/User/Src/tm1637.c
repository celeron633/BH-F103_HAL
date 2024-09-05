#include "tm1637.h"
#include "delay.h"

#include <string.h>
#include <stdlib.h>

static uint8_t digitMap[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};

static inline void CLK_H()
{
    HAL_GPIO_WritePin(TM1637_GPIO, TM1637_CLK, GPIO_PIN_SET);
    delay_us(10);
}

static inline void CLK_L()
{
    HAL_GPIO_WritePin(TM1637_GPIO, TM1637_CLK, GPIO_PIN_RESET);
    delay_us(10);
}

static inline void DIO_H()
{
    HAL_GPIO_WritePin(TM1637_GPIO, TM1637_DIO, GPIO_PIN_SET);
    delay_us(10);
}

static inline void DIO_L()
{
    HAL_GPIO_WritePin(TM1637_GPIO, TM1637_DIO, GPIO_PIN_RESET);
    delay_us(10);
}

void tm1637Init(void)
{
    HAL_Delay(20);
    __HAL_RCC_GPIOF_CLK_ENABLE();

    GPIO_InitTypeDef tm1637Gpio;
    // 使用开漏模式输出, 设置寄存器为1时, 输出高阻态
    tm1637Gpio.Mode = GPIO_MODE_OUTPUT_OD;
    tm1637Gpio.Pin = TM1637_DIO | TM1637_CLK;
    tm1637Gpio.Speed = GPIO_SPEED_HIGH;
    tm1637Gpio.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(TM1637_GPIO, &tm1637Gpio);
}

void tm1637Start(void)
{
    // 开始：CLK有效的情况下，DIO从高到低
    CLK_H();
    DIO_H();
    DIO_L();
}

void tm1637Stop(void)
{
    // 结束：时钟有效的情况下，DIO从低到高
    CLK_L();
    DIO_L();
    CLK_H();
    DIO_H();
}

void tm1637Write(uint8_t dat)
{
    for (int i = 0; i < 8; i++) {
        CLK_L();
        if ((dat & 0x01) == 1) {
            DIO_H();
        } else {
            DIO_L();
        }
        dat = (dat >> 1);
        CLK_H();
    }
}

void tm1637WaitACK(void)
{
    CLK_L();
    // delay_us(5);
    // 等待TM1637将DIO拉低
    // while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == 1) {

    // }
    CLK_H();
    CLK_L();
}

void tm1637Display(const char *dat, size_t len)
{
    if (len > 4) {
        len = 4;
    }

    uint8_t num2Display[4] = {0};
    for (size_t i = 0; i < 4; i++) {
        uint8_t idx = 0;
        if (len >= i + 1) {
            idx = (dat[i] - '0');
        } else {
            idx = 0;
        }
        if (idx > sizeof(digitMap)/sizeof(digitMap[0])) {
            idx = 0;
        }
        num2Display[i] = digitMap[idx];
    }

    // 设置模式
    // 40 地址自加模式，44 固定地址模式
    tm1637Start();
    tm1637Write(0x40);
    tm1637WaitACK();
    tm1637Stop();

    // 设置首地址
    tm1637Start();
    tm1637Write(0xc0);
    tm1637WaitACK();

    // 显示
    for (int i = 0; i < 4; i++) {
        tm1637Write(num2Display[i]);
        tm1637WaitACK();
    }
    tm1637Stop();

    // 亮度
    tm1637Start();
    tm1637Write(0x8c);
    tm1637WaitACK();
    tm1637Stop();
}

void tm1637DisplayInt(int i)
{
    char buf[8] = {0};
    if (!itoa(i, buf, 10)) {
        tm1637Display("0000", 4);
    }
    int len = strlen(buf);
    tm1637Display(buf, len);
}

#include "max7219.h"

#include <stdio.h>
#include <stdarg.h>

#define CS_LOW()    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET)
#define CS_HIGH()   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET)

static uint8_t SYMBOLS[] = {
    0x7E,	//  numeric 0
    0x30,	//  numeric 1
    0x6D,	//  numeric 2
    0x79,	//  numeric 3
    0x33,	//  numeric 4
    0x5B,	//  numeric 5
    0x5F,	//  numeric 6
    0x70,	//  numeric 7
    0x7F,	//  numeric 8
    0x7B,	//  numeric 9
    0x01,	//  minus
    0x4F,	//  letter E
    0x37,	//  letter H
    0x0E,	//  letter L
    0x67,	//  letter P
    0x00	//  blank
};

void max7219Init(uint8_t brightness)
{
    // 初始化CS脚用到的GPIO
    GPIO_InitTypeDef csGPIO;
    csGPIO.Mode = GPIO_MODE_OUTPUT_PP;
    csGPIO.Pin = GPIO_PIN_4;
    csGPIO.Pull = GPIO_NOPULL;
    csGPIO.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOA, &csGPIO);

    // 初始化MAX7219相关寄存器
    max7219TurnOff();
    max7219Write(REG_SCAN_LIMIT, MAX7219_DISPLAY_NUM-1);
    max7219Write(REG_INTENSITY, brightness);

    // 不使用bcd解码模式
    max7219DecodeOff();

    // 清零
    max7219Clear();

    // 打开显示
    max7219TurnOn();
}

void max7219TurnOn()
{
    max7219Write(REG_SHUTDOWN, 0x01);
}

void max7219TurnOff()
{
    max7219Write(REG_SHUTDOWN, 0x00);
}

void max7219Write(uint8_t regAddr, uint8_t regData)
{
    // printf("addr: [%d], data: [%d]\r\n", regAddr, regData);

    // 拉低片选
    CS_LOW();

    HAL_SPI_Transmit(&hspi1, &regAddr, 1, HAL_MAX_DELAY);
    HAL_SPI_Transmit(&hspi1, &regData, 1, HAL_MAX_DELAY);

    // 恢复片选
    CS_HIGH();
}

void max7219Clear(void)
{
    for (int i = 0; i < MAX7219_DISPLAY_NUM; i++) {
        max7219Write(i+1, (0x00));
    }
}

void max7219DecodeOn()
{
    max7219Write(REG_DECODE_MODE, 0xFF);
}

void max7219DecodeOff()
{
    max7219Write(REG_DECODE_MODE, 0x00);
}

void led_printf(const char *fmt, ...)
{
    char led_buf[8] = {0x00};
    char buf[128] = {0x00};

    va_list vl;
    va_start(vl, fmt);
    int vret = vsnprintf(buf, sizeof(buf),fmt, vl);
    va_end(vl);

    int j = 0;
    for (int i = 0; i < vret; ) {
        char ch = buf[i];
        char chNext = 0;
        if (i + 1 < vret) {
            chNext = buf[i+1];
        }

        if ((ch >= '0' && ch <= '9')) {
            if (chNext == '.') {
                led_buf[j] = (SYMBOLS[ch-'0'] | 0x80);
                j++;
                i+=2;
            } else {
                led_buf[j] = (SYMBOLS[ch-'0']);
                j++;
                i++;
            }
        } else if (ch == '.') {
            led_buf[j] = (0x80);
            j++;
            i++;
        } else {
            i++;
        }
    }

    for (int i = 0; i < j; i++) {
        max7219Write(i+1, led_buf[j-i-1]);
    }
}

void led_raw_print(uint8_t data[8])
{
    for (int i = 0; i < MAX7219_DISPLAY_NUM; i++) {
        max7219Write(i+1, data[i]);
    }
}

int isLedPrintable(char ch)
{

}
#include "lcd1602.h"
#include "main.h"

#include <stdio.h>

#define PCF8574_ADDR 0x4E
extern I2C_HandleTypeDef hi2c2;

void LCD_SendCmd(uint8_t cmd)
{
    uint8_t buffer[4] = {0};
    // cmd拆成两部分
    uint8_t cmdH = cmd & 0xF0;
    uint8_t cmdL = (cmd << 4) & 0xF0;

    // 先传高位部分, 再传低位部分
    buffer[0] = cmdH | 0x0C;    //rs=0, rw =0, en=1, bk=1
    buffer[1] = cmdH | 0x08;    //rs=0, rw =0, en=0, bk=1
    buffer[2] = cmdL | 0x0C;    //rs=0, rw =0, en=1, bk=1
    buffer[3] = cmdL | 0x08;    //rs=0, rw =0, en=0, bk=1

    if (HAL_I2C_Master_Transmit(&hi2c2, PCF8574_ADDR, (uint8_t *)buffer, 4, HAL_MAX_DELAY) == HAL_OK) {
        printf("send cmd 0x%02X success!\r\n", cmd);
    } else {
        printf("send cmd 0x%02X failed!\r\n", cmd);
    }
}

void LCD_SendData(uint8_t data)
{
    uint8_t buffer[4] = {0};
    // cmd拆成两部分
    uint8_t cmdH = data & 0xF0;
    uint8_t cmdL = (data << 4) & 0xF0;

    // 先传高位部分, 再传低位部分
    buffer[0] = cmdH | 0x0D;    // rs=1 rw =0, en=1, bk=1
    buffer[1] = cmdH | 0x09;    // rs=1 rw =0, en=0, bk=1
    buffer[2] = cmdL | 0x0D;    // rs=1 rw =0, en=1, bk=1
    buffer[3] = cmdL | 0x09;    // rs=1 rw =0, en=0, bk=1

    HAL_I2C_Master_Transmit(&hi2c2, PCF8574_ADDR, (uint8_t *)buffer, 4, HAL_MAX_DELAY);
}

void LCD_BackLightOff(void)
{
    int buffer = 0x00;  // rs=0 rw =0, en=0, bk=0
    HAL_I2C_Master_Transmit(&hi2c2, PCF8574_ADDR, (uint8_t *)buffer, 1, HAL_MAX_DELAY);
}

void LCD_BackLightOn(void)
{
    int buffer = 0x08;  // rs=0 rw =0, en=0, bk=1
    HAL_I2C_Master_Transmit(&hi2c2, PCF8574_ADDR, (uint8_t *)buffer, 1, HAL_MAX_DELAY);
}

void LCD_Init()
{
    // 4-bit
    // init 4-bit mode
    HAL_Delay(50);
    LCD_SendCmd(0x30);
    HAL_Delay(5);
    LCD_SendCmd(0x30);
    HAL_Delay(1);
    LCD_SendCmd(0x30);
    HAL_Delay(10);
    LCD_SendCmd(0x20);
    HAL_Delay(10);

    // init display
    LCD_SendCmd(0x28);
    HAL_Delay(1);
    LCD_SendCmd(0x08);
    HAL_Delay(1);
    LCD_SendCmd(0x01);
    HAL_Delay(1);
    HAL_Delay(1);
    LCD_SendCmd(0x06);
    HAL_Delay(1);
    LCD_SendCmd(0x0C);
    HAL_Delay(1);
}

void LCD_ShowString(const char *str)
{
    while (*str)
    {
        LCD_SendData(*str++);
    }
    HAL_Delay(1);
}
#include "lcd1602.h"
#include "main.h"

void LCD_SendCmd(uint8_t cmd)
{
    uint8_t buffer[4] = {0};
    // cmd拆成两部分
    uint8_t cmdH = cmd & 0xF0;
    uint8_t cmdL = (cmd << 4) & 0xF0;

    // 先传高位部分, 再传低位部分
    buffer[0] = cmdH | 0x0C;    // en=1, rs=0
    buffer[1] = cmdH | 0x08;    // en=0, rs=0
    buffer[2] = cmdL | 0x0C;    // en=1, rs=0
    buffer[3] = cmdL | 0x08;    // en=0, rs=0

    // HAL_I2C_Master_Transmit(&hi2c2, PCF8574_ADDR, (uint8_t *)buffer, 4, HAL_MAX_DELAY);
    HAL_Delay(1);
}

void LCD_SendData(uint8_t data)
{
    uint8_t buffer[4] = {0};
    // cmd拆成两部分
    uint8_t cmdH = data & 0xF0;
    uint8_t cmdL = (data << 4) & 0xF0;

    // 先传高位部分, 再传低位部分
    buffer[0] = cmdH | 0x0D;    // en=1, rs=1
    buffer[1] = cmdH | 0x09;    // en=0, rs=1
    buffer[2] = cmdL | 0x0D;    // en=1, rs=1
    buffer[3] = cmdL | 0x09;    // en=0, rs=1

    // HAL_I2C_Master_Transmit(&hi2c2, PCF8574_ADDR, (uint8_t *)buffer, 4, HAL_MAX_DELAY);
    HAL_Delay(1);
}

void LCD_Init()
{
    LCD_SendCmd(0x02);
    LCD_SendCmd(0x33);
    LCD_SendCmd(0x32);
    LCD_SendCmd(0x28);
    LCD_SendCmd(0x06);
    LCD_SendCmd(0x0C);
    // LCD_SendCmd(0x01);
    LCD_SendCmd(0x80);
}

void LCD_ShowString(const char *str)
{
    while (*str)
    {
        LCD_SendData(*str++);
    }
    HAL_Delay(1);
}
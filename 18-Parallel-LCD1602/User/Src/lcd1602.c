#include "lcd1602.h"
#include "main.h"

#define DATA_GPIO_Port GPIOA

static inline void LCD_RS_L()
{
    HAL_GPIO_WritePin(LCD1602_RS_GPIO_Port, LCD1602_RS_Pin, GPIO_PIN_RESET);
}

static inline void LCD_RS_H()
{
    HAL_GPIO_WritePin(LCD1602_RS_GPIO_Port, LCD1602_RS_Pin, GPIO_PIN_SET);
}

static inline void LCD_RW_L()
{
    HAL_GPIO_WritePin(LCD1602_RW_GPIO_Port, LCD1602_RW_Pin, GPIO_PIN_RESET);
}

static inline void LCD_RW_H()
{
    HAL_GPIO_WritePin(LCD1602_RW_GPIO_Port, LCD1602_RW_Pin, GPIO_PIN_SET);
}

static inline void LCD_EN_L()
{
    HAL_GPIO_WritePin(LCD1602_EN_GPIO_Port, LCD1602_EN_Pin, GPIO_PIN_RESET);
}

static inline void LCD_EN_H()
{
    HAL_GPIO_WritePin(LCD1602_EN_GPIO_Port, LCD1602_EN_Pin, GPIO_PIN_SET);
}

static inline void LCD_W_Data(uint8_t d)
{
    // 直接写输出数据寄存器操作bit4-bit7
    DATA_GPIO_Port->ODR = d;
}

static int LCD_GetBusyStatus()
{
    LCD_RS_L();
    LCD_RW_H();
    int status = HAL_GPIO_ReadPin(DATA_GPIO_Port, GPIO_PIN_7);
    LCD_EN_H();
    LCD_EN_L();

    return status;
}

void LCD_SendCmd(uint8_t cmd)
{
    LCD_RS_L();
    LCD_RW_L();
    LCD_W_Data(cmd);
    LCD_EN_H();
    LCD_EN_L();
    HAL_Delay(10);
}

void LCD_SendData(uint8_t data)
{
    while (LCD_GetBusyStatus() == 1)
    {
        
    }
    LCD_RS_H();
    LCD_RW_L();
    LCD_W_Data(data);
    LCD_EN_H();
    LCD_EN_L();
    HAL_Delay(1);
}

void LCD_Init()
{
    LCD_SendCmd(0x38);
    LCD_SendCmd(0x0F);
    LCD_SendCmd(0x06);
    LCD_SendCmd(0x01);
}

void LCD_ShowString(const char *str)
{
    while (*str)
    {
        LCD_SendData(*str++);
    }
}
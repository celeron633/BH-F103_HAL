#include "lcd1602.h"
#include "main.h"
#include "delay.h"

#define DATA_GPIO_Port GPIOA
#define DATA_4BIT_MODE

#define RS_Pin GPIO_PIN_9
#define RS_GPIO_Port GPIOB
#define RW_Pin GPIO_PIN_10
#define RW_GPIO_Port GPIOB
#define EN_Pin GPIO_PIN_11
#define EN_GPIO_Port GPIOB
#define D4_Pin GPIO_PIN_4
#define D4_GPIO_Port GPIOA
#define D5_Pin GPIO_PIN_5
#define D5_GPIO_Port GPIOA
#define D6_Pin GPIO_PIN_6
#define D6_GPIO_Port GPIOA
#define D7_Pin GPIO_PIN_7
#define D7_GPIO_Port GPIOA

// from delay.h us level delay
#define delay(i) delay_us(i)

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

#ifdef DATA_8BIT_MODE
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
    delay(20);
    LCD_EN_L();
}

void LCD_SendData(uint8_t data)
{
    // while (LCD_GetBusyStatus() == 1);
    LCD_RS_H();
    LCD_RW_L();
    LCD_W_Data(data);
    LCD_EN_H();
    delay(20);
    LCD_EN_L();
}
#else
void LCD_Send(uint8_t data, int rs)
{
    HAL_GPIO_WritePin(RS_GPIO_Port, RS_Pin, rs);

    // write bit to pin
    HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, (data>>3)&0x01);
    HAL_GPIO_WritePin(D6_GPIO_Port, D6_Pin, (data>>2)&0x01);
    HAL_GPIO_WritePin(D5_GPIO_Port, D5_Pin, (data>>1)&0x01);
    HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, (data>>0)&0x01);

    // flush to memory while en falling edge
    HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, 1);
    delay(20);
    HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, 0);
    delay(20);
}

void LCD_SendCmd(uint8_t cmd)
{
    uint8_t data2Send;
    data2Send = (cmd >> 4) & 0x0F;
    LCD_Send(data2Send, 0);

    data2Send = cmd & 0x0F;
    LCD_Send(data2Send, 0);
}

void LCD_SendData(uint8_t data)
{
    uint8_t data2Send;
    data2Send = (data >> 4) & 0x0F;
    LCD_Send(data2Send, 1);

    data2Send = data & 0x0F;
    LCD_Send(data2Send, 1);
}
#endif

void LCD_Init()
{
#ifdef DATA_8BIT_MODE
    // 8-bit
    // LCD_delay();
    LCD_SendCmd(0x38);
    LCD_SendCmd(0x01);
    LCD_SendCmd(0x06);
    LCD_SendCmd(0x0F);
#else
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
    LCD_SendCmd(0x06);
    HAL_Delay(1);
    LCD_SendCmd(0x0C);
    HAL_Delay(1);
#endif
}

void LCD_ShowString(const char *str)
{
    while (*str)
        LCD_SendData(*str++);
}
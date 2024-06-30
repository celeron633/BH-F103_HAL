#include "ili9341.h"

#include <stdio.h>

// 命令与数据相关
static void ili9341WriteCommand(uint16_t cmd)
{
    *(__IO uint16_t*)FSMC_LCD_COMMAND_ADDR = cmd;
}

static void ili9341WriteData(uint16_t d)
{
    *(__IO uint16_t*)FSMC_LCD_DATA_ADDR = d;
}

static uint16_t ili9341ReadData(void)
{
    return *(__IO uint16_t*)FSMC_LCD_DATA_ADDR;
}

void backlightControl(FunctionalState state)
{
    if (state == ENABLE) {
        HAL_GPIO_WritePin(GPIOG, GPIO_LCD_BL_Pin, GPIO_PIN_RESET);
    } else {
        HAL_GPIO_WritePin(GPIOG, GPIO_LCD_BL_Pin, GPIO_PIN_SET);
    }
}

void ili9341Reset()
{
    HAL_GPIO_WritePin(GPIOG, GPIO_LCD_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(GPIOG, GPIO_LCD_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(120);
}

uint16_t ili9341ReadID()
{
    uint16_t id = 0;

    // 我的这款实际上是ST7789V了
    ili9341WriteCommand(0x04);
    ili9341ReadData();
    ili9341ReadData();
    id = ili9341ReadData();
    id <<= 8;
    id |= ili9341ReadData();

    printf("lcd id is [0x%x]\n", id);

    return id;
}

void ili9341Init()
{
    // 开启背光
    backlightControl(ENABLE);
    // 复位ILI9341
    ili9341Reset();
}



#include "ili9341.h"

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

void ili9341Reset()
{
    HAL_GPIO_WritePin(GPIOG, GPIO_LCD_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(GPIOG, GPIO_LCD_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(120);
}

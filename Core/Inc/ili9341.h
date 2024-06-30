#ifndef __ILI9341_H__
#define __ILI9341_H__

#include <stdint.h>

#include "main.h"
#include "core_cm3.h"
#include "stm32f103xe.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_ll_fsmc.h"

#include "fsmc.h"

#define FSMC_LCD_BASE_ADDR      0x6C000000
// #define FSMC_LCD_COMMAND_ADDR   ((FSMC_LCD_BASE_ADDR) & ~(1 << (23+1)))
// 第24位为0, ADDR对应A23为低电平
#define FSMC_LCD_COMMAND_ADDR   0x6C000000
// #define FSMC_LCD_DATA_ADDR      ((FSMC_LCD_BASE_ADDR) | (1 << (23+1)))
// 第24位为1, ADDR对应A23为高电平
#define FSMC_LCD_DATA_ADDR      0x6D000000

// 写命令
static void     ili9341WriteCommand(uint16_t cmd);
// 写数据
static void     ili9341WriteData(uint16_t d);
// 读数据
static uint16_t ili9341ReadData(void);

// 9341复位
void ili9341Reset();
// 9341初始化, 初始化前先完成GPIO FSMC的初始化
void ili9341Init();

#endif
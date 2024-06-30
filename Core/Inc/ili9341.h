#ifndef __ILI9341_H__
#define __ILI9341_H__

#include <stdint.h>

#include "main.h"
#include "core_cm3.h"
#include "stm32f103xe.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_ll_fsmc.h"

#include "fsmc.h"

#include "fonts.h"

// 屏幕实际型号是ST7789V, 没有对ILI9341做兼容, 实际在设置Gram和初始化屏幕主控寄存器时有差异

/* 定义 ILI934 常用命令 */
#define     CMD_SetCoordinateX		 		    0x2A	     // 设置X坐标
#define     CMD_SetCoordinateY		 		    0x2B	     // 设置Y坐标
#define     CMD_SetPixel		 		        0x2C	     // 填充像素

/* 定义 LCD 驱动芯片 ID */
#define     LCDID_UNKNOWN                       0           // 未知
#define     LCDID_ILI9341                       0x9341      // ILI9341
#define     LCDID_ST7789V                       0x8552      // ST7789V

/* ILI934 显示区域的起始坐标和总行列数 */
#define     ILI9341_DispWindow_X_Star		    0           // 起始点的X坐标
#define     ILI9341_DispWindow_Y_Star		    0           // 起始点的Y坐标

#define     ILI9341_LESS_PIXEL	  		        240			// 液晶屏较短方向的像素宽度
#define     ILI9341_MORE_PIXEL	 		        320			// 液晶屏较长方向的像素宽度

/******************************* 定义 ILI934 显示屏常用颜色 ********************************/
#define      BACKGROUND		               BLACK   //默认背景颜色

#define      WHITE		 		           0xFFFF	   //白色
#define      BLACK                         0x0000	   //黑色 
#define      GREY                          0xF7DE	   //灰色 
#define      BLUE                          0x001F	   //蓝色 
#define      BLUE2                         0x051F	   //浅蓝色 
#define      RED                           0xF800	   //红色 
#define      MAGENTA                       0xF81F	   //红紫色，洋红色 
#define      GREEN                         0x07E0	   //绿色 
#define      CYAN                          0x7FFF	   //蓝绿色，青色 
#define      YELLOW                        0xFFE0	   //黄色 
#define      BRED                          0xF81F
#define      GRED                          0xFFE0
#define      GBLUE                         0x07FF

// FSMC HADDR地址
#define FSMC_LCD_BASE_ADDR      0x6C000000
// #define FSMC_LCD_COMMAND_ADDR   ((FSMC_LCD_BASE_ADDR) & ~(1 << (23+1)))
// 第24位为0, ADDR对应A23为低电平
#define FSMC_LCD_COMMAND_ADDR   0x6C000000
// #define FSMC_LCD_DATA_ADDR      ((FSMC_LCD_BASE_ADDR) | (1 << (23+1)))
// 第24位为1, ADDR对应A23为高电平
#define FSMC_LCD_DATA_ADDR      0x6D000000

// 背光控制
void backlightControl(FunctionalState state);
// 9341复位
void ili9341Reset();
// 9341初始化, 初始化前先完成GPIO FSMC的初始化
void ili9341Init();
// 读取芯片ID
uint16_t ili9341ReadID();
// 打开窗口
void ili9341OpenWindow(uint16_t x, uint16_t y, uint16_t width, uint16_t height);

// 显示字符
void ili9341ShowChar(uint16_t x, uint16_t y, char c);
// 显示字符串
void ili9341ShowString(uint16_t x, uint16_t y, const char* str, size_t len);

#endif
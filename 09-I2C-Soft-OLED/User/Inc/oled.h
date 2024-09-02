#ifndef __OLED_H__
#define __OLED_H__

#include "main.h"

#ifdef HARDWARE_I2C
#include "i2c.h"
#endif

#include "stm32f1xx_hal_uart.h"
#include "stm32f103xe.h"

extern struct OLED_Config g_oledCfg;

struct OLED_Config {
#ifdef HARDWARE_I2C
    I2C_HandleTypeDef *i2cHandle;
#endif
    uint8_t i2cAddr;
};

#ifdef HARDWARE_I2C
void OLED_ConfigDisplay(I2C_HandleTypeDef *handle, uint8_t i2cAddr);
#else 
void OLED_ConfigDisplay(uint8_t i2cAddr);
#endif

int  OLED_InitDisplay();

void OLED_SetPos(uint8_t X, uint8_t Y);
void OLED_SetCursor(uint8_t page, uint8_t X);
void OLED_Fill(uint8_t data);

void OLED_NewFrame();
void OLED_ClearArea(int16_t X, int16_t Y, uint8_t width, uint8_t height);
void OLED_ShowFrame();
void OLED_SetPixel(int X, int Y);

void OLED_ShowImage(int16_t X, int16_t Y, uint8_t width, uint8_t height, const uint8_t *image);

void OLED_ShowChar(uint8_t X, uint8_t Y, char c);
void OLED_ShowString(uint8_t X, uint8_t Y, const char *str);

void OLED_DrawRectangle(int16_t X, int16_t Y, uint8_t width, uint8_t height);

// TEST
void OLED_Test();

#endif
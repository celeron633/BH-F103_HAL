#ifndef __OLED_H__
#define __OLED_H__

#include "main.h"
#include "i2c.h"
#include "stm32f1xx_hal_uart.h"
#include "stm32f103xe.h"

extern struct OLED_Config g_oledCfg;

struct OLED_Config {
    I2C_HandleTypeDef *i2cHandle;
    uint8_t i2cAddr;
};

void oledConfigDisplay(I2C_HandleTypeDef *handle, uint8_t i2cAddr);
int  oledInitDisplay();

void oledSetPos(uint8_t xPos, uint8_t yPos);
void oledFill(uint8_t data);
void oledPrint(uint8_t xPos, uint8_t yPos, const char *str);
void oledShowChar(uint8_t xPos, uint8_t yPos, char c);
void oledShowString(uint8_t xPos, uint8_t yPos, char str[]);

// int oledWriteCmd(uint8_t cmd);
// int oledWriteData(uint8_t data);

#endif
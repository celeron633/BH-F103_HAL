#ifndef __OLED_H__
#define __OLED_H__

#include "main.h"
#include "i2c.h"
#include "stm32f1xx_hal_uart.h"
#include "stm32f103xe.h"

extern struct OLED_Config oledCfg;

struct OLED_Config {
    I2C_HandleTypeDef *i2cHandle;
    uint8_t i2cAddr;
};

void configOledDisplay(I2C_HandleTypeDef *h, uint8_t i2cAddr);
int initOledDisplay();

// int oledWriteCmd(uint8_t cmd);
// int oledWriteData(uint8_t data);

#endif
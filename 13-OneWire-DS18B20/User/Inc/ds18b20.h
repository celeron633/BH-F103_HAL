#ifndef __DS18B20_H__
#define __DS18B20_H__

#include <stdint.h>

#include "main.h"
#include "stm32f1xx_hal_gpio.h"

#define DS18B20_GPIO                GPIOF
#define DS18B20_DQ_PIN              GPIO_PIN_9 
#define DS18B20_CLK_ENABLE()        __HAL_RCC_GPIOF_CLK_ENABLE()

void    DS18B20_Init(void);
int     DS18B20_Reset(void);

void    DS18B20_WriteByte(uint8_t data);
uint8_t DS18B20_ReadByte();

int     DS18B20_ReadTemperature(double *temp);

#endif
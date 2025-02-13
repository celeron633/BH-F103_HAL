#ifndef __MY_I2C_H__
#define __MY_I2C_H__

#include "main.h"
#include "stm32f1xx_hal_i2c.h"

#include <stdint.h>

#define I2C_RCC
#define I2C_GPIO        GPIOB
#define I2C_CLK_PIN     GPIO_PIN_6
#define I2C_SDA_PIN     GPIO_PIN_7

void    I2C_Init(void);
void    I2C_Start(void);
void    I2C_Stop(void);
void    I2C_WriteByte(uint8_t data);
uint8_t I2C_ReadByte(void);
void    I2C_WriteAck();
void    I2C_WriteNAck();
uint8_t I2C_ReadAck(void);

void    I2C_Test(void);

void I2C_WriteData(uint8_t addr, uint8_t *data, size_t len);

#endif
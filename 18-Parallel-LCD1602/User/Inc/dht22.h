#ifndef __DHT22_H__
#define __DHT22_H__

#include <stdint.h>
#include "main.h"
#include "stm32f1xx_hal_gpio.h"

#define DHT22_GPIO                  GPIOF
#define DHT22_DIO_PIN               GPIO_PIN_11 
#define DHT22_CLK_ENABLE()          __HAL_RCC_GPIOF_CLK_ENABLE()

#define DIO_HIGH()                  HAL_GPIO_WritePin(DHT22_GPIO, DHT22_DIO_PIN, GPIO_PIN_SET)
#define DIO_LOW()                   HAL_GPIO_WritePin(DHT22_GPIO, DHT22_DIO_PIN, GPIO_PIN_RESET)
#define DIO_READ()                  HAL_GPIO_ReadPin(DHT22_GPIO, DHT22_DIO_PIN)

void    DHT22_Init(void);
int     DHT22_Measure(double *temperature, double *humidity);

#endif
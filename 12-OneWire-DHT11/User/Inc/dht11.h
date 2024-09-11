#ifndef __DHT11_H__
#define __DHT11_H__

#include <stdint.h>
#include "main.h"
#include "stm32f1xx_hal_gpio.h"
        
#define DHT11_GPIO                  GPIOF
#define DHT11_DIO_PIN               GPIO_PIN_9 
#define DHT11_CLK_ENABLE()          __HAL_RCC_GPIOF_CLK_ENABLE()

#define DIO_HIGH()                  HAL_GPIO_WritePin(DHT11_GPIO, DHT11_DIO_PIN, GPIO_PIN_SET)
#define DIO_LOW()                   HAL_GPIO_WritePin(DHT11_GPIO, DHT11_DIO_PIN, GPIO_PIN_RESET)
#define DIO_READ()                  HAL_GPIO_ReadPin(DHT11_GPIO, DHT11_DIO_PIN)

void    DHT11_Init(void);
int     DHT11_Start(void);
int     DHT11_ReadData(uint8_t *data);

#endif
#ifndef __DHT11_H__
#define __DHT11_H__

#include "main.h"
#include "stm32f1xx_hal_tim.h"
#include "stm32f1xx_ll_tim.h"
#include "stm32f1xx_hal_gpio.h"

#include <stdint.h>

#define DHT11_DATA_GPIO             GPIOA
#define DHT11_DATA_PORT             GPIO_PIN_5
#define DHT11_DATA_TIM              TIM6


// 初始化
void dht11Init();
// DHT11复位, 返回0成功, -1失败
int  dht11Reset();
int  dht11ReadData();

#endif
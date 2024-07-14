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
void dht11Reset();
// 检查DHT11复位的情况
int dht11Check();
// 读取一个bit
uint8_t dht11ReadBit(void);
// 读取一个字节
uint8_t dht11ReadByte(void);
// 读取温度数据
int dht11ReadData(uint8_t *tempPart1, uint8_t *tempPart2,uint8_t *humi);

#endif
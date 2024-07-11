#ifndef __DHT11_H__
#define __DHT11_H__

#include "main.h"
#include "stm32f1xx_hal_tim.h"
#include "stm32f1xx_ll_tim.h"
#include "stm32f1xx_hal_gpio.h"

#include <stdint.h>

typedef enum
{
    DHT_Type_DHT11 = 0,
    DHT_Type_DHT12,
    DHT_Type_DHT21,
    DHT_Type_DHT22,
    DHT_Type_AM2301,
    DHT_Type_AM2305
} DHT_Type_t;

typedef struct DHT11HandleType {
    GPIO_TypeDef    *gpio;
    uint32_t        gpioPin;
    TIM_TypeDef     *timer;
} DHT11HandleType;

void dht11Init(DHT11HandleType handle)
{
    /*
    // 初始化GPIO
    GPIO_InitTypeDef gpio;
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pin = handle.gpioPin;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(handle.gpio, &gpio);

    // 初始化用于微秒计时定时器
    TIM_HandleTypeDef htim6;
    htim6.Instance = TIM6;
    htim6.Init.Prescaler = (SystemCoreClock/1000000)-1;
    htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim6.Init.Period = 0xFFFF;
    htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    (void)HAL_TIM_Base_Init(&htim6);
    */
}

void  DHT_init(DHT11HandleType *dht, DHT_Type_t type, TIM_HandleTypeDef *tim, uint16_t  timerBusFrequencyMHz, GPIO_TypeDef *gpio, uint16_t  pin);
bool  DHT_readData(DHT11HandleType *dht, float *Temperature, float *Humidity);

#endif
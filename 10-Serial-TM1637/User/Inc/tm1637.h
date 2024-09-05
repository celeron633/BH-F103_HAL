#ifndef __TM1637_H__
#define __TM1637_H__

#include "main.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f103xe.h"

#define TM1637_CLK  GPIO_PIN_9
#define TM1637_DIO  GPIO_PIN_10
#define TM1637_GPIO GPIOF

void tm1637Init(void);
void tm1637Start(void);
void tm1637Stop(void);
void tm1637Write(uint8_t dat);
void tm1637WaitACK(void);
void tm1637Display(const char *dat, size_t len);
void tm1637DisplayInt(int i);

#endif
#ifndef __DELAY_H__
#define __DELAY_H__

#include "main.h"
#include "stm32f1xx_hal_tim.h"

void delay_us(uint16_t us);
void delay_ms(uint32_t ms);

#endif
#ifndef __DELAY_H__
#define __DELAY_H__

#include "main.h"
#include "stm32f1xx_hal_tim.h"
#include "stm32f1xx_ll_tim.h"

#include <stdint.h>

void tim_delay_us(uint32_t time);

#endif
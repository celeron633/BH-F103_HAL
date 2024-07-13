#include "delay.h"
#include "stm32f103xe.h"

#include <stdint.h>

extern TIM_HandleTypeDef htim6;

void tim_delay_us(uint32_t time)
{
    __HAL_TIM_SET_COUNTER(&htim6, 0);
    while (__HAL_TIM_GET_COUNTER(&htim6) < time)
    {
        
    }
}
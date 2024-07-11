#include "delay.h"
#include "stm32f103xe.h"

#include <stdint.h>

void tim_delay_us(uint32_t us)
{
    /*
    TIM6->CNT = 0;
    while (TIM6->CNT < us)
    {
        
    }
    */

   volatile uint32_t curVal = TIM6->CNT;
   while (TIM6->CNT - curVal < us)
   {
    
   }
   
}
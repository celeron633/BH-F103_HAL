#include "my_tim.h"

TIM_HandleTypeDef htim2;

HAL_StatusTypeDef InitTIM2(void)
{
    // 开启TIM2的时钟
    __HAL_RCC_TIM2_CLK_ENABLE();

    // 定时器时基单元
    TIM_Base_InitTypeDef tim2BaseInit;
    // 启用影子寄存器
    tim2BaseInit.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    tim2BaseInit.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    tim2BaseInit.CounterMode = TIM_COUNTERMODE_UP;
    // 一秒一个UPDATE
    // 设置ARR为10000
    tim2BaseInit.Period = 10000 - 1;
    // 7200分频
    tim2BaseInit.Prescaler = 7200 - 1;
    tim2BaseInit.RepetitionCounter = 0;

    // 句柄设置
    htim2.Instance = TIM2;
    htim2.Init = tim2BaseInit;
    
    HAL_StatusTypeDef ret = HAL_TIM_Base_Init(&htim2);
    if (ret != HAL_OK) {
        return ret;
    }

    // 设置时钟源
    TIM_ClockConfigTypeDef clockConfig;
    // 设置为内部时钟源
    clockConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    ret = HAL_TIM_ConfigClockSource(&htim2, &clockConfig);
    if (ret != HAL_OK) {
        return ret;
    }

    return HAL_OK;
}
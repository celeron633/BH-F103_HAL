#include "tpad.h"

#include <stdint.h>
#include <stdio.h>

extern TIM_HandleTypeDef htim2;

GPIO_InitTypeDef TPAD_GPIOStruct = {0};
struct TPAD_Config tpadConfig = {
    .initSamples = 10,
    .scanSamples = 3,
    .initChargeDelay = 0
};

static inline void SetGPIO4Output(void)
{
    TPAD_GPIOStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2;
    TPAD_GPIOStruct.Mode = GPIO_MODE_OUTPUT_PP;
    TPAD_GPIOStruct.Pull = GPIO_NOPULL;
    TPAD_GPIOStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOA, &TPAD_GPIOStruct);
}

static inline void SetGPIO4TIM(void)
{
    TPAD_GPIOStruct.Pin = GPIO_PIN_1;
    TPAD_GPIOStruct.Mode = GPIO_MODE_INPUT;
    TPAD_GPIOStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &TPAD_GPIOStruct);
}

static void DischargeTPAD(void)
{
    SetGPIO4Output();
    // 将触摸按键放电
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_Delay(50);
    // 重新设置PA1为TIM的IC的输入, 同时触摸电容开始充电
    SetGPIO4TIM();
}

void TPAD_Init()
{
    // 开启定时器的输入捕获
    HAL_TIM_IC_Start(&htim2, TIM_CHANNEL_2);
    __HAL_TIM_DISABLE(&htim2);

    static uint32_t countValues[10] = {0};
    uint32_t countValue = 0;
    for (int i = 0; i < tpadConfig.initSamples; i++) {
        printf("cal %d start!\r\n", i);
        
        // CNT寄存器先设置为0
        __HAL_TIM_SET_COUNTER(&htim2, 0);
        // 将触摸按键放电
        DischargeTPAD();
        // 开启定时器
        __HAL_TIM_ENABLE(&htim2);
        // 等待变成逻辑1
        while (__HAL_TIM_GET_FLAG(&htim2, TIM_FLAG_CC2) == 0);
        // 获取捕获寄存器的值
        countValue = __HAL_TIM_GET_COMPARE(&htim2, TIM_CHANNEL_2);
        countValues[i] = countValue;
        // 暂停定时器
        __HAL_TIM_DISABLE(&htim2);
        // 清除捕获标识位
        __HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_CC2);

        printf("cal %d end! count: %lu\r\n", i, countValue);
    }
}

void TPAD_Test()
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    // __HAL_RCC_GPIOB_CLK_ENABLE();

    SetGPIO4Output();
    // 将触摸按键放电
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
}

int TPAD_Scan()
{
    

    return 0;
}
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
    TPAD_GPIOStruct.Mode = GPIO_MODE_OUTPUT_OD;
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
    // 设置GPIO为输出模式
    SetGPIO4Output();
    // 将触摸按键放电, 本来就使用了一个1M的电阻上拉, 使用OD模式即可
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
    // 等待5ms确保全部释放掉
    HAL_Delay(5);
    // 重新设置PA1为TIM的IC的输入, 同时触摸电容开始充电
    SetGPIO4TIM();
}

void TPAD_Init()
{
    // 开启定时器的输入捕获
    HAL_TIM_IC_Start(&htim2, TIM_CHANNEL_2);
    int samples = tpadConfig.initSamples;
    if (samples > 128) {
        samples = 128;
    }
    uint32_t compareVals[128] = {0};

    for (int i = 0; i < samples; i++) {
        compareVals[i] = TPAD_GetVal();
    }

    uint64_t tmpVal = 0;
    for (int i = 0; i < samples; i++) {
        tmpVal += compareVals[i];
    }
    tpadConfig.initChargeDelay = tmpVal / samples;
    printf("init charge delay: [%lu] us\r\n", tpadConfig.initChargeDelay);
}

int TPAD_GetVal()
{
    uint32_t countValue = 0;
    // 清除捕获和更新标识
    __HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_CC2 | TIM_FLAG_UPDATE);
    // 放电电容按钮
    DischargeTPAD();
    // 设置定时器CNT值为0
    __HAL_TIM_SET_COUNTER(&htim2, 0);
    while (__HAL_TIM_GET_FLAG(&htim2, TIM_FLAG_CC2) == 0);
    // 等待捕获一个上升沿完成
    countValue = __HAL_TIM_GET_COMPARE(&htim2, TIM_CHANNEL_2);
#if 0
    printf("count: [%lu] us\r\n", countValue);
#endif

    return countValue;
}

int TPAD_Scan()
{
    int samples = tpadConfig.scanSamples;
    if (samples > 128) {
        samples = 128;
    }
    uint32_t compareVals[128] = {0};

    for (int i = 0; i < samples; i++) {
        compareVals[i] = TPAD_GetVal();
    }

    uint64_t tmpVal = 0;
    uint32_t scanDelay = 0;
    for (int i = 0; i < samples; i++) {
        tmpVal += compareVals[i];
    }
    scanDelay = tmpVal / samples;
    // tpadConfig.initChargeDelay = tmpVal / samples;
    printf("scan charge delay: [%lu] us\r\n", scanDelay);

    return scanDelay;
}

void TPAD_Test()
{
    
}
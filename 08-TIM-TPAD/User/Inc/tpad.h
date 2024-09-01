#ifndef __TPAD_H__
#define __TPAD_H__

#include "main.h"
#include "i2c.h"
#include "stm32f1xx_hal_tim.h"
#include "stm32f103xe.h"

#include <stdint.h>

struct TPAD_Config {
    // 初始化取样次数
    int initSamples;
    // 扫描取样次数
    int scanSamples;

    uint32_t initChargeDelay;
};

void TPAD_Init();
int TPAD_Scan();

void TPAD_Test();

#endif
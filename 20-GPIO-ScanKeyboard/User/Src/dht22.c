#include "dht22.h"
#include "delay.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern TIM_HandleTypeDef htim6;

static int DHT22_Start(void);
static int DHT22_ReadData(uint8_t *data);
static int DHT22_ProcessData(uint8_t *data, double *temperature, double *humidity);

void DHT22_Init(void)
{
    DHT22_CLK_ENABLE();

    GPIO_InitTypeDef dht22Gpio;
    dht22Gpio.Mode = GPIO_MODE_OUTPUT_OD;
    dht22Gpio.Pin = DHT22_DIO_PIN;
    dht22Gpio.Pull = GPIO_NOPULL;
    dht22Gpio.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(DHT22_GPIO, &dht22Gpio);
    // 默认释放控制
    DIO_HIGH();
}

static int DHT22_ProcessData(uint8_t *data, double *temperature, double *humidity)
{
    uint8_t humiH = 0;
    uint8_t humiL = 0;
    uint8_t tempH = 0;
    uint8_t tempL = 0;
    uint8_t cSum = 0;

    for (int i = 0; i < 8; i++) {
        if (data[i] == 1) {
            humiH = (humiH << 1) | 1;
        } else {
            humiH = (humiH << 1) | 0;
        }
    }
    printf("humiH: %d\r\n", humiH);

    for (int i = 8; i < 16; i++) {
        if (data[i] == 1) {
            humiL = (humiL << 1) | 1;
        } else {
            humiL = (humiL << 1) | 0;
        }
    }
    printf("humiL: %d\r\n", humiL);

    for (int i = 16; i < 24; i++) {
        if (data[i] == 1) {
            tempH = (tempH << 1) | 1;
        } else {
            tempH = (tempH << 1) | 0;
        }
    }
    printf("tempH: %d\r\n", tempH);

    for (int i = 24; i < 32; i++) {
        if (data[i] == 1) {
            tempL = (tempL << 1) | 1;
        } else {
            tempL = (tempL << 1) | 0;
        }
    }
    printf("tempL: %d\r\n", tempL);

    for (int i = 32; i < 40; i++) {
        if (data[i] == 1) {
            cSum = (cSum << 1) | 1;
        } else {
            cSum = (cSum << 1) | 0;
        }
    }
    printf("checkSum: %d\r\n", cSum);

    if (humiH + humiL + tempH + tempL == cSum) {
        printf("checksum ok\r\n");
    } else {
        printf("checksum error\r\n");
        return -1;
    }

    uint16_t fullHumidity = 0;
    uint16_t fullTemperature = 0;

    fullHumidity    = ((uint16_t)humiH << 8) | (uint16_t)humiL;
    fullTemperature = ((uint16_t)tempH << 8) | (uint16_t)tempL;
    *humidity       = (double)fullHumidity      / 10;
    *temperature    = (double)fullTemperature   / 10;

    return 0;
}

int DHT22_Measure(double *temperature, double *humidity)
{
    printf("wait 2s before reset sensor...\r\n");
    HAL_Delay(2000);
    int nRet = DHT22_Start();
    if (nRet < 0) {
        printf("DHT22 start FAILED!\r\n");
        return -1;
    }

    uint8_t buf[40] = {0};

    nRet = DHT22_ReadData(buf);
    if (nRet < 0) {
        printf("DHT22 read FAILED!\r\n");
        return -1;
    }
#if 0
    printf("raw timing: \r\n");
    for (int i = 0; i < 40; i++) {
        printf("%d ", buf[i]);
    }
    printf("\r\n");
#endif
    for (int i = 0; i < 40; i++) {
        if (buf[i] > 50) {
            buf[i] = 1;
        } else {
            buf[i] = 0;
        }
    }
#if 0
    printf("bits: \r\n");
    for (int i = 0; i < 40; i++) {
        printf("%d ", buf[i]);
    }
    printf("\r\n");
#endif

    if (DHT22_ProcessData(buf, temperature, humidity) < 0) {
        return -1;
    }

    return 0;
}

static int DHT22_Start(void)
{
    DIO_LOW();
    delay_us(1000);
    DIO_HIGH();

    // 等待从机拉低DIO
    __HAL_TIM_SET_COUNTER(&htim6, 0);
    while (DIO_READ() == GPIO_PIN_SET) {
        if (__HAL_TIM_GET_COUNTER(&htim6) > 100) {
            return -1;
        }
    }

    // 等待从机再拉高
    __HAL_TIM_SET_COUNTER(&htim6, 0);
    while (DIO_READ() == GPIO_PIN_RESET) {
        if (__HAL_TIM_GET_COUNTER(&htim6) > 200) {
            return -1;
        }
    }

    return 0;
}

static int DHT22_ReadData(uint8_t *data)
{
    // 等待从机拉低, 准备输出
    __HAL_TIM_SET_COUNTER(&htim6, 0);
    while (DIO_READ() == GPIO_PIN_SET)
    {
        if (__HAL_TIM_GET_COUNTER(&htim6) > 100) {
            printf("err1\r\n");
            return -1;
        }
    }

    // 连续接收80个bit
    for (int i = 0; i < 40; i++) { 
        __HAL_TIM_SET_COUNTER(&htim6, 0);
        while (DIO_READ() == GPIO_PIN_RESET)
        {
            if (__HAL_TIM_GET_COUNTER(&htim6) > 100) {
                printf("err2 i = %d\r\n", i);
                return -1;
            }
        }
        // 记录高电平时间 0维持26-28us, 1维持70微秒
        __HAL_TIM_SET_COUNTER(&htim6, 0);
        while (DIO_READ() == GPIO_PIN_SET)
        {
            if (__HAL_TIM_GET_COUNTER(&htim6) > 200) {
                printf("err3 i = %d\r\n", i);
                return -1;
            }
        }
        data[i] = __HAL_TIM_GET_COUNTER(&htim6);
    }
    
    return 0;
}
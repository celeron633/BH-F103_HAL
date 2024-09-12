#include "dht11.h"
#include "delay.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern TIM_HandleTypeDef htim6;

static int DHT11_Start(void);
static int DHT11_ReadData(uint8_t *data);

void DHT11_Init(void)
{
    // 至少等待2s, 传感器完成初始化
    DHT11_CLK_ENABLE();

    GPIO_InitTypeDef dht11Gpio;
    dht11Gpio.Mode = GPIO_MODE_OUTPUT_OD;
    dht11Gpio.Pin = DHT11_DIO_PIN;
    dht11Gpio.Pull = GPIO_NOPULL;
    dht11Gpio.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(DHT11_GPIO, &dht11Gpio);
    // 默认释放控制
    DIO_HIGH();
}

int DHT11_Measure(double *temperature, double *humidity)
{
    printf("wait 2s before reset sensor...\r\n");
    delay_ms(2000);
    int nRet = DHT11_Start();
    if (nRet < 0) {
        printf("DHT11 start FAILED!\r\n");
        return -1;
    }

    uint8_t buf[40] = {0};

    nRet = DHT11_ReadData(buf);
    if (nRet < 0) {
        printf("DHT11 read FAILED!\r\n");
        return -1;
    }
    for (int i = 0; i < 40; i++) {
        printf("%d ", buf[i]);
    }
    printf("\r\n");
    for (int i = 0; i < 40; i++) {
        if (buf[i] > 50) {
            buf[i] = 1;
        } else {
            buf[i] = 0;
        }
    }
    for (int i = 0; i < 40; i++) {
        printf("%d ", buf[i]);
    }
    printf("\r\n");

    int humiIntPart = 0;
    int humiFloatPart = 0;
    int tempInitPart = 0;
    int tempFloatPart = 0;

    for (int i = 0; i < 8; i++) {
        if (buf[i] == 1) {
            humiIntPart = (humiIntPart << 1) | 1;
        } else {
            humiIntPart = (humiIntPart << 1) | 0;
        }
    }
    printf("humidity int: %d\r\n", humiIntPart);

    for (int i = 8; i < 16; i++) {
        if (buf[i] == 1) {
            humiFloatPart = (humiFloatPart << 1) | 1;
        } else {
            humiFloatPart = (humiFloatPart << 1) | 0;
        }
    }
    printf("humidity float: %d\r\n", humiFloatPart);

    for (int i = 16; i < 24; i++) {
        if (buf[i] == 1) {
            tempInitPart = (tempInitPart << 1) | 1;
        } else {
            tempInitPart = (tempInitPart << 1) | 0;
        }
    }
    printf("temp int: %d\r\n", tempInitPart);

    for (int i = 24; i < 32; i++) {
        if (buf[i] == 1) {
            tempFloatPart = (tempFloatPart << 1) | 1;
        } else {
            tempFloatPart = (tempFloatPart << 1) | 0;
        }
    }
    printf("temp float: %d\r\n", tempFloatPart);

    char strBuf[8] = {0};
    snprintf(strBuf, sizeof(strBuf), "%d.%d", humiIntPart, humiFloatPart);
    *humidity = atof(strBuf);

    snprintf(strBuf, sizeof(strBuf), "%d.%d", tempInitPart, tempFloatPart);
    *temperature = atof(strBuf);

    return 0;
}

static int DHT11_Start(void)
{
    DIO_LOW();
    delay_ms(20);
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

static int DHT11_ReadData(uint8_t *data)
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

    // 接收80个bit
    for (int i = 0; i < 40; i++) { 
        __HAL_TIM_SET_COUNTER(&htim6, 0);
        while (DIO_READ() == GPIO_PIN_RESET)
        {
            if (__HAL_TIM_GET_COUNTER(&htim6) > 100) {
                printf("err2 i = %d\r\n", i);
                return -1;
            }
        }
        // 记录高电平时间 0维持23-27us, 1维持68-74微秒
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
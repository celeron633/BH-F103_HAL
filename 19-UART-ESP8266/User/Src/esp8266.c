#include "esp8266.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;

char atBuffer[512] = {0};

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart3) {
        printf("count: %d, size: %d\r\n", huart->RxXferCount, huart->RxXferSize);
    }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart == &huart3) {
        printf("rxLen: %d, buffer: [%s]\r\n", Size, atBuffer);
        memset(atBuffer, 0x00, sizeof(atBuffer));
        HAL_UARTEx_ReceiveToIdle_IT(&huart3, (uint8_t *)atBuffer, sizeof(atBuffer));
    }
}

void ESP8266_Reset()
{
    // 复位ESP8266
    HAL_GPIO_WritePin(WIFI_RST_GPIO_Port, WIFI_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(WIFI_RST_GPIO_Port, WIFI_RST_Pin, GPIO_PIN_SET);
}

int ESP8266_Init()
{
    printf("ESP8266_Init begin!\r\n");

    // 拉高EN引脚, 模块开始启动
    HAL_GPIO_WritePin(WIFI_EN_GPIO_Port, WIFI_EN_Pin, GPIO_PIN_SET);

    // 复位
    ESP8266_Reset();
    HAL_Delay(100);

    printf("ESP8266_Init end!\r\n");

    return 0;
}
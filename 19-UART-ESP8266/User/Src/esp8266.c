#include "esp8266.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// status
int wifiConnected = 0;

extern UART_HandleTypeDef huart3;



// AT command uart receive buffer
char atBuffer[512] = {0};

// for normal IT mode, receive till complete
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart3) {
        printf("count: %d, size: %d\r\n", huart->RxXferCount, huart->RxXferSize);
    }
}

// for idle modle
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

void ESP8266_Init()
{
    // 拉高EN引脚, 模块开始启动
    HAL_GPIO_WritePin(WIFI_EN_GPIO_Port, WIFI_EN_Pin, GPIO_PIN_SET);
    // 复位
    ESP8266_Reset();
    // 等待模块启动, 启动后uart有一堆信息, 等待一段时间全部忽略掉
    HAL_Delay(500);
}

int ESP8266_CheckAT()
{
    uint16_t rxLen = 0;
    char buffer[16] = {0};

    strncpy(buffer, "AT\r\n", sizeof(buffer));
    HAL_UART_Transmit(&huart3, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);
    memset(atBuffer, 0x00, sizeof(atBuffer));
    HAL_UARTEx_ReceiveToIdle(&huart3, (uint8_t *)atBuffer, sizeof(atBuffer), &rxLen, HAL_MAX_DELAY);

    printf("AT len: [%d], buf: [%s]\r\n", rxLen, atBuffer);\
    if (strstr(atBuffer, "OK") != NULL) {
        printf("AT command OK!\r\n");
        return 0;
    } else {
        printf("AT command FAILED!\r\n");
        return -1;
    }

    return 0;
}

int ESP8266_EnterStationMode()
{
    uint16_t rxLen = 0;
    char buffer[32] = {0};

    strncpy(buffer, "AT+CWMODE=1\r\n", sizeof(buffer));
    HAL_UART_Transmit(&huart3, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);
    memset(atBuffer, 0x00, sizeof(atBuffer));
    HAL_UARTEx_ReceiveToIdle(&huart3, (uint8_t *)atBuffer, sizeof(atBuffer), &rxLen, HAL_MAX_DELAY);

    printf("CWMODE len: [%d], buf: [%s]\r\n", rxLen, atBuffer);
    if (strstr(atBuffer, "OK") != NULL) {
        printf("AT+CWMODE command OK!\r\n");
        return 0;
    } else {
        printf("AT+CWMODE command FAILED!\r\n");
        return -1;
    }
    return 0;
}

void ESP8266_ScanWIFI()
{
    uint16_t rxLen = 0;
    char buffer[32] = {0};

    strncpy(buffer, "AT+CWLAP\r\n", sizeof(buffer));
    HAL_UART_Transmit(&huart3, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);
    HAL_Delay(100);
    memset(atBuffer, 0x00, sizeof(atBuffer));
    HAL_UARTEx_ReceiveToIdle(&huart3, (uint8_t *)atBuffer, sizeof(atBuffer), &rxLen, HAL_MAX_DELAY);

    printf("CWLAP len: [%d], buf: [%s]\r\n", rxLen, atBuffer);
}

int ESP8266_ConnectWIFI()
{
    uint16_t rxLen = 0;
    char buffer[64] = {0};

    strncpy(buffer, "AT+CWJAP=\"TestAP\",\"abcd1234\"\r\n", sizeof(buffer));
    HAL_UART_Transmit(&huart3, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);
    HAL_Delay(100);
    memset(atBuffer, 0x00, sizeof(atBuffer));
    HAL_UARTEx_ReceiveToIdle(&huart3, (uint8_t *)atBuffer, sizeof(atBuffer), &rxLen, HAL_MAX_DELAY);

    printf("CWJAP len: [%d], buf: [%s]\r\n", rxLen, atBuffer);
    if (strstr(atBuffer, "OK") != NULL) {
        printf("AT+CWJAP command OK!\r\n");
        return 0;
    } else {
        printf("AT+CWJAP command FAILED!\r\n");
        return -1;
    }
    return 0;
}

void ESP8266_GetWIFIStatus()
{
    uint16_t rxLen = 0;
    char buffer[32] = {0};

    strncpy(buffer, "AT+CIPSTATUS\r\n", sizeof(buffer));
    HAL_UART_Transmit(&huart3, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);
    memset(atBuffer, 0x00, sizeof(atBuffer));
    HAL_UARTEx_ReceiveToIdle(&huart3, (uint8_t *)atBuffer, sizeof(atBuffer), &rxLen, HAL_MAX_DELAY);

    printf("CIPSTATUS len: [%d], buf: [%s]\r\n", rxLen, atBuffer);
}

void ESP8266_GetIPStatus()
{
    uint16_t rxLen = 0;
    char buffer[32] = {0};

    strncpy(buffer, "AT+CIFSR\r\n", sizeof(buffer));
    HAL_UART_Transmit(&huart3, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);
    memset(atBuffer, 0x00, sizeof(atBuffer));
    // 这种Transmit了要马上接收, 不然消息早就已经没有了, 再idle收会卡住
    HAL_UARTEx_ReceiveToIdle(&huart3, (uint8_t *)atBuffer, sizeof(atBuffer), &rxLen, HAL_MAX_DELAY);

    printf("CIFSR len: [%d], buf: [%s]\r\n", rxLen, atBuffer);
}
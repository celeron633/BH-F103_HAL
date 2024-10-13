#include "esp8266.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// status
// TODO: add string check for AT return
int wifiConnected   = 0;
int tcpEstablished  = 0;

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

static int peekAddr(const char* str, char *out)
{
	char *ptr = strstr(str, "STAIP,");
	int j = 0;
	if (ptr != NULL) {
		// 跳过STAIP,
		ptr += 6;
		while (*ptr)
		{
			if (*ptr != '\"' && *ptr != '\r') {
				out[j++] = *ptr;
			}
			if (*ptr == '\r') {
				break;
			}
			ptr++;
		}
		out[j] = '\0';
	} else {
        return -1;
    }
    return 0;
}

static int checkTcpStatus(const char *buf)
{
    char buf2[4] = {0};
    char *ptr = strstr(buf, "STATUS:");
    if (ptr == NULL) {
        return -1;
    } else {
        int j = 0;
        ptr += 7;
        while (*ptr != '\r' && *ptr != '\0')
        {
            buf2[j++] = *ptr;
            ptr++;
        }
        buf2[j] = '\0';
        int tcpStat = atoi(buf2);
        printf("tcpStat: %d\r\n", tcpStat);
        if (tcpStat == 3) {
            return 0;
        } else {
            return -1;
        }
    }
    return -1;
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

uint16_t ESP8266_SendATCommand(const char *command, int needDelay)
{
    char buffer[64] = {0};
    uint16_t rxLength = 0;

    strncpy(buffer, command, sizeof(buffer));
    HAL_UART_Transmit(&huart3, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);
    memset(atBuffer, 0x00, sizeof(atBuffer));
    if (needDelay) {
        HAL_Delay(100);
    }
    HAL_UARTEx_ReceiveToIdle(&huart3, (uint8_t *)atBuffer, sizeof(atBuffer), &rxLength, HAL_MAX_DELAY);

    return rxLength;
}

int ESP8266_CheckAT()
{
    uint16_t rxLen = 0;

    rxLen = ESP8266_SendATCommand("AT\r\n", 0);
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

    rxLen = ESP8266_SendATCommand("AT+CWMODE=1\r\n", 0);
    printf("AT+CWMODE len: [%d], buf: [%s]\r\n", rxLen, atBuffer);
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

    rxLen = ESP8266_SendATCommand("AT+CWLAP\r\n", 1);
    printf("AT+CWLAP len: [%d], buf: [%s]\r\n", rxLen, atBuffer);
}

int ESP8266_ConnectWIFI()
{
    uint16_t rxLen = 0;

    rxLen = ESP8266_SendATCommand("AT+CWJAP=\"TestAP\",\"abcd1234\"\r\n", 1);
    printf("AT+CWJAP len: [%d], buf: [%s]\r\n", rxLen, atBuffer);
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

    rxLen = ESP8266_SendATCommand("AT+CIPSTATUS\r\n", 0);
    printf("AT+CIPSTATUS len: [%d], buf: [%s]\r\n", rxLen, atBuffer);
}

int ESP8266_GetIPStatus()
{
    uint16_t rxLen = 0;

    char ipAddr[24] = {0};
    rxLen = ESP8266_SendATCommand("AT+CIFSR\r\n", 0);
    printf("AT+CIFSR len: [%d], buf: [%s]\r\n", rxLen, atBuffer);

    if (peekAddr(atBuffer, ipAddr) >= 0) {
        printf("IP ADDRESS: [%s]\r\n", ipAddr);
        return 0;
    } else {
        printf("Get IP address FAILED!\r\n");
        return -1;
    }

    return -1;
}

int ESP8266_TCPConnect(const char *ip, uint16_t port)
{
    uint16_t rxLen = 0;

    char atCommand[64] = {0};
    snprintf(atCommand, sizeof(atCommand), "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", ip, port);
    printf("AT cmd: [%s]\r\n", atCommand);

    rxLen = ESP8266_SendATCommand(atCommand, 0);
    printf("AT+CIPSTART len: [%d], buf: [%s]\r\n", rxLen, atBuffer);

    return 0;
}

int ESP8266_CheckTCPStatus(void)
{
    uint16_t rxLen = 0;
    rxLen = ESP8266_SendATCommand("AT+CIPSTATUS\r\n", 0);
    printf("AT+CIPSTATUS len: [%d], buf: [%s]\r\n", rxLen, atBuffer);
    if (checkTcpStatus(atBuffer) < 0) {
        printf("tcp not established!\r\n");
        return -1;
    } else {
        printf("tcp establisehd!\r\n");
    }
    return 0;
}
#ifndef __ESP8266_H__
#define __ESP8266_H__

#include "main.h"

uint16_t ESP8266_SendATCommand(const char *command, int needDelay);

void ESP8266_Reset();
void ESP8266_Init();
int ESP8266_CheckAT();
int ESP8266_EnterStationMode();
void ESP8266_ScanWIFI();
int ESP8266_ConnectWIFI();
void ESP8266_GetWIFIStatus();
int ESP8266_GetIPStatus();
int ESP8266_TCPConnect(const char *ip, uint16_t port);
int ESP8266_CheckTCPStatus(void);

#endif
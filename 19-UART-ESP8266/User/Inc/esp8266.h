#ifndef __ESP8266_H__
#define __ESP8266_H__

#include "main.h"

void ESP8266_Reset();
void ESP8266_Init();
int ESP8266_CheckAT();
int ESP8266_EnterStationMode();
void ESP8266_ScanWIFI();
int ESP8266_ConnectWIFI();
void ESP8266_GetWIFIStatus();
void ESP8266_GetIPStatus();


#endif
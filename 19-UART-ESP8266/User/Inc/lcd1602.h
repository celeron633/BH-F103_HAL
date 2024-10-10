#ifndef __LCD1602_H__
#define __LCD1602_H__

#include <stdio.h>
#include <stdint.h>

void LCD_SendCmd(uint8_t cmd);
void LCD_SendData(uint8_t data);

void LCD_Init();
void LCD_Clear();
void LCD_SetCursor(int row, int col);
void LCD_ShowString(const char *str);

#endif
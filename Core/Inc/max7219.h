#ifndef __MAX7219_H__
#define __MAX7219_H__

#include "main.h"

#include <stdint.h>

extern SPI_HandleTypeDef hspi1;

#define MAX7219_DISPLAY_NUM 8

// 寄存器枚举
typedef enum {
	REG_NO_OP 			= 0x00,
	REG_DIGIT_0 		= 0x01,
	REG_DIGIT_1 		= 0x02,
	REG_DIGIT_2 		= 0x03,
	REG_DIGIT_3 		= 0x04,
	REG_DIGIT_4 		= 0x05,
	REG_DIGIT_5 		= 0x06,
	REG_DIGIT_6 		= 0x07,
	REG_DIGIT_7 		= 0x08,
	REG_DECODE_MODE 	= 0x09,
	REG_INTENSITY 		= 0x0A,
	REG_SCAN_LIMIT 		= 0x0B,
	REG_SHUTDOWN 		= 0x0C,
	REG_DISPLAY_TEST 	= 0x0F,
} MAX7219_REGISTERS;

void max7219Init(uint8_t brightness);

// 开启
void max7219TurnOn();
// 关闭
void max7219TurnOff();

// 打开BCD解码
void max7219DecodeOn();
// 关闭BCD解码
void max7219DecodeOff();

// 写数据
void max7219Write(uint8_t regAddr, uint8_t regData);

// 清屏
void max7219Clear(void);

void led_printf(const char *fmt, ...);

int isLedPrintable(char ch);

#endif
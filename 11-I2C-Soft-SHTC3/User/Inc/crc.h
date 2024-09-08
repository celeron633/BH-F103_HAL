#ifndef __CRC_H__
#define __CRC_H__

#include <stdint.h>
#include <stdio.h>

// CRC-8 参数
#define CRC8_POLY 0x31   // 生成多项式
#define CRC8_INIT 0xFF   // 初始化值
#define CRC8_XOR_OUT 0x00 // 最终异或值

uint8_t crc8(const uint8_t* data, size_t length);

#endif
#include "crc.h"

uint8_t crc8(const uint8_t* data, size_t length) {
    uint8_t crc = CRC8_INIT;
    for (size_t i = 0; i < length; ++i) {
        crc ^= data[i];
        for (int j = 0; j < 8; ++j) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ CRC8_POLY;
            }
            else {
                crc <<= 1;
            }
        }
    }
    return crc ^ CRC8_XOR_OUT;
}
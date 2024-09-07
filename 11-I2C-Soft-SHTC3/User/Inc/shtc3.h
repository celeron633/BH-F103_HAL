#ifndef __SHTC3_H__
#define __SHTC3_H__

#include "my_i2c.h"

#define SHTC3_ADDR      (0x70)
#define SHTC3_W_ADDR    (SHTC3_ADDR << 1)
#define SHTC3_R_ADDR    ((SHTC3_ADDR << 1) | 0x01)

int SHTC3_Init(void);

int SHTC3_WriteCmd(uint16_t cmd);

int SHTC3_Wakeup(void);
int SHTC3_Sleep(void);

int SHTC3_Measure(double *temperature, double *humidity);

uint8_t SHTC3_GetID(void);

#endif
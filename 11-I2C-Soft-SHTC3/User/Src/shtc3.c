#include "shtc3.h"

#include <stdio.h>

int SHTC3_Init(void)
{
    I2C_Init();

    return 0;
}

int SHTC3_WriteCmd(uint16_t cmd)
{
    uint8_t cmd1 = (cmd >> 8)       & 0xFF;
    uint8_t cmd2 = ((uint8_t)cmd)   & 0xFF;
#if 0
    printf("cmd: [0x%X]\r\n", cmd);
#endif

    I2C_Start();
    I2C_WriteByte(SHTC3_W_ADDR);
    if (I2C_ReadAck() == 1) {
        printf("WriteByte wait ACK failed while write address\r\n");
        I2C_Stop();
        return -1;
    }

    I2C_WriteByte(cmd1);
    if (I2C_ReadAck() == 1) {
        printf("WriteByte wait ACK failed while write cmd1\r\n");
        I2C_Stop();
        return -1;
    }
    I2C_WriteByte(cmd2);
    if (I2C_ReadAck() == 1) {
        printf("WriteByte wait ACK failed while write cmd2\r\n");
        I2C_Stop();
        return -1;
    }
    I2C_Stop();

    return 0;
}

int SHTC3_Wakeup(void)
{
    // wakeup cmd: 0x3517
    if (SHTC3_WriteCmd(0x3517) < 0) {
        printf("wakeup sensor failed!\r\n");
        return -1;
    }
    
    printf("shtc3 wake up success!\r\n");
    // give sensor some time to wakeup
    HAL_Delay(10);
    return 0;
}

int SHTC3_Sleep(void)
{
     // sleep cmd: 0xB098
    if (SHTC3_WriteCmd(0xB098) < 0) {
        printf("sleep failed!\r\n");
        return -1;
    }
    
    printf("shtc3 sleep success!\r\n");
    return 0;
}

uint8_t SHTC3_GetID(void)
{
    SHTC3_WriteCmd(0xEFC8);
    uint8_t idInfo[2];

    I2C_Start();
    I2C_WriteByte(SHTC3_R_ADDR);
    if (I2C_ReadAck() == 1) {
        printf("ReadACK failed\r\n");
        I2C_Stop();
        return -1;
    } else {
        idInfo[0] = I2C_ReadByte();
        I2C_WriteAck();
        idInfo[1] = I2C_ReadByte();
        I2C_WriteAck();
    }
    I2C_Stop();

    printf("id[0]: [%02x], id[1]: [%02x]\r\n", idInfo[0], idInfo[1]);
    return 0;
}

int SHTC3_Measure(double *temperature, double *humidity)
{
    // 1. send measure command to sensor
    // 0x7866: read T first, Normal Mode, Clock Stretching Disabled
    SHTC3_WriteCmd(0x7866);

    // 2. give sensor sometime
    HAL_Delay(1);

    // 3. polling for finish
    do {
        printf("polling...\r\n");
        I2C_Start();
        I2C_WriteByte(SHTC3_R_ADDR);
        uint8_t status = I2C_ReadAck();
        if (status == 0) {
            printf("sensor measure done!\r\n");
            break;
        } else if (status == 1) {
            printf("sensor is still measuring!\r\n");
            I2C_Stop();
        }
    } while (1);
    // 4. get data
    uint8_t shtc3Result[6] = {0};
    for (int i = 0; i < 6; i++) {
        shtc3Result[i] = I2C_ReadByte();
        I2C_WriteAck();
        // I2C_WriteAck();
    }
    I2C_Stop();

    // 5. process data
    printf("data:\r\n");
    for (int i = 0; i < 6; i++) {
        printf("0x%02X ", shtc3Result[i]);
    }
    printf("\r\n");
    *temperature = 0;
    *humidity = 0;

    // 6. let sensor go sleep
    SHTC3_Sleep();

    return 0;
}
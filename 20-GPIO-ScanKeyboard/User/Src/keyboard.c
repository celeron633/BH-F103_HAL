#include "keyboard.h"
#include "stm32f103xe.h"

#include <stdint.h>


struct KBD_Pin {
    GPIO_TypeDef *gpioPort;
    uint16_t gpioPin;
};
typedef struct KBD_Pin KBD_Pin_t;

static int kbdStatusArray[4][3] = {0};
static KBD_Pin_t KBD_rows[] = {{KBD_R1_GPIO_Port, KBD_R1_Pin}, {KBD_R2_GPIO_Port, KBD_R2_Pin}, {KBD_R3_GPIO_Port, KBD_R3_Pin}, \
    {KBD_R4_GPIO_Port, KBD_R4_Pin}};
static KBD_Pin_t KBD_cols[] = {{KBD_C1_GPIO_Port, KBD_C1_Pin}, {KBD_C2_GPIO_Port, KBD_C2_Pin}, {KBD_C3_GPIO_Port, KBD_C3_Pin}};


void KBD_Init()
{
    // 所有行先初始化为高电平
    for (int i = 0; i < KBD_ROWS; i++) {
        HAL_GPIO_WritePin(KBD_rows[i].gpioPort, KBD_rows[i].gpioPin, GPIO_PIN_SET);
    }
}

int KBD_Scan()
{
    // 扫描每一行
    for (int i = 0; i < KBD_ROWS; i++) {
        // 拉低
        HAL_GPIO_WritePin(KBD_rows[i].gpioPort, KBD_rows[i].gpioPin, GPIO_PIN_RESET);
        HAL_Delay(5);
        // 扫描每一列
        for (int j = 0; j < KBD_COLS; j++) {
            if (HAL_GPIO_ReadPin(KBD_cols[j].gpioPort, KBD_cols[j].gpioPin) == GPIO_PIN_RESET) {
                kbdStatusArray[i][j] = 1;
            } else {
                kbdStatusArray[i][j] = 0;
            }
        }
        HAL_GPIO_WritePin(KBD_rows[i].gpioPort, KBD_rows[i].gpioPin, GPIO_PIN_SET);
    }

    for (int i = 0; i < KBD_ROWS; i++) {
        for (int j = 0; j < KBD_COLS; j++) {
            if (kbdStatusArray[i][j] == 1) {
                int tmp = i*3+(j+1);
                if (tmp == 11) {
                    return 0;
                } else {
                    return tmp;
                }
            }
        }
    }
    // 没有键按下
    return -1;
}
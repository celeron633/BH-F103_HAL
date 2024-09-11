#include "dht11.h"
#include "delay.h"

#include <stdio.h>

void DHT11_Init(void)
{
    // 至少等待2s, 传感器完成初始化
    printf("waiting 2s before dht11 is fully initialized..\r\n");
    delay_ms(2000);
    printf("wait done\r\n");

    DHT11_CLK_ENABLE();

    GPIO_InitTypeDef dht11Gpio;
    dht11Gpio.Mode = GPIO_MODE_OUTPUT_OD;
    dht11Gpio.Pin = DHT11_DIO_PIN;
    dht11Gpio.Pull = GPIO_NOPULL;
    dht11Gpio.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(DHT11_GPIO, &dht11Gpio);
    // 默认释放控制
    DIO_HIGH();
}

int DHT11_Start(void)
{
    DIO_LOW();
    delay_ms(20);
    DIO_HIGH();

    int timeout = 0;

    // 等待从机拉低DIO
    // printf("waiting dht11 pull low...\r\n");
    while (DIO_READ() == GPIO_PIN_SET) {
        delay_us(1);
        timeout++;
        if (timeout > 100) {
            return -1;
        }
    }
    timeout = 0;
    // printf("waiting dht11 pull high again...\r\n");
    while (DIO_READ() == GPIO_PIN_RESET) {
        delay_us(1);
        timeout++;
        if (timeout > 200) {
            return -1;
        }
    }
    // printf("start outputing..\r\n");

    return 0;
}

int DHT11_ReadData(uint8_t *data)
{

}
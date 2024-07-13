#include "dht11.h"
#include "delay.h"

extern TIM_HandleTypeDef htim6;
int retryCount1 = 0;
int retryCount2 = 0;

static inline void dht11DataIn()
{
    GPIOA->CRL &= 0xFF0FFFFF;
    GPIOA->CRL |= (0x04 << 20);
}

static inline void dht11DataOut()
{
    GPIOA->CRL &= 0xFF0FFFFF;
    GPIOA->CRL |= (0x07 << 20);
}

void dht11Init()
{
    // 初始化GPIO
    GPIO_InitTypeDef dht11GPIO;
    dht11GPIO.Mode  = GPIO_MODE_OUTPUT_OD;
    dht11GPIO.Pin   = DHT11_DATA_PORT;
    dht11GPIO.Pull  = GPIO_NOPULL;
    dht11GPIO.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(DHT11_DATA_GPIO, &dht11GPIO);

    // 启动定时器
    __HAL_TIM_ENABLE(&htim6);
}

int dht11Reset()
{
    // 切换到输出模式
    dht11DataOut();
    // 先拉低18毫秒
    HAL_GPIO_WritePin(DHT11_DATA_GPIO, DHT11_DATA_PORT, GPIO_PIN_RESET);
    HAL_Delay(18);
    // 再拉高30us, 等待DHT11响应
    HAL_GPIO_WritePin(DHT11_DATA_GPIO, DHT11_DATA_PORT, GPIO_PIN_SET);
    tim_delay_us(30);
    // 切换到输入模式
    dht11DataIn();
    // 先会被DHT11拉低80us左右, 再会被DHT11拉高80us
    int retryCnt = 0;
    while (HAL_GPIO_ReadPin(DHT11_DATA_GPIO, DHT11_DATA_PORT) == GPIO_PIN_SET) {
        retryCnt++;
        tim_delay_us(10);
        // 超时了
        if (retryCnt >= 120) {
            return -1;
        }
    }
    retryCount1 = retryCnt;
    retryCnt = 0;
    while (HAL_GPIO_ReadPin(DHT11_DATA_GPIO, DHT11_DATA_PORT) == GPIO_PIN_RESET) {
        retryCnt++;
        tim_delay_us(10);
        // 超时了
        if (retryCnt >= 120) {
            return -1;
        }
    }
    retryCount2 = retryCnt;
    // 后面DHT11开始返回温度数据了

    return 0;
}
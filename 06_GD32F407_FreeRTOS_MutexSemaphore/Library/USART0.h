#ifndef __USART0_H__
#define __USART0_H__

#include "gd32f4xx.h"

// --------------------------------- USART ---------------------------
// 接收开关配置 1打开 0关闭
#define USART0_RECV_CALLBACK    1

// 发送引脚 PA9,PA15,PB6
#define USART0_TX_RCU    RCU_GPIOA
#define USART0_TX_PORT   GPIOA
#define USART0_TX_PIN    GPIO_PIN_9

// 接收引脚 PA10,PB3,PB7
#define USART0_RX_RCU    RCU_GPIOA
#define USART0_RX_PORT   GPIOA
#define USART0_RX_PIN    GPIO_PIN_10

// 波特率
#define USART0_BAUDRATE  115200

// --------------------------------- DMA ---------------------------
// DMA发送功能开关 1开启 0关闭
#define USART0_DMA_TX_ENABLE          1
// DMA接收功能开关 1开启 0关闭
#define USART0_DMA_RX_ENABLE          1

#define USART0_TX_DMA_RCU             RCU_DMA1
#define USART0_TX_DMA_PERIPH_CH       DMA1, DMA_CH7
#define USART0_TX_DMA_SUBPERI         DMA_SUBPERI4

#define USART0_RX_DMA_RCU             RCU_DMA1
#define USART0_RX_DMA_PERIPH_CH       DMA1, DMA_CH2
#define USART0_RX_DMA_SUBPERI         DMA_SUBPERI4

void USART0_init();

// 发送1个字节数据
void USART0_send_data(uint8_t data);

// 发送多个字节(字节数字)
void USART0_send_data_array(uint8_t* arr, uint32_t len);

// 发送字符串 \0
void USART0_send_string(char* str);


#endif
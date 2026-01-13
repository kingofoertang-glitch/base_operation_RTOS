#ifndef __BSP_DOT_MATRIX_H__
#define __BSP_DOT_MATRIX_H__

#include "gd32f4xx.h"


#define DM_CLK_RCU		RCU_GPIOA
#define DM_CLK_PORT		GPIOA
#define DM_CLK_PIN		GPIO_PIN_5

#define DM_DIO_RCU		RCU_GPIOA
#define DM_DIO_PORT		GPIOA
#define DM_DIO_PIN		GPIO_PIN_7

#define DM_CS_RCU		  RCU_GPIOA
#define DM_CS_PORT		GPIOA
#define DM_CS_PIN		  GPIO_PIN_6

#define DM_CLK_L()  	gpio_bit_write(DM_CLK_PORT, DM_CLK_PIN, RESET)
#define DM_CLK_H()  	gpio_bit_write(DM_CLK_PORT, DM_CLK_PIN, SET)

#define DM_DIO_L()  	gpio_bit_write(DM_DIO_PORT, DM_DIO_PIN, RESET)
#define DM_DIO_H()  	gpio_bit_write(DM_DIO_PORT, DM_DIO_PIN, SET)

#define DM_CS_L()  		gpio_bit_write(DM_CS_PORT, DM_CS_PIN, RESET)
#define DM_CS_H()  		gpio_bit_write(DM_CS_PORT, DM_CS_PIN, SET)


void DM_init();

void DM_display(uint8_t* dat);

// 取出4字节的1号字节做显示
void DM_display2(uint32_t* dat);

#endif
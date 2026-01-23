#ifndef __BSP_EC12_H__
#define __BSP_EC12_H__

#include "gd32f4xx.h"


// CW: À≥ ±’Î clockwise  
//CCW: ƒÊ ±’Î counter clockwise

#define EC12_A_TRIG_HANDLE  EXTI11_on_trig
#define EC12_B_TRIG_HANDLE  EXTI13_on_trig
#define EC12_D_TRIG_HANDLE  EXTI15_on_trig

#define EC12_A_STATE()  gpio_input_bit_get(GPIOD, GPIO_PIN_11)
#define EC12_B_STATE()  gpio_input_bit_get(GPIOD, GPIO_PIN_13)
#define EC12_D_STATE()  gpio_input_bit_get(GPIOD, GPIO_PIN_15)

void bsp_ec12_init();

void bsp_ec12_loop();

#endif
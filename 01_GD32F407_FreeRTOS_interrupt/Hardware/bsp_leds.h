#ifndef __BSP_LEDS_H__
#define __BSP_LEDS_H__

#include "gd32f4xx.h"


void bsp_leds_init(void);

// 点亮指定位置的灯
void bsp_leds_turn_on(uint8_t led_index);

// 熄灭指定位置的灯
void bsp_leds_turn_off(uint8_t led_index);

#endif
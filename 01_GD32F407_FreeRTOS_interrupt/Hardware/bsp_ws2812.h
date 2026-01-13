#ifndef __BSP_WS2812_H__
#define __BSP_WS2812_H__

#include "gd32f4xx.h"

#ifndef u8
#define u8 uint8_t
#endif

#ifndef u16
#define u16 uint16_t
#endif

#ifndef u32
#define u32 uint32_t
#endif


#define WS2812_NUM    4
#define SPI_BYTE_LEN  30

void bsp_ws2812_init();

// 8bit(±£¡Ù) + 8bit(RED) + 8bit(Green) + 8bit(Blue)
void bsp_ws2812_set_color(uint16_t index, uint32_t color);

void bsp_ws2812_display();

void bsp_ws2812_HSVtoRGB(double h, double s, double v, u8* r, u8* g, u8* b); 
void bsp_ws2812_HSVtoRGB24(double h, double s, double v, u32* rgb);

#endif
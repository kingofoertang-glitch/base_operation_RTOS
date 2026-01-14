#ifndef __BSP_KEYS_H__
#define __BSP_KEYS_H__

#include "gd32f4xx.h"

// 初始化按键
void bsp_keys_init(void);

// 扫描按键
uint8_t bsp_keys_scan(void);

#endif

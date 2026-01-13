#ifndef __BSP_ADKEY_H__
#define __BSP_ADKEY_H__

#include "gd32f4xx.h"

void bsp_adkey_init();

// 按键扫描函数, 需要周期性的调用
void bsp_adkey_scan();

#endif
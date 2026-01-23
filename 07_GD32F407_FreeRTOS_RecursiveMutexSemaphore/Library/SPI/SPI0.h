#ifndef __SPI0_H__
#define __SPI0_H__

#include "gd32f4xx.h"
#include "SPI_config.h"

#if USE_SPI0

void SPI0_init();

void SPI0_write(uint8_t dat);

uint8_t SPI0_read();

uint8_t SPI0_read_write(uint8_t dat);

#endif

#endif
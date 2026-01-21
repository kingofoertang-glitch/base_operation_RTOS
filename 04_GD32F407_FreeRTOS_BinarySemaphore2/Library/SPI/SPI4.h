#ifndef __SPI4_H__
#define __SPI4_H__

#include "gd32f4xx.h"
#include "SPI_config.h"

#if USE_SPI4

void SPI4_init();

void SPI4_write(uint8_t dat);

uint8_t SPI4_read();

uint8_t SPI4_read_write(uint8_t dat);

#endif

#endif
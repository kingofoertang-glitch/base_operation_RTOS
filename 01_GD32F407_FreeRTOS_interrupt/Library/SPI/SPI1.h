#ifndef __SPI1_H__
#define __SPI1_H__

#include "SPI_config.h"

#if USE_SPI_1

void SPI1_init();

#if SPI1_MOSI_ENABLE
void SPI1_write(uint8_t data);
#endif

#if SPI1_MISO_ENABLE
uint8_t SPI1_read();
#endif

#if SPI1_MOSI_ENABLE && SPI1_MISO_ENABLE
uint8_t SPI1_write_read(uint8_t dat);
#endif


#if SPI1_DMA_TX_ENABLE
void SPI1_dma_tx_start(uint8_t* data, uint32_t len, uint8_t src_increment);
void SPI1_dma_tx_stop();
#if SPI1_DMA_TX_CALLBACK
extern void SPI1_dma_tx_on_finish();
#endif
#endif

#endif
#endif
#ifndef __SPI_H__
#define __SPI_H__

#include "SPI_config.h"

#if USE_SPI_0
#include "SPI0.h"
#endif

#if USE_SPI_1
#include "SPI1.h"
#endif

#if USE_SPI_2
#include "SPI2.h"
#endif

#if USE_SPI_3
#include "SPI3.h"
#endif

#if USE_SPI_4
#include "SPI4.h"
#endif

#if USE_SPI_5
#include "SPI5.h"
#endif


void SPI_init();


#endif
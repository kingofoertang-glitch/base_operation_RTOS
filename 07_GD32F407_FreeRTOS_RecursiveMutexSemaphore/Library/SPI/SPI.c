#include "SPI.h"


void SPI_init() {

#if USE_SPI_0
	SPI0_init();
#endif

#if USE_SPI_1
	SPI1_init();
#endif
	
#if USE_SPI_2
	SPI2_init();
#endif
	
#if USE_SPI_3
	SPI3_init();
#endif
	
#if USE_SPI_4
	SPI4_init();
#endif
	
#if USE_SPI_5
	SPI5_init();
#endif
	
}
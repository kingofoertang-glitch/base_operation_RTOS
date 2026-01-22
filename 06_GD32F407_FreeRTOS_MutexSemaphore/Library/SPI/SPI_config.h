#ifndef __SPI_CONFIG_H__
#define __SPI_CONFIG_H__

#include "gd32f4xx.h"
#include "systick.h"

#define USE_SPI_0		0
#define USE_SPI_1		1
#define USE_SPI_2		0
#define USE_SPI_3		0
#define USE_SPI_4		0
#define USE_SPI_5		0


/*********************************************/
/******************* SPI0 ********************/
/*********************************************/
#if USE_SPI_0
#define SPI0_SOFT			    0
#define SPI0_MOSI_ENABLE	1
#define SPI0_MISO_ENABLE	0

// PA5(AF5), PB3(AF5)
#define SPI0_CLK_PORT_RCU	  RCU_GPIOA
#define SPI0_CLK_PORT		    GPIOA
#define SPI0_CLK_PIN		    GPIO_PIN_5

#if SPI0_MOSI_ENABLE
// PA7(AF5), PB5(AF5)
#define SPI0_MOSI_PORT_RCU	RCU_GPIOA
#define SPI0_MOSI_PORT		  GPIOA
#define SPI0_MOSI_PIN		    GPIO_PIN_7
#endif

#if SPI0_MISO_ENABLE
// PA6(AF5), PB4(AF5)
#define SPI0_MISO_PORT_RCU	RCU_GPIOB
#define SPI0_MISO_PORT		  GPIOB
#define SPI0_MISO_PIN		    GPIO_PIN_4
#endif

//预分频：SPI_PSC_x x为(2, 4, 8, 16, 32, 64, 128, 256)
#define SPI0_PARAM_PSC		SPI_PSC_4
//极性和相位：
//	SPI_CK_PL_LOW_PH_1EDGE：	0，0
//	SPI_CK_PL_HIGH_PH_1EDGE：	1，0
//	SPI_CK_PL_LOW_PH_2EDGE：  	0，1
//	SPI_CK_PL_HIGH_PH_2EDGE：	1，1
#define SPI0_PARAM_CPOL_CPHA	SPI_CK_PL_LOW_PH_1EDGE
// 大小端：
//	SPI_ENDIAN_MSB： 大端
//	SPI_ENDIAN_LSB： 小端
#define SPI0_PARAM_ENDIAN		SPI_ENDIAN_MSB

#if SPI0_SOFT == 0
#define SPI0_DMA_ENABLE     1
#endif

#endif

/*********************************************/
/******************* SPI1 ********************/
/*********************************************/
#if USE_SPI_1
#define SPI1_SOFT			    0
#define SPI1_MOSI_ENABLE	1
#define SPI1_MISO_ENABLE	0

// PA9(AF5),PB10(AF5),PB13(AF5),PC7(AF5),PD3(AF5),PI1(AF5)
#define SPI1_CLK_PORT_RCU	  RCU_GPIOB
#define SPI1_CLK_PORT		    GPIOB
#define SPI1_CLK_PIN		    GPIO_PIN_10

#if SPI1_MOSI_ENABLE
// PB15(AF5),PC1(AF5),PC3(AF5),PI3(AF5)
#define SPI1_MOSI_PORT_RCU	RCU_GPIOB
#define SPI1_MOSI_PORT		  GPIOB
#define SPI1_MOSI_PIN		    GPIO_PIN_15
#endif

#if SPI1_MISO_ENABLE
// PB14(AF5),PC2(AF5),PI2(AF5)
#define SPI1_MISO_PORT_RCU	RCU_GPIOB
#define SPI1_MISO_PORT		GPIOB
#define SPI1_MISO_PIN		GPIO_PIN_14
#endif

//预分频：SPI_PSC_x x为(2, 4, 8, 16, 32, 64, 128, 256)
#define SPI1_PARAM_PSC		SPI_PSC_4
//极性和相位：
//	SPI_CK_PL_LOW_PH_1EDGE：	0，0
//	SPI_CK_PL_HIGH_PH_1EDGE：	1，0
//	SPI_CK_PL_LOW_PH_2EDGE：  0，1
//	SPI_CK_PL_HIGH_PH_2EDGE：	1，1
#define SPI1_PARAM_CPOL_CPHA	SPI_CK_PL_HIGH_PH_2EDGE
// 大小端：
//	SPI_ENDIAN_MSB： 大端
//	SPI_ENDIAN_LSB： 小端
#define SPI1_PARAM_ENDIAN		SPI_ENDIAN_MSB

///// DMA
#if SPI1_SOFT == 0

  #define SPI1_DMA_TX_ENABLE     1
  #define SPI1_DMA_RX_ENABLE     1

  #if SPI1_DMA_TX_ENABLE
    #define SPI1_DMA_TX_CALLBACK    0
    #define SPI1_DMA_TX_NVIC     1, 1
  #endif // SPI1_DMA_TX_ENABLE

#endif // SPI1_SOFT

#endif

/*********************************************/
/******************* SPI2 ********************/
/*********************************************/
#if USE_SPI_2
#define SPI2_SOFT			1
#define SPI2_MOSI_ENABLE	1
#define SPI2_MISO_ENABLE	1

// PB3(AF6),PC10(AF6)
#define SPI2_CLK_PORT_RCU	RCU_GPIOC
#define SPI2_CLK_PORT		GPIOC
#define SPI2_CLK_PIN		GPIO_PIN_10

#if SPI2_MOSI_ENABLE
// PB0(AF7),PB2(AF7),PB5(AF6),PC1(AF5),PC12(AF6),PD0(AF6),PD6(AF5)
#define SPI2_MOSI_PORT_RCU	RCU_GPIOC
#define SPI2_MOSI_PORT		GPIOC
#define SPI2_MOSI_PIN		GPIO_PIN_12
#endif

#if SPI2_MISO_ENABLE
// PB4(AF6),PC11(AF6)
#define SPI2_MISO_PORT_RCU	RCU_GPIOB
#define SPI2_MISO_PORT		GPIOB
#define SPI2_MISO_PIN		GPIO_PIN_4
#endif

//预分频：SPI_PSC_x x为(2, 4, 8, 16, 32, 64, 128, 256)
#define SPI2_PARAM_PSC		SPI_PSC_2
//极性和相位：
//	SPI_CK_PL_LOW_PH_1EDGE：	0，0
//	SPI_CK_PL_HIGH_PH_1EDGE：	1，0
//	SPI_CK_PL_LOW_PH_2EDGE：	0，1
//	SPI_CK_PL_HIGH_PH_2EDGE：	1，1
#define SPI2_PARAM_CPOL_CPHA	SPI_CK_PL_LOW_PH_1EDGE
// 大小端：
//	SPI_ENDIAN_MSB： 大端
//	SPI_ENDIAN_LSB： 小端
#define SPI2_PARAM_ENDIAN		SPI_ENDIAN_MSB

#endif

/*********************************************/
/******************* SPI3 ********************/
/*********************************************/
#if USE_SPI_3
#define SPI3_SOFT			1
#define SPI3_MOSI_ENABLE	1
#define SPI3_MISO_ENABLE	1

// PB13(AF6),PE2(AF5),PE12(AF5),PG11(AF6)
#define SPI3_CLK_PORT_RCU	RCU_GPIOC
#define SPI3_CLK_PORT		GPIOC
#define SPI3_CLK_PIN		GPIO_PIN_10

#if SPI3_MOSI_ENABLE
// PA1(AF5),PE6(AF5),PE14(AF5),PG13(AF6)
#define SPI3_MOSI_PORT_RCU	RCU_GPIOC
#define SPI3_MOSI_PORT		GPIOC
#define SPI3_MOSI_PIN		GPIO_PIN_12
#endif

#if SPI3_MISO_ENABLE
// PA11(AF6),PD0(AF5),PE5(AF5),PE13(AF5),PG12(AF6)
#define SPI3_MISO_PORT_RCU	RCU_GPIOB
#define SPI3_MISO_PORT		GPIOB
#define SPI3_MISO_PIN		GPIO_PIN_4
#endif

//预分频：SPI_PSC_x x为(2, 4, 8, 16, 32, 64, 128, 256)
#define SPI3_PARAM_PSC		SPI_PSC_2
//极性和相位：
//	SPI_CK_PL_LOW_PH_1EDGE：	0，0
//	SPI_CK_PL_HIGH_PH_1EDGE：	1，0
//	SPI_CK_PL_LOW_PH_2EDGE：  	0，1
//	SPI_CK_PL_HIGH_PH_2EDGE：	1，1
#define SPI3_PARAM_CPOL_CPHA	SPI_CK_PL_LOW_PH_1EDGE
// 大小端：
//	SPI_ENDIAN_MSB： 大端
//	SPI_ENDIAN_LSB： 小端
#define SPI3_PARAM_ENDIAN		SPI_ENDIAN_MSB

#endif


/*********************************************/
/******************* SPI4 ********************/
/*********************************************/
#if USE_SPI_4
#define SPI4_SOFT			1
#define SPI4_MOSI_ENABLE	1
#define SPI4_MISO_ENABLE	1

// PB0(AF6),PE12(AF6),PF7(AF5),PH6(AF5)
#define SPI4_CLK_PORT_RCU	RCU_GPIOC
#define SPI4_CLK_PORT		GPIOC
#define SPI4_CLK_PIN		GPIO_PIN_10

#if SPI4_MOSI_ENABLE
// PA10(AF6),PB8(AF6),PBE14(AF6),PF9(AF5),PF11(AF5)
#define SPI4_MOSI_PORT_RCU	RCU_GPIOC
#define SPI4_MOSI_PORT		GPIOC
#define SPI4_MOSI_PIN		GPIO_PIN_12
#endif

#if SPI4_MISO_ENABLE
// PA12(AF6),PE13(AF6),PF8(AF5),PH7(AF5)
#define SPI4_MISO_PORT_RCU	RCU_GPIOB
#define SPI4_MISO_PORT		GPIOB
#define SPI4_MISO_PIN		GPIO_PIN_4
#endif

//预分频：SPI_PSC_x x为(2, 4, 8, 16, 32, 64, 128, 256)
#define SPI4_PARAM_PSC		SPI_PSC_2
//极性和相位：
//	SPI_CK_PL_LOW_PH_1EDGE：	0，0
//	SPI_CK_PL_HIGH_PH_1EDGE：	1，0
//	SPI_CK_PL_LOW_PH_2EDGE：  0，1
//	SPI_CK_PL_HIGH_PH_2EDGE：	1，1
#define SPI4_PARAM_CPOL_CPHA	SPI_CK_PL_LOW_PH_1EDGE
// 大小端：
//	SPI_ENDIAN_MSB： 大端
//	SPI_ENDIAN_LSB： 小端
#define SPI4_PARAM_ENDIAN	SPI_ENDIAN_MSB

#endif


/*********************************************/
/******************* SPI5 ********************/
/*********************************************/
#if USE_SPI_5
#define SPI5_SOFT			1
#define SPI5_MOSI_ENABLE	1
#define SPI5_MISO_ENABLE	1

// PG13(AF5)
#define SPI5_CLK_PORT_RCU		RCU_GPIOC
#define SPI5_CLK_PORT			GPIOC
#define SPI5_CLK_PIN			GPIO_PIN_10
#define SPI5_CLK_AF				GPIO_AF_6
#if SPI5_MOSI_ENABLE
// PG14(AF5)
#define SPI5_MOSI_PORT_RCU		RCU_GPIOC
#define SPI5_MOSI_PORT			GPIOC
#define SPI5_MOSI_PIN			GPIO_PIN_12
#define SPI5_MOSI_AF			GPIO_AF_6
#endif
#if SPI5_MISO_ENABLE
// PG12(AF5)
#define SPI5_MISO_PORT_RCU		RCU_GPIOB
#define SPI5_MISO_PORT			GPIOB
#define SPI5_MISO_PIN			GPIO_PIN_4
#define SPI5_MISO_AF			GPIO_AF_6
#endif
//预分频：SPI_PSC_x x为(2, 4, 8, 16, 32, 64, 128, 256)
#define SPI5_PARAM_PSC			SPI_PSC_2
//极性和相位：
//	SPI_CK_PL_LOW_PH_1EDGE：	0，0
//	SPI_CK_PL_HIGH_PH_1EDGE：	1，0
//	SPI_CK_PL_LOW_PH_2EDGE：  	0，1
//	SPI_CK_PL_HIGH_PH_2EDGE：	1，1
#define SPI5_PARAM_CPOL_CPHA	SPI_CK_PL_LOW_PH_1EDGE
// 大小端：
//	SPI_ENDIAN_MSB： 大端
//	SPI_ENDIAN_LSB： 小端
#define SPI5_PARAM_ENDIAN		SPI_ENDIAN_MSB

#endif


#endif
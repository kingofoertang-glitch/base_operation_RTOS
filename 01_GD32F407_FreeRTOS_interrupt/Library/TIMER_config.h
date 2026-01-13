#ifndef __TIMER_CONFIG_H__
#define __TIMER_CONFIG_H__

#include "gd32f4xx.h"

#define USE_TIMER_0     0 
#define USE_TIMER_1     0   
#define USE_TIMER_2     0   
#define USE_TIMER_3     0  
#define USE_TIMER_4     0   
#define USE_TIMER_5     0   
#define USE_TIMER_6     0   
#define USE_TIMER_7     0   
#define USE_TIMER_8     0   
#define USE_TIMER_9     0   
#define USE_TIMER_10    0   
#define USE_TIMER_11    0   
#define USE_TIMER_12    0   
#define USE_TIMER_13    0   

// ===================== TIMER0 =====================
#if USE_TIMER_0

#define TM0_PRESCALER     10U 
#define TM0_FREQ          1000U
#define TM0_PERIOD        (SystemCoreClock / TM0_PRESCALER / TM0_FREQ)

#define TM0_CH0           RCU_GPIOE, GPIOE, GPIO_PIN_9
#define TM0_CH0_ON        RCU_GPIOE, GPIOE, GPIO_PIN_8

#endif

// ===================== TIMER1 =====================
#if USE_TIMER_1

#define TM1_PRESCALER     10U 
#define TM1_FREQ          1000U
#define TM1_PERIOD        (SystemCoreClock / TM1_PRESCALER / TM1_FREQ)

#define TM1_CH1           RCU_GPIOB, GPIOB, GPIO_PIN_9

#endif

// ===================== TIMER3 =====================
#if USE_TIMER_3

#define TM3_PRESCALER     10U 
#define TM3_FREQ          1000U
#define TM3_PERIOD        (SystemCoreClock / TM3_PRESCALER / TM3_FREQ)

#define TM3_CH0           RCU_GPIOD, GPIOD, GPIO_PIN_12
#define TM3_CH1           RCU_GPIOD, GPIOD, GPIO_PIN_13
#define TM3_CH2           RCU_GPIOD, GPIOD, GPIO_PIN_14
#define TM3_CH3           RCU_GPIOD, GPIOD, GPIO_PIN_15

#endif


#endif
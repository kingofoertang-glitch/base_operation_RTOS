#ifndef __BSP_LED188_H__
#define __BSP_LED188_H__

#include "gd32f4xx.h"

#define LED188_RCU      RCU_GPIOE
#define LED188_1_PORT   GPIOE
#define LED188_1_PIN    GPIO_PIN_15
#define LED188_2_PORT   GPIOE
#define LED188_2_PIN    GPIO_PIN_13
#define LED188_3_PORT   GPIOE
#define LED188_3_PIN    GPIO_PIN_11
#define LED188_4_PORT   GPIOE
#define LED188_4_PIN    GPIO_PIN_9
#define LED188_5_PORT   GPIOE
#define LED188_5_PIN    GPIO_PIN_7

// 高阻输入模式
#define LED188_1_HZ() \
  gpio_mode_set(LED188_1_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, LED188_1_PIN)
#define LED188_2_HZ() \
  gpio_mode_set(LED188_2_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, LED188_2_PIN)
#define LED188_3_HZ() \
  gpio_mode_set(LED188_3_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, LED188_3_PIN)
#define LED188_4_HZ() \
  gpio_mode_set(LED188_4_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, LED188_4_PIN)
#define LED188_5_HZ() \
  gpio_mode_set(LED188_5_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, LED188_5_PIN)

// 推挽输出
#define LED188_1_PP() \
  gpio_mode_set(LED188_1_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED188_1_PIN)
#define LED188_2_PP() \
  gpio_mode_set(LED188_2_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED188_2_PIN)
#define LED188_3_PP() \
  gpio_mode_set(LED188_3_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED188_3_PIN)
#define LED188_4_PP() \
  gpio_mode_set(LED188_4_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED188_4_PIN)
#define LED188_5_PP() \
  gpio_mode_set(LED188_5_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED188_5_PIN)

#define ALL_IN() \
  LED188_1_HZ(); \
  LED188_2_HZ(); \
  LED188_3_HZ(); \
  LED188_4_HZ(); \
  LED188_5_HZ();
  

#define LED188_1(val) gpio_bit_write(LED188_1_PORT, LED188_1_PIN, val)
#define LED188_2(val) gpio_bit_write(LED188_2_PORT, LED188_2_PIN, val)
#define LED188_3(val) gpio_bit_write(LED188_3_PORT, LED188_3_PIN, val)
#define LED188_4(val) gpio_bit_write(LED188_4_PORT, LED188_4_PIN, val)
#define LED188_5(val) gpio_bit_write(LED188_5_PORT, LED188_5_PIN, val)

// 推挽模式, 输出高低电平
#define OUT1(val) \
  LED188_1_PP();  \
  LED188_1(val)
  
// 推挽模式, 输出高低电平  
#define OUT2(val) \
  LED188_2_PP();  \
  LED188_2(val)
  
// 推挽模式, 输出高低电平  
#define OUT3(val) \
  LED188_3_PP();  \
  LED188_3(val)
  
// 推挽模式, 输出高低电平  
#define OUT4(val) \
  LED188_4_PP();  \
  LED188_4(val)
  
// 推挽模式, 输出高低电平  
#define OUT5(val) \
  LED188_5_PP();  \
  LED188_5(val)
  
  
void LED188_init();

void LED188_set_num(uint8_t num); // [0, 199]
void LED188_clear(); // 关闭显示
void LED188_set_charge(uint8_t enable);
void LED188_set_percent(uint8_t enable);

void LED188_display();

#endif
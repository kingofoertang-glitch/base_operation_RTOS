#ifndef __TIMER_H__
#define __TIMER_H__

#include "gd32f4xx.h"
#include "TIMER_config.h"


/*
 * @brief 初始化定时器
*/
void TIMER_init();

/***
 * @brief 更新指定TIMER和CHANNEL的占空比 (比较值)
 * @param timer TIMERx  TIMERx(x=0,1,2,3,4...13)
 * @param ch TIMER_CHx  TIMER_CH_0, TIMER_CH_1, TIMER_CH_2, TIMER_CH_3
 * @param duty 占空比 [0, 100]
 */
void TIMER_channel_update(uint32_t timer_periph, uint16_t channel, float duty);

/***
 * @brief 更新指定TIMER和CHANNEL的周期 (自动重装值)
 * @param timer TIMERx  TIMERx(x=0,1,2,3,4...13)
 * @param prescaler 分频系数 [1, 65536]
 * @param period 周期值 [1, 65536]
 */
void TIMER_period_update(uint32_t timer_periph, uint16_t prescaler, uint32_t period);

/***
 * @brief 启用禁用指定通道
 */
void TIMER_channel_config(uint32_t timer_periph, uint32_t channel, uint8_t enable);

#endif
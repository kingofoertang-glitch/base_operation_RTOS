#ifndef __BUZZER_H__
#define __BUZZER_H__

#include "gd32f4xx.h"


#ifndef u8
#define u8   uint8_t
#define u16  uint16_t
#define u32  uint32_t
#endif


void Buzzer_init();

// 按照指定频率播放
void Buzzer_play(u16 hz_val);

// 指定音调音符播放 12345678
void Buzzer_beep(u16 idx);

void Buzzer_stop();

#endif
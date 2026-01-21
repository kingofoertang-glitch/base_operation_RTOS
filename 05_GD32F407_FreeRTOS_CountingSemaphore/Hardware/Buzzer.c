#include "Buzzer.h"
#include "TIMER.h"

//			 C	 D     E 	F	 G	  A	   B	 C`
// u16 hz[] = {523, 587, 659, 698, 784, 880, 988, 1047};
//			 C`	   D`     E`   F`	  G`	A`	  B`    C``
//u16 hz[] = {1047, 1175, 1319, 1397, 1568, 1760, 1976, 2093};
u16 FREQS[] = {
	523 * 1, 587 * 1, 659 * 1, 698 * 1, 784 * 1, 880 * 1, 988 * 1, 
	523 * 2, 587 * 2, 659 * 2, 698 * 2, 784 * 2, 880 * 2, 988 * 2, 
	523 * 4, 587 * 4, 659 * 4, 698 * 4, 784 * 4, 880 * 4, 988 * 4, 
	523 * 8, 587 * 8, 659 * 8, 698 * 8, 784 * 8, 880 * 8, 988 * 8, 
};
// TIMER1_CH1


// 初始化PB9 TIMER1_CH1
void Buzzer_init(){    
    
}

void Buzzer_play(u16 hz_val) {
    // 根据系统主频, 预分频系数, 计算周期计数值
    uint32_t period = SystemCoreClock / TM1_PRESCALER / hz_val;
    
    // 更新周期计数值 (根据输出频率)
    TIMER_period_update(TIMER1, TM1_PRESCALER, period);
    
    // 设置输出占空比
    TIMER_channel_update(TIMER1, TIMER_CH_1, 60.0f);
    
    // 启用 Timer0 CH0通道
    TIMER_channel_config(TIMER1, TIMER_CH_1, ENABLE);
}

void Buzzer_beep(u16 idx){
    if(idx == 0){ // 不发音
        Buzzer_stop();
        return;
    }
    
    Buzzer_play(FREQS[idx - 1]);
}

void Buzzer_stop(){
    // 禁用 TIMER的通道
    TIMER_channel_config(TIMER1, TIMER_CH_1, DISABLE);
}

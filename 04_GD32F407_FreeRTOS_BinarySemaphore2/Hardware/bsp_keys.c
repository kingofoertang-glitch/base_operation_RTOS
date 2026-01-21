#include "bsp_keys.h"
#include <stdio.h>

// PC0, PC1, PC2, PC3 作为按键上拉输入
// 初始化按键
void bsp_keys_init(void){
    // 1. 时钟初始化
    rcu_periph_clock_enable(RCU_GPIOC);
    // 2. 配置GPIO 输入输出模式
    gpio_mode_set(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
}

// keydown weak func
__attribute__((weak)) void Keys_on_keydown(uint8_t key){};
// keyup weak func
__attribute__((weak)) void Keys_on_keyup(uint8_t key){};

// 记录上一次状态
FlagStatus last_state0 = SET;
FlagStatus last_state1 = SET;
FlagStatus last_state2 = SET;
FlagStatus last_state3 = SET;

// 扫描按键
uint8_t bsp_keys_scan(void){

    uint8_t keys_state = 0xFF;

    // PC0
    FlagStatus state0 = gpio_input_bit_get(GPIOC, GPIO_PIN_0);
    if(state0 != last_state0){
        last_state0 = state0;
        if(state0 == RESET){
            Keys_on_keydown(0);
            // 如果按下, 设置为0
            keys_state &= ~(0x01 << 0);
        }else{
            Keys_on_keyup(0);
        }
    }
    // PC1
    FlagStatus state1 = gpio_input_bit_get(GPIOC, GPIO_PIN_1);
    if(state1 != last_state1){
        last_state1 = state1;
        if(state1 == RESET){
            Keys_on_keydown(1);
            // 如果按下, 设置为0
            keys_state &= ~(0x01 << 1);
        }else{
            Keys_on_keyup(1);
        }
    }
    // PC2
    FlagStatus state2 = gpio_input_bit_get(GPIOC, GPIO_PIN_2);
    if(state2 != last_state2){
        last_state2 = state2;
        if(state2 == RESET){
            Keys_on_keydown(2);
            // 如果按下, 设置为0
            keys_state &= ~(0x01 << 2);
        }else{
            Keys_on_keyup(2);
        }
    }
    // PC3
    FlagStatus state3 = gpio_input_bit_get(GPIOC, GPIO_PIN_3);
    if(state3 != last_state3){
        last_state3 = state3;
        if(state3 == RESET){
            Keys_on_keydown(3);
            // 如果按下, 设置为0
            keys_state &= ~(0x01 << 3);
        }else{
            Keys_on_keyup(3);
        }
    }

    // 返回按键状态
    return keys_state;
}
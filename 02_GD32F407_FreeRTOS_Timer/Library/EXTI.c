#include "EXTI.h"


void EXTI_config(
    rcu_periph_enum rcu, uint32_t gpio, uint32_t pull_up_down, uint32_t pin,
    uint8_t exti_port, uint8_t exti_pin,
    exti_line_enum linex, exti_trig_type_enum trig_type,
    IRQn_Type nvic_irq, uint8_t nvic_irq_pre_priority, uint8_t nvic_irq_sub_priority
){
    // GPIO初始化 ---------------------------------------
    rcu_periph_clock_enable(rcu);
    // GPIO配置模式
    gpio_mode_set(gpio, GPIO_MODE_INPUT, pull_up_down, pin);
    
    // EXTI初始化 ---------------------------------------
    rcu_periph_clock_enable(RCU_SYSCFG);

    /* 将外部中断线line连接到PA0引脚 connect EXTI line to GPIO pin */
    syscfg_exti_line_config(exti_port, exti_pin);

    /* 初始化EXTI_1并监听边缘 configure EXTI line */
    exti_init(linex, EXTI_INTERRUPT, trig_type);
    
    // NVIC中断配置 ---------------------------------------    
    /* 配置EXTI1优先级 enable and set key EXTI interrupt to the lowest priority */
    nvic_irq_enable(nvic_irq, nvic_irq_pre_priority, nvic_irq_sub_priority);
    // 启用EXTI_1的中断功能
    exti_interrupt_enable(linex);    
    // 清理一次标记, 避免无法触发中断函数或者刚初始化就触发
    exti_interrupt_flag_clear(linex);
}

// 只支持软触发
void EXTI_soft_config(
    exti_line_enum linex, 
    IRQn_Type nvic_irq, uint8_t nvic_irq_pre_priority, uint8_t nvic_irq_sub_priority
){
    // EXTI初始化 ---------------------------------------
    /* 初始化EXTI_1并监听边缘 configure EXTI line */
    exti_init(linex, EXTI_INTERRUPT, EXTI_TRIG_NONE);
    
    // NVIC中断配置 ---------------------------------------    
    /* 配置EXTI1优先级 enable and set key EXTI interrupt to the lowest priority */
    nvic_irq_enable(nvic_irq, nvic_irq_pre_priority, nvic_irq_sub_priority);
    // 启用EXTI_1的中断功能
    exti_interrupt_enable(linex);    
    // 清理一次标记, 避免无法触发中断函数或者刚初始化就触发
    exti_interrupt_flag_clear(linex);
}

// 一次性把所有要用到的外部中断全部初始化
void EXTI_init(){

#if USE_EXTI0
    #if EXTI0_SOFT
    EXTI_soft_config(EXTI_0, EXTI0_IRQn, EXTI0_IRQ_PRI);
    #else
    EXTI_config(
        EXTI0_RCU, EXTI0_GPIO, EXTI0_PUPD, GPIO_PIN_0,
        EXTI0_SOURCE_PORT, EXTI_SOURCE_PIN0,
        EXTI_0, EXTI0_TRIG_TYPE,
        EXTI0_IRQn, EXTI0_IRQ_PRI
    );
    #endif
#endif

#if USE_EXTI1
    #if EXTI1_SOFT
    EXTI_soft_config(EXTI_1, EXTI1_IRQn, EXTI1_IRQ_PRI);
    #else
    EXTI_config(
        EXTI1_RCU, EXTI1_GPIO, EXTI1_PUPD, GPIO_PIN_1,
        EXTI1_SOURCE_PORT, EXTI_SOURCE_PIN1,
        EXTI_1, EXTI1_TRIG_TYPE,
        EXTI1_IRQn, EXTI1_IRQ_PRI
    );
    #endif
#endif

#if USE_EXTI2
    #if EXTI2_SOFT
    EXTI_soft_config(EXTI_2, EXTI2_IRQn, EXTI2_IRQ_PRI);
    #else
    EXTI_config(
        EXTI2_RCU, EXTI2_GPIO, EXTI2_PUPD, GPIO_PIN_2,
        EXTI2_SOURCE_PORT, EXTI_SOURCE_PIN2,
        EXTI_2, EXTI2_TRIG_TYPE,
        EXTI2_IRQn, EXTI2_IRQ_PRI
    );
    #endif
#endif

#if USE_EXTI3
    EXTI_config(
        EXTI3_RCU, EXTI3_GPIO, EXTI3_PUPD, GPIO_PIN_3,
        EXTI3_SOURCE_PORT, EXTI_SOURCE_PIN3,
        EXTI_3, EXTI3_TRIG_TYPE,
        EXTI3_IRQn, EXTI3_IRQ_PRI
    );
#endif

#if USE_EXTI4
    EXTI_config(
        EXTI4_RCU, EXTI4_GPIO, EXTI4_PUPD, GPIO_PIN_4,
        EXTI4_SOURCE_PORT, EXTI_SOURCE_PIN4,
        EXTI_4, EXTI4_TRIG_TYPE,
        EXTI4_IRQn, EXTI4_IRQ_PRI
    );
#endif

#if USE_EXTI5
    EXTI_config(
        EXTI5_RCU, EXTI5_GPIO, EXTI5_PUPD, GPIO_PIN_5,
        EXTI5_SOURCE_PORT, EXTI_SOURCE_PIN5,
        EXTI_5, EXTI5_TRIG_TYPE,
        EXTI5_9_IRQn, EXTI5_IRQ_PRI
    );
#endif

#if USE_EXTI6
    EXTI_config(
        EXTI6_RCU, EXTI6_GPIO, EXTI6_PUPD, GPIO_PIN_6,
        EXTI6_SOURCE_PORT, EXTI_SOURCE_PIN6,
        EXTI_6, EXTI6_TRIG_TYPE,
        EXTI5_9_IRQn, EXTI6_IRQ_PRI
    );
#endif

#if USE_EXTI7
    EXTI_config(
        EXTI7_RCU, EXTI7_GPIO, EXTI7_PUPD, GPIO_PIN_7,
        EXTI7_SOURCE_PORT, EXTI_SOURCE_PIN7,
        EXTI_7, EXTI7_TRIG_TYPE,
        EXTI5_9_IRQn, EXTI7_IRQ_PRI
    );
#endif

#if USE_EXTI8
    EXTI_config(
        EXTI8_RCU, EXTI8_GPIO, EXTI8_PUPD, GPIO_PIN_8,
        EXTI8_SOURCE_PORT, EXTI_SOURCE_PIN8,
        EXTI_8, EXTI8_TRIG_TYPE,
        EXTI5_9_IRQn, EXTI8_IRQ_PRI
    );
#endif

#if USE_EXTI9
    EXTI_config(
        EXTI9_RCU, EXTI9_GPIO, EXTI9_PUPD, GPIO_PIN_9,
        EXTI9_SOURCE_PORT, EXTI_SOURCE_PIN9,
        EXTI_9, EXTI9_TRIG_TYPE,
        EXTI5_9_IRQn, EXTI9_IRQ_PRI
    );
#endif

#if USE_EXTI10
    EXTI_config(
        EXTI10_RCU, EXTI10_GPIO, EXTI10_PUPD, GPIO_PIN_10,
        EXTI10_SOURCE_PORT, EXTI_SOURCE_PIN10,
        EXTI_10, EXTI10_TRIG_TYPE,
        EXTI10_15_IRQn, EXTI10_IRQ_PRI
    );
#endif

#if USE_EXTI11
    EXTI_config(
        EXTI11_RCU, EXTI11_GPIO, EXTI11_PUPD, GPIO_PIN_11,
        EXTI11_SOURCE_PORT, EXTI_SOURCE_PIN11,
        EXTI_11, EXTI11_TRIG_TYPE,
        EXTI10_15_IRQn, EXTI11_IRQ_PRI
    );
#endif
    
#if USE_EXTI12
    EXTI_config(
        EXTI12_RCU, EXTI12_GPIO, EXTI12_PUPD, GPIO_PIN_12,
        EXTI12_SOURCE_PORT, EXTI_SOURCE_PIN12,
        EXTI_12, EXTI12_TRIG_TYPE,
        EXTI10_15_IRQn, EXTI12_IRQ_PRI
    );
#endif
    
#if USE_EXTI13
    EXTI_config(
        EXTI13_RCU, EXTI13_GPIO, EXTI13_PUPD, GPIO_PIN_13,
        EXTI13_SOURCE_PORT, EXTI_SOURCE_PIN13,
        EXTI_13, EXTI13_TRIG_TYPE,
        EXTI10_15_IRQn, EXTI13_IRQ_PRI
    );
#endif

#if USE_EXTI14
    EXTI_config(
        EXTI14_RCU, EXTI14_GPIO, EXTI14_PUPD, GPIO_PIN_14,
        EXTI14_SOURCE_PORT, EXTI_SOURCE_PIN14,
        EXTI_14, EXTI14_TRIG_TYPE,
        EXTI10_15_IRQn, EXTI14_IRQ_PRI
    );
#endif
    
#if USE_EXTI15
    EXTI_config(
        EXTI15_RCU, EXTI15_GPIO, EXTI15_PUPD, GPIO_PIN_15,
        EXTI15_SOURCE_PORT, EXTI_SOURCE_PIN15,
        EXTI_15, EXTI15_TRIG_TYPE,
        EXTI10_15_IRQn, EXTI15_IRQ_PRI
    );
#endif
}

#include <stdio.h>
__attribute__((weak)) void EXTI0_on_trig(void) {
    printf("EXTI_0 校长: default\n"); 
}
__attribute__((weak)) void EXTI1_on_trig(void) {}
__attribute__((weak)) void EXTI2_on_trig(void) {}
__attribute__((weak)) void EXTI3_on_trig(void) {}
__attribute__((weak)) void EXTI4_on_trig(void) {}
__attribute__((weak)) void EXTI5_on_trig(void) {}
__attribute__((weak)) void EXTI6_on_trig(void) {}
__attribute__((weak)) void EXTI7_on_trig(void) {}
__attribute__((weak)) void EXTI8_on_trig(void) {}
__attribute__((weak)) void EXTI9_on_trig(void) {}
__attribute__((weak)) void EXTI10_on_trig(void) {}
__attribute__((weak)) void EXTI11_on_trig(void) {}
__attribute__((weak)) void EXTI12_on_trig(void) {}
__attribute__((weak)) void EXTI13_on_trig(void) {}
__attribute__((weak)) void EXTI14_on_trig(void) {}
__attribute__((weak)) void EXTI15_on_trig(void) {}

void EXTI0_IRQHandler(void){
    if(SET == exti_interrupt_flag_get(EXTI_0)){
        exti_interrupt_flag_clear(EXTI_0);
        EXTI0_on_trig();
    }
}

void EXTI1_IRQHandler(void){
    if(SET == exti_interrupt_flag_get(EXTI_1)){
        exti_interrupt_flag_clear(EXTI_1);
        EXTI1_on_trig();
    }
}

void EXTI2_IRQHandler(void){
    if(SET == exti_interrupt_flag_get(EXTI_2)){
        exti_interrupt_flag_clear(EXTI_2);
        EXTI2_on_trig();
    }
}

void EXTI3_IRQHandler(void){
    if(SET == exti_interrupt_flag_get(EXTI_3)){
        exti_interrupt_flag_clear(EXTI_3);
        EXTI3_on_trig();
    }
}

void EXTI4_IRQHandler(void){
    if(SET == exti_interrupt_flag_get(EXTI_4)){
        exti_interrupt_flag_clear(EXTI_4);
        EXTI4_on_trig();
    }
}

void EXTI5_9_IRQHandler(void){
    if(SET == exti_interrupt_flag_get(EXTI_5)){
        exti_interrupt_flag_clear(EXTI_5);
        EXTI5_on_trig();
    }
    if(SET == exti_interrupt_flag_get(EXTI_6)){
        exti_interrupt_flag_clear(EXTI_6);
        EXTI6_on_trig();
    }
    if(SET == exti_interrupt_flag_get(EXTI_7)){
        exti_interrupt_flag_clear(EXTI_7);
        EXTI7_on_trig();
    }
    if(SET == exti_interrupt_flag_get(EXTI_8)){
        exti_interrupt_flag_clear(EXTI_8);
        EXTI8_on_trig();
    }
    if(SET == exti_interrupt_flag_get(EXTI_9)){
        exti_interrupt_flag_clear(EXTI_9);
        EXTI9_on_trig();
    }
}

void EXTI10_15_IRQHandler(void){
    if(SET == exti_interrupt_flag_get(EXTI_10)){
        exti_interrupt_flag_clear(EXTI_10);
        EXTI10_on_trig();
    }
    if(SET == exti_interrupt_flag_get(EXTI_11)){
        exti_interrupt_flag_clear(EXTI_11);
        EXTI11_on_trig();
    }
    if(SET == exti_interrupt_flag_get(EXTI_12)){
        exti_interrupt_flag_clear(EXTI_12);
        EXTI12_on_trig();
    }
    if(SET == exti_interrupt_flag_get(EXTI_13)){
        exti_interrupt_flag_clear(EXTI_13);
        EXTI13_on_trig();
    }
    if(SET == exti_interrupt_flag_get(EXTI_14)){
        exti_interrupt_flag_clear(EXTI_14);
        EXTI14_on_trig();
    }
    if(SET == exti_interrupt_flag_get(EXTI_15)){
        exti_interrupt_flag_clear(EXTI_15);
        EXTI15_on_trig();
    }
}

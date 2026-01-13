#include "TIMER.h"

// GPIO 初始化
static void timer_gpio_config(rcu_periph_enum rcu, 
    uint32_t port, uint32_t pin, uint32_t alt_func_num){
    
    rcu_periph_clock_enable(rcu);
    // GPIO模式 AF
    gpio_mode_set(port, GPIO_MODE_AF, GPIO_PUPD_NONE, pin);
    // GPIO输出选项
    gpio_output_options_set(port, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, pin);
    // GPIO引脚 AF 复用
    gpio_af_set(port, alt_func_num, pin);
    
}

// TIMER 初始化
static void timer_init_config(rcu_periph_enum rcu, 
    uint32_t timer_periph, uint16_t prescaler, uint32_t period){    
    rcu_periph_clock_enable(rcu);
    // 重置Timer
    timer_deinit(timer_periph);
    // 初始化结构体参数
    timer_parameter_struct initpara;
    /* 初始化为默认参数 initialize TIMER init parameter struct */
    timer_struct_para_init(&initpara);
    // 配置分频系数 Max: 65535
    initpara.prescaler         = prescaler - 1U;    
    // 配置一个周期的计数值
    initpara.period            = period - 1U;
    /* 初始化TIMER定时器 initialize TIMER counter */
    timer_init(timer_periph, &initpara);
    // 启用Timer
    timer_enable(timer_periph);
}

// TIMER 通道初始化
void TIMER_channel_config(uint32_t timer_periph, uint32_t channel, uint8_t enable){
     // 初始化输出通道-------------------------------------------------
    timer_oc_parameter_struct ocpara;
    /* 初始化默认参数 initialize TIMER channel output parameter struct */
    timer_channel_output_struct_para_init(&ocpara);
    /* 启用TIMER0_CH0的OP极(正极)  Positive*/
    ocpara.outputstate  = enable ? TIMER_CCX_ENABLE : TIMER_CCX_DISABLE;
    /* 启用TIMER0_CH0的ON极(负极)  Negative*/
    ocpara.outputnstate = enable ? TIMER_CCXN_ENABLE : TIMER_CCXN_DISABLE;
    /* 配置TIMER0_CH0的OP极(正极)极性 */    
    ocpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    /* 配置TIMER0_CH0的ON极(负极)极性 */    
    ocpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_channel_output_config(timer_periph, channel, &ocpara);
    /* 配置通道输出的比较模式 configure TIMER channel output compare mode */
    timer_channel_output_mode_config(timer_periph, channel, TIMER_OC_MODE_PWM0);
}
    
/*
 * @brief 初始化定时器
*/
void TIMER_init(){
    // 升级所有TIMER频率 x2=84MHz  x4=168MHz
    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);

// ================================= TIMER0 ================================= 
#if USE_TIMER_0 
    // 初始化Timer0
    timer_init_config(RCU_TIMER0, TIMER0, TM0_PRESCALER, TM0_PERIOD);
    
    #ifdef TM0_CH0
    timer_gpio_config(TM0_CH0,  GPIO_AF_1);
    #endif
    #ifdef TM0_CH0_ON
    timer_gpio_config(TM0_CH0_ON,  GPIO_AF_1);
    #endif

    // 初始化Timer0 CH0通道
    TIMER_channel_config(TIMER0, TIMER_CH_0, ENABLE);
#endif

// ================================= TIMER1 ================================= 
#if USE_TIMER_1
    // 初始化Timer1
    timer_init_config(RCU_TIMER1, TIMER1, TM1_PRESCALER, TM1_PERIOD);
    
    #ifdef TM1_CH1
    timer_gpio_config(TM1_CH1,  GPIO_AF_1);
    #endif

    // 初始化Timer1 CH1通道
    TIMER_channel_config(TIMER1, TIMER_CH_1, ENABLE);
#endif

// ================================= TIMER3 ================================= 
#if USE_TIMER_3
    
    // 初始化Timer3
    timer_init_config(RCU_TIMER3, TIMER3, TM3_PRESCALER, TM3_PERIOD);
    
    #ifdef TM3_CH0
    timer_gpio_config(TM3_CH0,  GPIO_AF_2);
    // 初始化Timer3 CH0通道
    TIMER_channel_config(TIMER3, TIMER_CH_0, ENABLE);
    #endif

    #ifdef TM3_CH1
    timer_gpio_config(TM3_CH1,  GPIO_AF_2);
    // 初始化Timer3 CH1通道
    TIMER_channel_config(TIMER3, TIMER_CH_1, ENABLE);
    #endif

    #ifdef TM3_CH2
    timer_gpio_config(TM3_CH2,  GPIO_AF_2);
    // 初始化Timer3 CH2通道
    TIMER_channel_config(TIMER3, TIMER_CH_2, ENABLE);
    #endif

    #ifdef TM3_CH3
    timer_gpio_config(TM3_CH3,  GPIO_AF_2);
    // 初始化Timer3 CH3通道
    TIMER_channel_config(TIMER3, TIMER_CH_3, ENABLE);
    #endif
#endif



// ================================= Break ================================= 
#if USE_TIMER_0 || USE_TIMER_7
    // 针对Timer0和Timer7高级定时器, 必须配置break保护电路, 才能输出
    /* TIMER通道互补保护电路 */
    timer_break_parameter_struct breakpara;
    /* 初始化TIMER break参数结构体 */
    timer_break_struct_para_init(&breakpara);
    /* break输入的极性 HIGH */
    breakpara.breakpolarity   = TIMER_BREAK_POLARITY_HIGH;
    /* 输出自动的启用 */
    breakpara.outputautostate = TIMER_OUTAUTO_ENABLE;
    /* break输入的启用*/
    breakpara.breakstate      = TIMER_BREAK_ENABLE;
    /* 死区时间 */    
    breakpara.deadtime        = 0U;

    #if USE_TIMER_0
    /* 配置TIMER0 break */
    timer_break_config(TIMER0, &breakpara);
    /* 启用TIMER0 break */
    timer_break_enable(TIMER0);
    #endif

    #if USE_TIMER_7
    /* 配置TIMER7 break */
    timer_break_config(TIMER7, &breakpara);
    /* 启用TIMER7 break */
    timer_break_enable(TIMER7);
    #endif

#endif


}

/***
 * @brief 更新指定TIMER和CHANNEL的占空比 (比较值)
 * @param timer TIMERx  TIMERx(x=0,1,2,3,4...13)
 * @param ch TIMER_CHx  TIMER_CH_0, TIMER_CH_1, TIMER_CH_2, TIMER_CH_3
 * @param duty 占空比 [0, 100]
 */
void TIMER_channel_update(uint32_t timer_periph, uint16_t channel, float duty){
    if(duty > 100) duty = 100;
    else if(duty < 0) duty = 0; 
    
    uint32_t pulse = (TIMER_CAR(timer_periph) - 1) * duty / 100.0f;
    // 配置通道输出的脉冲计数值(占空比)configure TIMER channel output pulse value
    timer_channel_output_pulse_value_config(timer_periph, channel, pulse);
}

/***
 * @brief 更新指定TIMER和CHANNEL的周期 (自动重装值)
 * @param timer TIMERx  TIMERx(x=0,1,2,3,4...13)
 * @param prescaler 分频系数 [1, 65536]
 * @param period 周期值 [1, 65536]
 */
void TIMER_period_update(uint32_t timer_periph, uint16_t prescaler, uint32_t period){
    // 配置分频系数 Max: 65535
    timer_prescaler_config(timer_periph, prescaler - 1U, TIMER_PSC_RELOAD_NOW);
    // 配置一个周期的计数值
    timer_autoreload_value_config(timer_periph, period - 1U);
}
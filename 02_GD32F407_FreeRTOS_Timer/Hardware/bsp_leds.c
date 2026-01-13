#include "bsp_leds.h"

// 声明gpio初始化所需要的所有参数
typedef struct {
    rcu_periph_enum rcu;
    uint32_t port;      // 端口
    uint32_t pin;       // 引脚
} Led_GPIO_t;

// 声明所有gpio的初始化参数
Led_GPIO_t g_gpio_list[] = {
    { RCU_GPIOC, GPIOC, GPIO_PIN_6 },// 0 总开关
    { RCU_GPIOD, GPIOD, GPIO_PIN_8 },// 1 LED
    { RCU_GPIOD, GPIOD, GPIO_PIN_9 },// 2 LED
    { RCU_GPIOD, GPIOD, GPIO_PIN_10},// 3 LED
    { RCU_GPIOD, GPIOD, GPIO_PIN_11},// 4 LED
    { RCU_GPIOD, GPIOD, GPIO_PIN_13},// 6 LED
    { RCU_GPIOD, GPIOD, GPIO_PIN_14},// 7 LED
    { RCU_GPIOD, GPIOD, GPIO_PIN_15},// 8 LED
};

// u8 计算数组的元素个数
uint8_t led_cnt = sizeof(g_gpio_list) / sizeof(Led_GPIO_t);

static void GPIO_config(rcu_periph_enum rcu, uint32_t port, uint32_t pin){
    // 初始化GPIO端口RCU时钟
    rcu_periph_clock_enable(rcu);
    // 设置GPIO模式: 初始化引脚为输出模式
    gpio_mode_set(port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin);
    // 设置GPIO输出选项
    gpio_output_options_set(port, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, pin);

}
void bsp_leds_init(){

    // 将结构体数组里的所有引脚初始化为推挽输出, 并拉高点评
    for(uint8_t i = 0; i < led_cnt; i++){
        Led_GPIO_t gpio = g_gpio_list[i];
        // 初始化
        GPIO_config(gpio.rcu, gpio.port, gpio.pin);
        // 全部拉高
        gpio_bit_set(gpio.port, gpio.pin);
    }
    
    // 拉低总开关
    gpio_bit_reset(g_gpio_list[0].port, g_gpio_list[0].pin);
    
}

// 点亮指定位置的灯
void bsp_leds_turn_on(uint8_t led_index){
    gpio_bit_reset(g_gpio_list[led_index].port, g_gpio_list[led_index].pin);
    
}

// 熄灭指定位置的灯
void bsp_leds_turn_off(uint8_t led_index){
    gpio_bit_set(g_gpio_list[led_index].port, g_gpio_list[led_index].pin);
}
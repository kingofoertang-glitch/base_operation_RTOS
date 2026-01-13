#include "bsp_adkey.h"
#include "systick.h"

#define ADC_VAL_KEY_1_2_3 730
#define ADC_VAL_KEY_1_2   770
#define ADC_VAL_KEY_1_3   880
#define ADC_VAL_KEY_1     948
#define ADC_VAL_KEY_2_3   1780
#define ADC_VAL_KEY_2     2048
#define ADC_VAL_KEY_3     3144
#define ADC_VAL_KEY_UP    4050


// PA2 -> IN2 
static void ADC_config() {
    /****************** GPIO config *********************/
    // 1. 时钟初始化
    rcu_periph_clock_enable(RCU_GPIOA);
    // 2. 配置GPIO 输入输出模式
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_2);

    /******************** ADC config **********************/
    // 配置时钟
    rcu_periph_clock_enable(RCU_ADC0);
    // 重置
    adc_deinit();
    // 配置主频
    adc_clock_config(ADC_ADCCK_PCLK2_DIV4);
    ////////// 采样配置
    adc_resolution_config(ADC0, ADC_RESOLUTION_12B); // 分辨率
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);// 数据右对齐

    //////////// 规则配置
    // 配置通道和通道数量
    adc_routine_channel_config(ADC0, 0, ADC_CHANNEL_2, ADC_SAMPLETIME_56);
    adc_channel_length_config(ADC0, ADC_ROUTINE_CHANNEL, 1);

    /////////// 模式配置
    adc_special_function_config(ADC0, ADC_SCAN_MODE, DISABLE);// 取消扫描模式
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);// 连续模式

    // 打开ADC
    adc_enable(ADC0);

    // 校准ADC
    adc_calibration_enable(ADC0);
    delay_1ms(100);
    
    // 软触发一次
    adc_software_trigger_enable(ADC0, ADC_ROUTINE_CHANNEL);
}

void bsp_adkey_init(){
    ADC_config();
}

static uint16_t adkey_get_value(){

//    adc_software_trigger_enable(ADC0, ADC_ROUTINE_CHANNEL);
//    while(RESET == adc_flag_get(ADC0, ADC_FLAG_EOC));
//    adc_flag_clear(ADC0, ADC_FLAG_EOC);

    uint16_t value = adc_routine_data_read(ADC0);  
//    float vol = value * 3.3 / 4096;  
//    return vol;
  
    return value;
}

__attribute__((weak)) void bsp_adkey_on_down(uint8_t index){};
__attribute__((weak)) void bsp_adkey_on_up(uint8_t index){};

#include <stdio.h>

// 0, 1, 2
#define RANGE 20
#define KEY1  (1 << 0)
#define KEY2  (1 << 1)
#define KEY3  (1 << 2)
  
// (±RANGE)用于判断是否在某个范围内(def - RANGE, def + RANGE)
#define IS_CATCH(value, def) ((value) > (def - RANGE)) && ((value) < (def + RANGE))
  
#define ADC_COUNT 5
uint16_t recent_values[ADC_COUNT];
uint8_t  recent_index = 0;
  
// 取最近N次采样的平均值(去掉最大和最小值)
uint16_t calc_recent_avg(void){
  
  uint32_t sum = 0;
  uint16_t max = 0, min = 65535;
  
  for(uint8_t i = 0; i < ADC_COUNT; i++){
    uint16_t v = recent_values[i];    
    // 有任何一个值更大, 记录下来
    if(v > max) max = v;
    // 有任何一个值更小, 记录下来
    if(v < min) min = v;
    
    sum += v;
  }
  return (sum - max - min) / (ADC_COUNT - 2);  
}
  
void bsp_adkey_scan(){
  static uint8_t key_state = 0x00, last_state = 0x00; // 1: 按下, 0:抬起
  
  uint16_t value = adkey_get_value();
  
  // 步骤3: 优化: 滑动窗口均值滤波 --------------------------------------------------
  // 将数据缓存到数组
  recent_values[recent_index++] = value;
  // 保证recent_index不越界
//  if (recent_index >= ADC_COUNT) recent_index = 0;
  recent_index %= ADC_COUNT;
  
  // 计算平均值
  uint16_t avg_val = calc_recent_avg();

#if 1  
  // 新的avg_val连续出现3次,才认为真正稳定的有效数字
  static uint8_t avg_new_repeat = 0; // 新的数字重复次数
  static uint16_t avg_new = 0;
  
  if(avg_new_repeat < 3){ // 10, 10, 10, 15, 20, 30, 30, 30, 30
      if(avg_val == avg_new){ 
        avg_new_repeat++;
      }
      // 新的值
      avg_new = avg_val;
      return;
  }
  avg_new_repeat = 0;
#endif
  
  //  printf("adc_value: %d\n",ser value);
//  printf("%d,%d,%d,%d\n", ADC_VAL_KEY_1, ADC_VAL_KEY_2, ADC_VAL_KEY_3, avg_val);
  
  // 步骤1: 分段--------------------------------------------------
  if(avg_val > ADC_VAL_KEY_UP){
    key_state = 0x00;
  }else if(IS_CATCH(avg_val, ADC_VAL_KEY_1_2_3)){
//    key_state = 0b00000111;    
    // 按键1,2,3同时按下了
    key_state |= KEY1;
    key_state |= KEY2;
    key_state |= KEY3;
  }else if(IS_CATCH(avg_val, ADC_VAL_KEY_1_2)){
    // 按键1,2同时按下了
    key_state |= KEY1;
    key_state |= KEY2;
  }else if(IS_CATCH(avg_val, ADC_VAL_KEY_1_3)){
    // 按键1,3同时按下了
    key_state |= KEY1;
    key_state |= KEY3;
  }else if(IS_CATCH(avg_val, ADC_VAL_KEY_2_3)){
    // 按键2,3同时按下了
    key_state |= KEY2;
    key_state |= KEY3;
  }else if(IS_CATCH(avg_val, ADC_VAL_KEY_1)){
    key_state |= KEY1;
  }else if(IS_CATCH(avg_val, ADC_VAL_KEY_2)){
    key_state |= KEY2;
  }else if(IS_CATCH(avg_val, ADC_VAL_KEY_3)){
    key_state |= KEY3;
  }
  // 打印key_state低3位
//  for(int i = 0; i < 3; i++){
//    printf("%d ", (key_state >> i) & 0x01);
//  }
//  printf("\n");
  
  // 0b 0000 0001 last_state
  //^0b 0000 0100 key_state
  //    0000 0101  (1变0, 0变1) diff
  // 步骤2: 回调判断 --------------------------------------------------
  uint8_t diff = last_state ^ key_state; // 发生变化的bit位会被设置为1
  
  // 判断按键是否发生了变化
  if(diff == 0b00000000){  
    return; // 按键没有变化
  }
  // 记录最新的状态
  last_state = key_state;
  
  // 根据最新的状态判定, 按下抬起
  if(diff & KEY1){  // 说明KEY1变化
    if(key_state & KEY1){
      bsp_adkey_on_down(0); // 按下
    }else{
      bsp_adkey_on_up(0); // 抬起
    }
  }
  
  if(diff & KEY2){  // 说明KEY2变化
    if(key_state & KEY2){
      bsp_adkey_on_down(1); // 按下
    }else{
      bsp_adkey_on_up(1); // 抬起
    }
  }
  
  if(diff & KEY3){  // 说明KEY3变化
    if(key_state & KEY3){
      bsp_adkey_on_down(2); // 按下
    }else{
      bsp_adkey_on_up(2); // 抬起
    }
  }

}


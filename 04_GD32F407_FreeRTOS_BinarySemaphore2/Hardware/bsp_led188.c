#include "bsp_led188.h"

/*****************
1. 在十位数显示数字0
2. 在十位数显示数字8
3. 将188数码管所有灯都点亮
4. 根据指定数字点灯: 123 % 闪电

逐个引脚拉低, 按需拉高
******************/

static void GPIO_config(rcu_periph_enum rcu, uint32_t port, uint32_t pin) {
  // 1. 时钟初始化
  rcu_periph_clock_enable(rcu);
  // 2. 配置GPIO 输入输出模式
  gpio_mode_set(port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin);
  // 3. 配置GPIO 输出选项
  gpio_output_options_set(port, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, pin);
  // 4. 默认输出电平
  gpio_bit_write(port, pin, RESET);
}

  
void LED188_init(){
  GPIO_config(LED188_RCU, LED188_1_PORT, LED188_1_PIN);
  GPIO_config(LED188_RCU, LED188_2_PORT, LED188_2_PIN);
  GPIO_config(LED188_RCU, LED188_3_PORT, LED188_3_PIN);
  GPIO_config(LED188_RCU, LED188_4_PORT, LED188_4_PIN);
  GPIO_config(LED188_RCU, LED188_5_PORT, LED188_5_PIN);  
}

static uint8_t num_table[] = {
  0x3F, // 0
  0x06, // 1
  0x5B, // 2
  0x4F, // 3
  0x66, // 4
  0x6D, // 5
  0x7D, // 6
  0x07, // 7
  0x7F, // 8
  0x6F, // 9
};


#define A3 (1L << 0)
#define B3 (1L << 1)
#define C3 (1L << 2)
#define D3 (1L << 3)
#define E3 (1L << 4)
#define F3 (1L << 5)
#define G3 (1L << 6)

#define A2 (1L << 8)
#define B2 (1L << 9)
#define C2 (1L << 10)
#define D2 (1L << 11)
#define E2 (1L << 12)
#define F2 (1L << 13)
#define G2 (1L << 14)

#define B1 (1L << 16)
#define C1 (1L << 17)

#define K1 (1L << 18)
#define K2 (1L << 19)

//////////////////////////////////////////////////////////////////////////////////////////////////
//  19  18      17  16      15  14  13  12  11  10   9   8      7   6   5   4   3   2   1   0   //
//  K2  K1  |   C1  B1  |   XX  G2  F2  E2  D2  C2  B2  A2  |  XX  G3  F3  E3  D3  C3  B3  A3   //
//////////////////////////////////////////////////////////////////////////////////////////////////

//												  1 2 3
// static uint32_t states = 0x0F666D;
static uint32_t states = 0b000011111111000001001111;
//                             **##xGFEDCBAxGFEDCBA

void LED188_set_num(uint8_t num){  // [0, 199]
  
  // 123
  if(num > 199) num = 199;
  
  // 取出百位 -------------------------------
  if(num >= 100){
    states |= 0x030000;
  }else {
    states &= ~0x030000;  
  }
  
  // 123 -> 12 -> 2
  // 取出十位 -------------------------------
  uint8_t shi = num / 10 % 10;
  // 先清空十位数字内容
  states &= ~0x00FF00;
  // 填充要显示的数字内容
  states |= (num_table[shi] << 8);
  
  // 取出个位 -------------------------------
  uint8_t ge = num % 10;
  // 先清空个位数字内容
  states &= ~0x0000FF;
  // 填充要显示的数字内容
  states |= num_table[ge];
  
}

void LED188_clear(){ // 关闭显示

  states = 0x000000;
  
}

void LED188_set_charge(uint8_t enable){
  
  if(enable){
    states |= K1;
  }else {
    states &= ~K1;  
  }
}

void LED188_set_percent(uint8_t enable){
  
  if(enable){
    states |= K2;
  }else {
    states &= ~K2;  
  }

}
////////////////////////////////////////////////
// 低/高 |   1   |   2   |   3   |   4   |   5
//   1   |   X   |  B3   |  D3   |   F3  |   G3
//   2   |   A3  |   X   |  B2   |   D2  |   E2
//   3   |   C3  |  A2   |  X    |   C2  |   F2
//   4   |   E3  |  C1   |  B1   |   X   |   G2
//   5   |   X   |  K2   |  K1   |   X   |   X
////////////////////////////////////////////////
void LED188_display(){ // 2ms
  // 2ms * 5 = 10ms
  // 1000ms / 10ms = 100帧/秒
  static uint8_t i = 1; 
  
  // 先把所有引脚默认改为高阻输入
  ALL_IN();
  
  if(states == 0x00) return;
  
  switch(i++){  // 1,2,3,4,5,1,2,3,4,5,1,2,3,4,5,1,2,3,4,5,...

    case 1: // 拉低1, 其他按需拉高
      OUT1(0);
    
      if(states & B3) OUT2(1);  // B3
      if(states & D3) OUT3(1);  // D3
      if(states & F3) OUT4(1);  // F3
      if(states & G3) OUT5(1);  // G3
      break;
    case 2: // 拉低2, 其他按需拉高
      OUT2(0);
    
      if(states & A3) OUT1(1);  // A3
      if(states & B2) OUT3(1);  // B2
      if(states & D2) OUT4(1);  // D2
      if(states & E2) OUT5(1);  // E2
      break;
    case 3: // 拉低3, 其他按需拉高
      OUT3(0);
    
      if(states & C3) OUT1(1);  // C3
      if(states & A2) OUT2(1);  // A2
      if(states & C2) OUT4(1);  // C2
      if(states & F2) OUT5(1);  // F2
      break;
    case 4: // 拉低4, 其他按需拉高
      OUT4(0);
    
      if(states & E3) OUT1(1);  // E3
      if(states & C1) OUT2(1);  // C1
      if(states & B1) OUT3(1);  // B1
      if(states & G2) OUT5(1);  // G2
      break;
    case 5:// 拉低5, 其他按需拉高
      OUT5(0);
      
      if(states & K2) OUT2(1);  // K2 百分号
      if(states & K1) OUT3(1);  // K1 闪电
    
      i = 1; // 重置为1
      break;
    default:
      break;
  }
  
  
}
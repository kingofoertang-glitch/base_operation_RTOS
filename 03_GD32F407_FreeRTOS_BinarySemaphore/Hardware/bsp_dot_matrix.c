#include "bsp_dot_matrix.h"

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

static void GPIO_init(){
  GPIO_config(DM_CLK_RCU, DM_CLK_PORT, DM_CLK_PIN);
  GPIO_config(DM_DIO_RCU, DM_DIO_PORT, DM_DIO_PIN);
  GPIO_config(DM_CS_RCU, DM_CS_PORT, DM_CS_PIN);
}

void DM_write_byte(uint8_t dat){
  // 高位先出 0b1010 1100
  for(uint8_t i = 0; i < 8; i++){
    // 拉低CLK
    DM_CLK_L();

    // 根据要发送的位, 输出高电平或低电平
    if(dat & 0x80){
      DM_DIO_H();
    } else{
      DM_DIO_L();
    }
    // 左移一位, 准备发送下一位, 移除已发送的位
    dat <<= 1;

    // 拉高CLK
    DM_CLK_H();
  }
}

void DM_write_command(uint8_t reg, uint8_t dat){
  // 拉低CS
  DM_CS_L();
  // 发送地址寄存器
  DM_write_byte(reg);
  // 发送数据寄存器
  DM_write_byte(dat);
  // 拉高CS
  DM_CS_H();
}

void DM_init(){  
  
  GPIO_init();
  // 初始化参数
  DM_write_command(0x09, 0x00); // 译码模式：0x00 关闭， 0xFF 开启 （B 型 BCD 译码）
  DM_write_command(0x0A, 0x02); // 亮度设置：0x00 最低， 0x0F 最高
  DM_write_command(0x0B, 0x07); // 扫描模式： 显示前N+1行
  DM_write_command(0x0C, 0x01); // 关机模式： 0x00 关机， 0x01 正常显示
  DM_write_command(0x0F, 0x00); // 测试模式： 0x00 正常显示， 0x01 测试模式(全亮)
}

void DM_display(uint8_t* dat){ // 8个单字节参数
  for(uint8_t i = 0; i < 8; i++){
    DM_write_command(i + 1, dat[i]);
  }
  
  // DM_write_command(0x01, 0b00000000);
  // DM_write_command(0x02, 0b00000000);
  // DM_write_command(0x03, 0b00000000);
  // DM_write_command(0x04, 0b00000000);
  // DM_write_command(0x05, 0b00000000);
  // DM_write_command(0x06, 0b00000000);
  // DM_write_command(0x07, 0b00000000);
  // DM_write_command(0x08, 0b00000000);
}
// 0x00FF00
// 0x00AA00
// 0x00BB00
// 0x00FF00
// 0x00FF00
// 0x00FF00
// 0x00FF00
// 0x00FF00

// 0xAFBB00  >> 8    -> 0x00AFBB
// 0x00AFBB & 0xFF   -> 0xBB
void DM_display2(uint32_t* dat){ // 8个4字节参数, 只取每行1号字节
  for(uint8_t i = 0; i < 8; i++){
    DM_write_command(i + 1, (dat[i] >> 8) & 0xFF);
  }
  
}
#include "bsp_ws2812.h"
#include "SPI/SPI.h"
#include <string.h>


// 10 Green, 10 Red, 10 Blue

static uint8_t data[WS2812_NUM][SPI_BYTE_LEN] = {0};
//                                       0            1
static uint16_t lookup_table[2] = {0b1110000000, 0b1111111000};

void bsp_ws2812_init(){

  // 初始化二维数组内容为0
  memset(data, 0x00, WS2812_NUM * SPI_BYTE_LEN);  
}

// 32bit 缓冲区 右进(每次10bit), 左出(每次8bit)
// 0b     1110000000  1111111000
void convert_1BYTE_to_10byte(uint8_t BYTE, uint8_t output[10]){
  
  uint32_t buffer = 0x00000000; // 当做缓冲区处理位操作
  int bit_count = 0;  // 记录缓冲区有效的spi byte个数
  int output_cnt = 0; // 记录输出的字节个数
  
  // 将BYTE的最高位到最低位依次取出, 逐个转成10bit的spi数据
  for(int i = 7; i >= 0; i--){ // 7,6,5,4,3,2,1,0
    // 将WS2812的字节BYTE, 转成spi发送用的字节byte
    uint16_t mapping = lookup_table[(BYTE >> i) & 0x01];
    // 将mapping的低10位, 放到buffer的最右边
    buffer = (buffer << 10) | mapping;
    // 记录已经存储的bit个数
    bit_count += 10;
    // 从左边取8bit存进output
    while(bit_count >= 8){      
      // 把高8bit取出放到输出字节数组里
      output[output_cnt++] = (buffer >> (bit_count - 8)) & 0xFF;
      // 更新有效bit位个数
      bit_count -= 8;  
    }    
  }
}

// 0xAARRGGBB
// 8bit(保留) + 8bit(RED) + 8bit(Green) + 8bit(Blue)
void bsp_ws2812_set_color(uint16_t index, uint32_t color){
  // 分别取出 R, G, B
  uint8_t R = (color >> 16) & 0xFF;
  uint8_t G = (color >> 8) & 0xFF;
  uint8_t B = (color >> 0) & 0xFF;
  
  uint8_t tmp[30];
  
  // G -> 10byte  [0, 9]
  convert_1BYTE_to_10byte(G, &tmp[0]);
  // R -> 10byte  [10, 19]
  convert_1BYTE_to_10byte(R, &tmp[10]);
  // R -> 10byte  [20, 29]
  convert_1BYTE_to_10byte(B, &tmp[20]);
  
  // 将tmp内容拷贝到指定 LED
  memcpy(data[index], tmp, SPI_BYTE_LEN);
}

void bsp_ws2812_display(){
  SPI1_dma_tx_start(&data[0][0], WS2812_NUM * SPI_BYTE_LEN, 1);
}

#include <math.h>


/**
 * 将HSV颜色模式转成RGB
 * hue：[0, 360)		色相：表示颜色，比如红橙黄绿青蓝紫
 * saturation：[0, 100%]	饱和度：颜色的深浅、浓度、鲜艳程度。如红色可以分为深红、洋红、浅红等。
 * value：[0, 100%]		明度：颜色的强度，明暗
 * 
 */
void bsp_ws2812_HSVtoRGB(double h, double s, double v, u8* r, u8* g, u8* b) {
    double r1, g1, b1;

    double c = v * s;
    double x = c * (1 - fabs(fmod(h / 60.0, 2) - 1));
    double m = v - c;

    h = fmod(h, 360.0); // Ensure h is in the range [0, 360)
    s = fmax(0.0, fmin(1.0, s)); // Clamp s to [0, 1]
    v = fmax(0.0, fmin(1.0, v)); // Clamp v to [0, 1]


    if (h >= 0 && h < 60) {
        r1 = c, g1 = x, b1 = 0;
    } else if (h >= 60 && h < 120) {
        r1 = x, g1 = c, b1 = 0;
    } else if (h >= 120 && h < 180) {
        r1 = 0, g1 = c, b1 = x;
    } else if (h >= 180 && h < 240) {
        r1 = 0, g1 = x, b1 = c;
    } else if (h >= 240 && h < 300) {
        r1 = x, g1 = 0, b1 = c;
    } else {
        r1 = c, g1 = 0, b1 = x;
    }

    *r = (u8)((r1 + m) * 255);
    *g = (u8)((g1 + m) * 255);
    *b = (u8)((b1 + m) * 255);
}

void bsp_ws2812_HSVtoRGB24(double h, double s, double v, u32* rgb) {
    u8 r, g, b;
    bsp_ws2812_HSVtoRGB(h, s, v, &r, &g, &b);
    *rgb = ((u32)r << 16) | ((u32)g << 8) | b;
}
#include "bsp_ec12.h"
#include <stdio.h>


volatile uint8_t ec12_cur_state = 0;

__attribute__((weak)) void bsp_ec12_on_turn(uint8_t is_clockwise){}
  
__attribute__((weak)) void bsp_ec12_on_click(uint8_t is_down){}

  
void bsp_ec12_init(){

  
}

void EC12_A_TRIG_HANDLE(void) {
//  printf("EXTI 11\n");  
  ec12_cur_state = 1;
}

void EC12_B_TRIG_HANDLE(void) {
//  printf("EXTI 13\n");
//  ec12_cur_state = 2;
}

void EC12_D_TRIG_HANDLE(void) {
//  printf("EXTI 13\n");
//  ec12_cur_state = 3;
  
  // 读取D引脚最新的状态, 判定是按下还是抬起
  FlagStatus state = EC12_D_STATE();
  
  bsp_ec12_on_click(state);
  
}
  
  
void bsp_ec12_loop(){ // 5ms
  
  if(ec12_cur_state == 1){ // 说明A引脚发生跳变
    ec12_cur_state = 0;
    
    #if 0
    // 根据A当前状态, 判断A是发生了上升沿还是下降沿
    if(EC12_A_STATE()){ // A上升沿
      
      if(EC12_B_STATE()){// B高(逆时针)
        printf("逆CCW: 1\n");
      }else {            // B低(顺时针)
        printf("顺CW: 1\n"); 
      }
      
    }else {             // A下降沿
    
      if(EC12_B_STATE()){// B高(顺时针)
        printf("顺CW: 2\n");          
      }else {            // B低(逆时针)
        printf("逆CCW: 2\n");        
      }
    }
    #endif
    
    // 简洁写法: 判断A边沿发生后, 两个引脚高低是否相同
    // 相同为逆时针, 不同为顺时针
    // printf("1-> %s\n", EC12_A_STATE() == EC12_B_STATE() ? "逆CCW" : "顺CW"); 
    bsp_ec12_on_turn(EC12_A_STATE() != EC12_B_STATE());
  }
  
  if(ec12_cur_state == 2){ // 说明B引脚发生跳变(高变低 或 低变高)
    ec12_cur_state = 0;
    
    // 简洁写法: 判断B边沿发生后, 两个引脚高低是否相同
    // 相同为顺时针, 不同为逆时针
    // printf("2-> %s\n", EC12_A_STATE() == EC12_B_STATE() ? "顺CW" : "逆CCW"); 
  }
}

  
#include "bsp_touch_key.h"
#include <stdio.h>

void bsp_touch_key_init(){
    	// 初始化按键寄存器
	
	uint8_t databuf;
	
	// 配置 CIN4 的灵敏度
	databuf = 0x79;
	TK_I2C_WRITE(TK_DEVICE_ADDRESS, TK_REG_SEN_SET_0, &databuf, 1);
	// 配置 其他 的灵敏度
	databuf = 0x79;
	TK_I2C_WRITE(TK_DEVICE_ADDRESS, TK_REG_SEN_SET_COM, &databuf, 1);
	 // 控制寄存器1
	// 1. 采样周期间隔设置      3bit 4
	// 2. 进入睡眠的速度		   1bit 0 （慢）
	// 3. 基准线是否停止校准    1bit 1
	// 4. 按键后停止校准		   1bit 0
	// 5. 按键反应速度          2bit 3
	databuf = TK_SLPCYC_3R5T | TK_SLOW_TO_SLEEP | TK_HOLD | TK_KVF_50S_CORREC | TK_RTM3;
	TK_I2C_WRITE(TK_DEVICE_ADDRESS, TK_REG_CTRL_0, &databuf, 1);
	
	// 配置基准电容（15pF)
	databuf = 0x0F;
	TK_I2C_WRITE(TK_DEVICE_ADDRESS, TK_REG_CTRL_1, &databuf, 1);
}

volatile uint8_t touch_flag = 0;

void TK_INT_FUNC(void) { // 此函数是在中断里执行的
//    printf("触摸\n");    
    touch_flag = 1;
}

__attribute__((weak)) void bsp_touch_on_keydown(uint8_t key_index){ }
__attribute__((weak)) void bsp_touch_on_keyup(uint8_t key_index){ }

void bsp_touch_key_scan(){ // 5ms
//    printf("input\n");
    static uint16_t pre_state = 0x0000;
    
    if(touch_flag == 0) return;    
    touch_flag = 0;
    
//    printf("I2C读取SC12B数据\n");    
        
    uint8_t buf[2];
    // 从0x08寄存器开始, 读取2个字节
    int8_t rst = TK_I2C_READ(TK_DEVICE_ADDRESS, TK_REG_OUTPUT, buf, 2);
    
    if(rst != 0){        
        printf("I2C0_read err: %d\n", rst);            
        return ;
    }
    
    // 将两个字节合并成一个uint16_t
    uint16_t current = (uint16_t)((buf[0] << 8) | buf[1]);
    current >>= 4;

//        printf("current: %04X\n", current);
    
    // CH0 CH1 CH2 .....  CH11
    // 0b00001234 56789*0#
//        printf("-> ");
//        for(int8_t i = 11; i >= 0; i--){
//            printf("%d ", (current >> i) & 0x01);
//        }
//        printf("\n");
    
    // 将变化的位设置为1, 不变的位设置为0
    uint16_t diff = pre_state ^ current;
    
    if(diff == 0){
        // 状态没有变化, 不做任何处理
        return;            
    }
    
    // 0b 0000 00001010 pre
    //^0b 0000 01000010 cur
    // 0b 0000 01001000 diff    
    for(int8_t i = 11; i >= 0; i--) {
        if(((diff >> i) & 0x01) == 0) {
            continue; // 该位无变化
        }
        
        // i 位有变化, 按下0->1, 抬起:1->0
        if((current >> i) & 0x01){ // 1
//            printf("down->%d\n", 11 - i);
            bsp_touch_on_keydown(11 - i);
        }else {
//            printf("up->%d\n", 11 - i);   
            bsp_touch_on_keyup(11 - i);         
        }
        
    }
    
    // 保存上一次状态
    pre_state = current;
}
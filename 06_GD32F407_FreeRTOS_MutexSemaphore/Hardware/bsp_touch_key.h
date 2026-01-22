#ifndef __BSP_TOUCH_KEY_H__
#define __BSP_TOUCH_KEY_H__

#include "gd32f4xx.h"
#include "I2C.h"

// 高内聚, 低耦合
#define TK_INT_FUNC                         EXTI8_on_trig
#define TK_I2C_WRITE(addr, reg, data, len)  I2C0_write(addr, reg, data, len)
#define TK_I2C_READ(addr, reg, data, len)   I2C0_read(addr, reg, data, len)


#define TK_DEVICE_ADDRESS                     0x40                  


#define TK_REG_SEN_SET_0		    0x00 	//CIN4 通道灵敏度的设置地址
#define TK_REG_SEN_SET_COM	 	    0x01 	//其他通道灵敏度的设置地址
#define TK_REG_CTRL_0	 			0x02 	//CTRL0 控制寄存器设置地址
#define TK_REG_CTRL_1	 			0x03 	//CTRL1 控制寄存器设置地址
#define TK_REG_OUTPUT	 			0x08	//触摸状态寄存器输出地址
#define TK_REG_SAMP		 			0x0A 	//触摸数据值存器输出地址

#define TK_RTM0 0 	//3 个采样周期有效，1 个采样周期判断无效
#define TK_RTM1 1 	//4 个采样周期有效，2 个采样周期判断无效
#define TK_RTM2 2 	//5 个采样周期有效，3 个采样周期判断无效
#define TK_RTM3 3 	//6 个采样周期有效，4 个采样周期判断无效

#define TK_KVF_STOP_CORREC 	(1u<<2) // 按键有效，触摸不校准
#define TK_KVF_50S_CORREC 	(0u<<2) // 按下有效后，50S 开始校准

#define TK_HOLD 	(1u<<3) 	//基线保持不校准
#define TK_NOTHOLD 	(0u<<3) 	//基线持续校准

#define TK_FAST_TO_SLEEP (1u<<4) //快速进入休眠
#define TK_SLOW_TO_SLEEP (0u<<4) // 75S 进入休眠

#define TK_SLPCYC_LGT 	(0u<<5) //无穷大
#define TK_SLPCYC_0R5T 	(1u<<5) //休眠后采样间隔 60MS
#define TK_SLPCYC_1R5T 	(2u<<5) //休眠后采样间隔 180MS
#define TK_SLPCYC_2R5T 	(3u<<5) //休眠后采样间隔 300MS
#define TK_SLPCYC_3R5T 	(4u<<5) //休眠后采样间隔 420MS
#define TK_SLPCYC_4R5T 	(5u<<5) //休眠后采样间隔 540MS
#define TK_SLPCYC_5R5T 	(6u<<5) //休眠后采样间隔 660MS
#define TK_SLPCYC_6R5T 	(7u<<5) //休眠后采样间隔 780MS

void bsp_touch_key_init();

void bsp_touch_key_scan();


#endif
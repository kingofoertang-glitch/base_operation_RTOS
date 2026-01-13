#ifndef __I2C2_H__
#define __I2C2_H__

#include "gd32f4xx.h"

#include "I2C_config.h"

void I2C2_init();

/**********************************************************
 * @brief I2C写数据到指定设备的寄存器
 * @param addr 设备地址7bit
 * @param reg  寄存器地址 
 * @param data 字节数组
 * @param len  数据个数
 * @return 错误码: 0无错误
 **********************************************************/
int8_t I2C2_write(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len);

/**********************************************************
 * @brief I2C从指定设备的寄存器读取数据
 * @param addr 设备地址7bit
 * @param reg  寄存器地址 
 * @param data 用来接收数据的字节数组
 * @param len  要读取的数据个数
 * @return 错误码: 0无错误
 **********************************************************/
int8_t I2C2_read(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len);

#endif
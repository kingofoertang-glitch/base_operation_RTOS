#include "I2C1.h"

#if USE_I2C1


#if I2C1_SOFT

#include "systick.h"

#if I2C1_SPEED >= 400000U
#define DELAY()     delay_1us(1)
#else
#define DELAY()     delay_1us(5)
#endif

#define SDA(bit)    gpio_bit_write(I2C1_SDA_PORT, I2C1_SDA_PIN, bit ? SET : RESET)
#define SCL(bit)    gpio_bit_write(I2C1_SCL_PORT, I2C1_SCL_PIN, bit ? SET : RESET)

#define I2C1_SDA_IN()    gpio_mode_set(I2C1_SDA_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, I2C1_SDA_PIN)
#define I2C1_SDA_OUT()   gpio_mode_set(I2C1_SDA_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, I2C1_SDA_PIN)
#define I2C1_SDA_STATE() gpio_input_bit_get(I2C1_SDA_PORT, I2C1_SDA_PIN)


void I2C1_init(){
    
    
    // 软实现: CPU直接操作GPIO
    // 硬实现: 片上外设,AF复用
    
    // 标准模式: 100kbit/s (100kbps), 快速模式400kbit/s
    // 1 000 000 us / 100 000bit =  10us/bit     =>    5us休眠
    // 1 000 000 us / 400 000bit = 2.5us/bit     => 1.25us休眠
    
    // 将I2C引脚初始化为开漏输出(内部上拉)
    // PB6 SCL
    rcu_periph_clock_enable(I2C1_SCL_RCU);
    gpio_mode_set(I2C1_SCL_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, I2C1_SCL_PIN);
    gpio_output_options_set(I2C1_SCL_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_MAX, I2C1_SCL_PIN);
    // PB7 SDA
    rcu_periph_clock_enable(I2C1_SDA_RCU);
    gpio_mode_set(I2C1_SDA_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, I2C1_SDA_PIN);
    gpio_output_options_set(I2C1_SDA_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_MAX, I2C1_SDA_PIN);
    
    
}

// 函数声明
static void start();                // 开始信号
static void send(uint8_t data);     // 写地址|寄存器|数据
static uint8_t wait_ack();          // 等待响应(0成功, 其他失败)
static void stop();                 // 停止信号

static uint8_t recv();              // 接收一个字节
static void send_ack();             // 发送响应
static void send_nack();            // 发送响应

/**********************************************************
 * @brief I2C写数据到指定设备的寄存器
 * @param addr 设备地址7bit, 需要 << 1
 * @param reg  寄存器地址 
 * @param data 字节数组
 * @param len  数据个数
 * @return 错误码: 0无错误
 **********************************************************/
int8_t I2C1_write(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len){
    
    // 通用开头 --------------------------------------------↓
    // 发送开始信号
    start();
    
    // 发送设备地址(写地址)
    send(addr << 1);    // 0xA2
    // 等待响应
    if(wait_ack()) return 1; // 设备不存在
    
    // 发送寄存器地址
    send(reg);
    // 等待响应
    if(wait_ack()) return 2; // 寄存器不存在
    // 通用开头 --------------------------------------------↑
    
    // 循环发送所有的数据(字节)
    for(uint32_t i = 0; i < len; i++){
        // 发送数据
        send(data[i]);
        // 等待响应
        if(wait_ack()) return 3; // 数据写失败
    }
    
    // 发送停止信号
    stop();
    
    return 0;
}

int8_t I2C1_write2(uint8_t addr, uint8_t reg, uint8_t* data, uint16_t offset, uint32_t len){
    
    // 通用开头 --------------------------------------------↓
    // 发送开始信号
    start();
    
    // 发送设备地址(写地址)
    send(addr << 1);    // 0xA2
    // 等待响应
    if(wait_ack()) return 1; // 设备不存在
    
    // 发送寄存器地址
    send(reg);
    // 等待响应
    if(wait_ack()) return 2; // 寄存器不存在
    // 通用开头 --------------------------------------------↑
    
    // 循环发送所有的数据(字节)
    for(uint32_t i = 0; i < len; i++){
        // 发送数据
        send(data[i * offset]);
        // 等待响应
        if(wait_ack()) return 3; // 数据写失败
    }
    
    // 发送停止信号
    stop();
    
    return 0;
}

/**********************************************************
 * @brief I2C从指定设备的寄存器读取数据
 * @param addr 设备地址7bit (例如0x51)
 * @param reg  寄存器地址 
 * @param data 用来接收数据的字节数组
 * @param len  要读取的数据个数
 * @return 错误码: 0无错误
 **********************************************************/
int8_t I2C1_read(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len){
    
    // 通用开头 --------------------------------------------↓
    // 发送开始信号
    start();
    
    // 发送设备地址(写地址)
    send(addr << 1);            // 0xA2
    // 等待响应
    if(wait_ack()) return 1; // 设备不存在
    
    // 发送寄存器地址
    send(reg);
    // 等待响应
    if(wait_ack()) return 2; // 寄存器不存在
    // 通用开头 --------------------------------------------↑
    
    // 再发送开始信号
    start();
    
    // 发送设备地址(读地址)
    send(addr << 1 | 0x01);    // 0xA3
    // 等待响应
    if(wait_ack()) return 3;   // 设备不存在
    
    /*******************循环接收数据*****************/
    for(uint32_t i = 0; i < len; i++){
        // 接收字节
        data[i] = recv();
        
        if(i != len - 1){
            send_ack(); // 发送响应信号
        }else {
            send_nack();// 发送空响应信号
        }
    }
    
    /************************************************/
    
    // 发送停止信号
    stop();
    
    return 0;
}


// 开始信号
static void start(){
    
    // 拉高两个引脚
    SDA(1);
    DELAY();
    SCL(1);
    DELAY();
    
    // 按顺序拉低
    SDA(0); // 核心: SCL高电平时, 拉低SDA
    DELAY();
    SCL(0);
    DELAY();
    
}

// 写地址|寄存器|数据
static void send(uint8_t data){
    // 8bit, 先发高位
    // 1101 0010
    // 101 0010            << 1
    // 01 0010            << 1
    
    //&1000 0000
    
    for(uint8_t i = 0; i < 8; i++){
        // 根据要发的最高位决定SDA高低
        if(data & 0x80){ // 获取最高位
           SDA(1); // 是1
        }else {
           SDA(0);
        }
        // 左移data
        data <<= 1;
        DELAY();
        
        SCL(1); // 从机在此时读数据
        DELAY();// 高电平数据有效期, SDA不能动
        SCL(0);
        DELAY();// 低电平数据非有效期, SDA可以修改
        
    }
    
}
// 等待响应(0成功, 其他失败)
static uint8_t wait_ack(){
    // 拉高SDA, 等待从设备拉低
    SDA(1);
    DELAY();
    
    // 拉高SCL, 同时释放SDA控制权(变为输入模式)
    SCL(1);
    I2C1_SDA_IN();
    DELAY(); // 从机此时会拉低SDA
    
    if(I2C1_SDA_STATE() == RESET){
        // 从机拉低了SDA, 应答成功
        SCL(0);
        I2C1_SDA_OUT();
    }else {
        // 无人应答, 返回错误
        SCL(0);
        I2C1_SDA_OUT();
        // 发送stop信号
        stop();
        return 1;
    }
    
    return 0;
}          
// 停止信号
static void stop(){
    
//    SCL(0);
//    DELAY();

    // 拉低SDA引脚
    SDA(0);
    DELAY();
    
    SCL(1); // 确保SCL是高电平
    DELAY();
    SDA(1); // 核心: SCL高电平时, 拉高SDA
    DELAY();
}

// 接收一个字节
static uint8_t recv(){
    // 释放SDA控制权, 进入输入模式
    I2C1_SDA_IN();
    
    uint8_t cnt  = 8;    // 1字节->8bit
    uint8_t data = 0x00; // 空的容器,接收数据
    
    while(cnt--){       // 接收一个bit位(先收高位)
        // SCL拉低
        SCL(0);
        DELAY();        // 从机即可修改SDA数据
        
        SCL(1);         // 设置数据有效性
        
#if 0
        // 0000 0000 -> 1101 0010
        // 0000 0000
        // 0000 0001    7
        // 0000 0011    6
        // 0000 0110    5
        // 0000 1101    4
        // 0001 1010    3
        // 0011 0100    2
        // 0110 1001    1
        // 1101 0010    0
        data <<= 1;
        
        if(I2C1_SDA_STATE()){
            data++;
        }
#endif
        
        // 0000 0000 -> 1101 0010
        // 1000 0000 7
        // 1100 0000 6
        // 1100 0000 5
        // 1101 0000 4
        // 1101 0000 3
        // 1101 0000 2
        // 1101 0010 1
        // 1101 0010 0
        
        if(I2C1_SDA_STATE()){
            data |= (1 << cnt);
        }
        
        // SCL在高电平Delay一会儿, 进入下一个循环
        DELAY();
    }
    
    // SCL拉低
    SCL(0);
    
    return data;
}

// 发送响应
static void send_ack(){
    // 获取SDA控制权
    I2C1_SDA_OUT();
    
    // 拉低SDA
    SDA(0);
    DELAY();
    
    // 拉高SCL
    SCL(1);
    DELAY();
    
    // 拉低SCL
    SCL(0);
    DELAY();
}
// 发送响应
static void send_nack(){
    // 获取SDA控制权
    I2C1_SDA_OUT();
    
    // 拉低SDA
    SDA(1);
    DELAY();
    
    // 拉高SCL
    SCL(1);
    DELAY();
    
    // 拉低SCL
    SCL(0);
    DELAY();
} 


#else

#define i2cx    I2C1

void I2C1_init(){    
    // 硬实现: 片上外设, AF复用

    // 标准模式: 100kbit/s (100kbps), 快速模式400kbit/s
    // 1 000 000 us / 100 000bit =  10us/bit     =>    5us休眠
    // 1 000 000 us / 400 000bit = 2.5us/bit     => 1.25us休眠

    // GPIO引脚 --------------------------------------------------
    // 初始化SCL引脚为上拉复用AF
    rcu_periph_clock_enable(I2C1_SCL_RCU);
    gpio_mode_set(I2C1_SCL_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, I2C1_SCL_PIN);
    gpio_output_options_set(I2C1_SCL_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_MAX, I2C1_SCL_PIN);
    gpio_af_set(I2C1_SCL_PORT, GPIO_AF_4, I2C1_SCL_PIN);
    // 初始化SDA引脚为上拉复用AF
    rcu_periph_clock_enable(I2C1_SDA_RCU);
    gpio_mode_set(I2C1_SDA_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, I2C1_SDA_PIN);
    gpio_output_options_set(I2C1_SDA_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_MAX, I2C1_SDA_PIN);
    gpio_af_set(I2C1_SDA_PORT, GPIO_AF_4, I2C1_SDA_PIN);

    // I2C外设 --------------------------------------------------
    // 时钟
    rcu_periph_clock_enable(RCU_I2C1);
    /* 重置 reset I2C */
    i2c_deinit(i2cx);
    /* 配置速度, 低高电平比例 configure I2C clock */
    i2c_clock_config(i2cx, I2C1_SPEED, I2C_DTCY_16_9);
    /* configure I2C address */
    i2c_mode_addr_config(i2cx, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, 0x00);
    /* 启用  */
    i2c_enable(i2cx);

    /* whether or not to send an ACK */
    i2c_ack_config(i2cx, I2C_ACK_ENABLE);
}

#define	TIMEOUT	50000
// 等待信号变为SET, 返回0为成功
static uint8_t I2C_wait(uint32_t flag) {
    uint16_t cnt = 0;

    while(!i2c_flag_get(i2cx, flag)) {
        cnt++;
        if(cnt > TIMEOUT) return 1;
    }
    return 0;
}

// 等待信号变为RESET, 返回0为成功
static uint8_t I2C_waitn(uint32_t flag) {
    uint16_t cnt = 0;

    while(i2c_flag_get(i2cx, flag)) {
        cnt++;
        if(cnt > TIMEOUT) return 1;
    }
	return 0;
}

/**********************************************************
 * @brief I2C写数据到指定设备的寄存器
 * @param addr 设备地址7bit
 * @param reg  寄存器地址 
 * @param data 字节数组
 * @param len  数据个数
 * @return 错误码: 0无错误
 **********************************************************/
int8_t I2C1_write(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len){
    uint8_t write_addr = addr << 1;

    /************* start ***********************/
    // 等待I2C闲置
    if(I2C_waitn(I2C_FLAG_I2CBSY)) return 1;
    // start
    i2c_start_on_bus(i2cx);
    // 等待I2C主设备成功发送起始信号
    if(I2C_wait(I2C_FLAG_SBSEND)) return 2;

    /************* device address **************/
    // 发送设备地址
    i2c_master_addressing(i2cx, write_addr, I2C_TRANSMITTER);
    // 等待地址发送完成
    if(I2C_wait(I2C_FLAG_ADDSEND)) return 3;
    i2c_flag_clear(i2cx, I2C_FLAG_ADDSEND);

    /************ register address ************/
    // 寄存器地址
    // 等待发送数据缓冲区为空
    if(I2C_wait(I2C_FLAG_TBE)) return 4;
    // 发送数据
    i2c_data_transmit(i2cx, reg);
    // 等待数据发送完成
    if(I2C_wait(I2C_FLAG_BTC)) return 5;

    /***************** data ******************/
    // 发送数据
    uint32_t i;
    for(i = 0; i < len; i++) {

        // 等待发送数据缓冲区为空
        if(I2C_wait(I2C_FLAG_TBE)) return 6;

        // 发送数据
        i2c_data_transmit(i2cx, data[i]);

        // 等待数据发送完成
        if(I2C_wait(I2C_FLAG_BTC)) return 7;
    }

    /***************** stop ********************/
    // stop
    i2c_stop_on_bus(i2cx);

    while(I2C_CTL0(i2cx) & I2C_CTL0_STOP);    
    i2c_ack_config(i2cx, I2C_ACK_ENABLE);

    return 0;
}

/**********************************************************
 * @brief I2C从指定设备的寄存器读取数据
 * @param addr 设备地址7bit
 * @param reg  寄存器地址 
 * @param data 用来接收数据的字节数组
 * @param len  要读取的数据个数
 * @return 错误码: 0无错误
 **********************************************************/
int8_t I2C1_read(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len){
    uint8_t write_addr = addr << 1;
    uint8_t read_addr  = (addr << 1) | 1;
    // 等待I2C空闲
    if(I2C_waitn(I2C_FLAG_I2CBSY)) return 1;

    /************* start ***********************/
    // 发送启动信号
    i2c_start_on_bus(i2cx);
    // 等待I2C主设备成功发送起始信号
    if(I2C_wait(I2C_FLAG_SBSEND)) return 2;

    /************* device address **************/
    // 发送从设备写地址
    i2c_master_addressing(i2cx, write_addr, I2C_TRANSMITTER);
    if(I2C_wait(I2C_FLAG_ADDSEND)) return 3;
    i2c_flag_clear(i2cx, I2C_FLAG_ADDSEND);

    /********** register address **************/
    // 等待发送缓冲区	
    if(I2C_wait(I2C_FLAG_TBE)) return 4;
    // 发送寄存器地址
    i2c_data_transmit(i2cx, reg);
    // 等待发送数据完成	
    if(I2C_wait(I2C_FLAG_BTC)) return 5;

    /************* start ***********************/
    // 发送再启动信号
    i2c_start_on_bus(i2cx);
    if(I2C_wait(I2C_FLAG_SBSEND)) return 7;

    /************* device address **************/
    // 发送从设备读地址
    i2c_master_addressing(i2cx, read_addr, I2C_RECEIVER);
    if(I2C_wait(I2C_FLAG_ADDSEND)) return 8;
    i2c_flag_clear(i2cx, I2C_FLAG_ADDSEND);

    /************* data **************/
    //ack
    i2c_ack_config(i2cx, I2C_ACK_ENABLE);
    // 接收一个数据后，自动发送ACK
    i2c_ackpos_config(i2cx, I2C_ACKPOS_CURRENT);
    // 确认ACK已启用
    if(I2C_wait(I2C_CTL0(i2cx) & I2C_CTL0_ACKEN)) return 11;

    // 读取数据
    uint32_t i;
    for (i = 0; i < len; i++) {
        if (i == len - 1) {
            // 在读取最后一个字节之前，禁用ACK，配置为自动NACK
            i2c_ack_config(i2cx, I2C_ACK_DISABLE);
        }

        // 等待接收缓冲区不为空
        if(I2C_wait(I2C_FLAG_RBNE)) return 10;

        data[i] = i2c_data_receive(i2cx);
    }

    /***************** stop ********************/
    i2c_stop_on_bus(i2cx);
    while(I2C_CTL0(i2cx) & I2C_CTL0_STOP);

    i2c_ack_config(i2cx, I2C_ACK_ENABLE);

    return 0;
}

#endif


#endif
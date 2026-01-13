#include "USART0.h"
#include <stdio.h>
#include <string.h>

#define  RX_BUF_LEN  1024
// 数据接收缓冲区
uint8_t  g_rx_buffer[RX_BUF_LEN]; // abc
// 收到的字节个数
uint32_t g_rx_cnt = 0;

#define USART0_DATA_ADDR               ((uint32_t)&USART_DATA(USART0))


// 1. 配置DMA发送设置

static void USART0_DMA_TX_config(){
    // M2P: 内存到外设拷贝
    // 方向: 动态内存地址 -> 固定USART_DATA外设地址
    // src: memory0_addr
    // dst: periph_addr   
    // DMA1, CH7, 子集100 -> USART0_TX
    
    // DMA初始化 --------------------------------
    rcu_periph_clock_enable(USART0_TX_DMA_RCU); // m2m只能使用DMA1
    
    /* 重置DMA1 deinitialize DMA a channel registers */
    dma_deinit(USART0_TX_DMA_PERIPH_CH); // 内存到内存, 可随意指定通道
    
    dma_single_data_parameter_struct init_struct;
    /* 初始化默认参数 initialize the DMA single data mode parameters struct with the default values */
    dma_single_data_para_struct_init(&init_struct);
    /* 配置拷贝方向: 内存到外设 */
    init_struct.direction           = DMA_MEMORY_TO_PERIPH;
    
    /* 配置数据源地址: 动态内存, 发送前才确定 */
//    init_struct.memory0_addr        = (uint32_t)src;
    init_struct.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
    
    /* 配置目标地址: 外设 */
    init_struct.periph_addr         = USART0_DATA_ADDR;
    init_struct.periph_inc          = DMA_PERIPH_INCREASE_DISABLE; // 自增increase
    
    /* 配置数据的宽度和总个数 */
    init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
//    init_struct.number              = sizeof(src) / sizeof(src[0]);
    
    /* 循环模式(关闭) */
    init_struct.circular_mode       = DMA_CIRCULAR_MODE_DISABLE;
    init_struct.priority            = DMA_PRIORITY_LOW; // 优先级

    /* 执行DMA的初始化 DMA single data mode initialize */
    dma_single_data_mode_init(USART0_TX_DMA_PERIPH_CH, &init_struct);
    
    /* 配置通道子集(外设) DMA channel peripheral select */
    dma_channel_subperipheral_select(USART0_TX_DMA_PERIPH_CH, USART0_TX_DMA_SUBPERI);

    
//    // 中断初始化 --------------------------------
//    // 开启传输完成中断(可选)
//    nvic_irq_enable(DMA1_Channel0_IRQn, 2, 2);
//    // 启用中断标记 enable DMA interrupt
//    dma_interrupt_enable(DMA_PERIPH_CH, DMA_INT_FTF);
}


// 2. 配置DMA接收设置
static void USART0_DMA_RX_config(){
    
    // P2M: 外设到内存拷贝
    // 方向: 固定USART_DATA外设地址 -> 固定内存地址
    // src: periph_addr
    // dst: memory0_addr
    // DMA1, CH2, 子集100 -> USART0_RX
    
    // DMA初始化 --------------------------------
    rcu_periph_clock_enable(USART0_RX_DMA_RCU); // m2m只能使用DMA1
    
    /* 重置DMA1 deinitialize DMA a channel registers */
    dma_deinit(USART0_RX_DMA_PERIPH_CH); // 内存到内存, 可随意指定通道
    
    dma_single_data_parameter_struct init_struct;
    /* 初始化默认参数 initialize the DMA single data mode parameters struct with the default values */
    dma_single_data_para_struct_init(&init_struct);
    /* 配置拷贝方向: 外设到内存 */
    init_struct.direction           = DMA_PERIPH_TO_MEMORY;
    
    /* 配置数据源地址: 外设 */
    init_struct.periph_addr         = USART0_DATA_ADDR;
    init_struct.periph_inc          = DMA_PERIPH_INCREASE_DISABLE; // 关闭自增 
    
    /* 配置目标地址: 内存 */
    init_struct.memory0_addr        = (uint32_t)g_rx_buffer;
    init_struct.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;  // 开启自增 
    
    /* 配置数据的宽度和总个数 */
    init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
    init_struct.number              = RX_BUF_LEN;                   // 设置为缓冲区大小
    
    /* 循环模式(关闭) */
    init_struct.circular_mode       = DMA_CIRCULAR_MODE_DISABLE;
    init_struct.priority            = DMA_PRIORITY_LOW; // 优先级

    /* 执行DMA的初始化 DMA single data mode initialize */
    dma_single_data_mode_init(USART0_RX_DMA_PERIPH_CH, &init_struct);
    
    /* 配置通道子集(外设) DMA channel peripheral select */
    dma_channel_subperipheral_select(USART0_RX_DMA_PERIPH_CH, USART0_RX_DMA_SUBPERI);

    /* 通知DMA去USART0搬运数据 */
    dma_channel_enable(USART0_RX_DMA_PERIPH_CH);
}

static void USART_config(){
    
#if USART0_DMA_TX_ENABLE
    USART0_DMA_TX_config();
#endif 
    
#if USART0_DMA_RX_ENABLE
    USART0_DMA_RX_config();
#endif
    
    // GPIO引脚初始化 --------------------------------------------------------
    /* 启用GPIO时钟 */    
    rcu_periph_clock_enable(USART0_TX_RCU);
    rcu_periph_clock_enable(USART0_RX_RCU);
    
    /* 配置PA9的GPIO模式 configure USART0 TX as alternate function push-pull */
    gpio_mode_set(USART0_TX_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, USART0_TX_PIN);
    gpio_output_options_set(USART0_TX_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, USART0_TX_PIN);

    /* 配置PA10的GPIO模式 configure USART0 RX as alternate function push-pull */
    gpio_mode_set(USART0_RX_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, USART0_RX_PIN);
    
    /* 复用功能配置 configure the USART0 TX pin and USART0 RX pin */
    gpio_af_set(USART0_TX_PORT, GPIO_AF_7, USART0_TX_PIN);
    gpio_af_set(USART0_RX_PORT, GPIO_AF_7, USART0_RX_PIN);
    
    // 串口初始化 -----------------------------------------------------------
    /* 启用USART0时钟 */    
    rcu_periph_clock_enable(RCU_USART0);
    /* 重置复位USART0 */
    usart_deinit(USART0);
    /* 配置串口参数: 波特率*, 数据位, 校验位, 停止位, 大小端模式 */
    usart_baudrate_set(USART0, USART0_BAUDRATE); 
    usart_word_length_set(USART0, USART_WL_8BIT);   // 数据位: 默认8bit
    usart_parity_config(USART0, USART_PM_NONE);     // 校验位: 默认无校验
    usart_stop_bit_set(USART0, USART_STB_1BIT);     // 停止位: 默认1bit
    usart_data_first_config(USART0, USART_MSBF_LSB);// 数据模式: 小端模式
    /* 启用发送功能 */
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    /* 启用接收功能 */
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    
#if USART0_DMA_TX_ENABLE
    /* 启用DMA发送功能 configure USART DMA for transmission */
    usart_dma_transmit_config(USART0, USART_TRANSMIT_DMA_ENABLE);
#endif
#if USART0_DMA_RX_ENABLE
    /* 启用DMA接收功能 configure USART DMA for reception */
    usart_dma_receive_config(USART0, USART_RECEIVE_DMA_ENABLE);
#endif

    // 中断初始化 --------------------------------------------------------------
    // 配置中断优先级 
    nvic_irq_enable(USART0_IRQn, 6, 0); // 串口中断优先级在FreeRTOS可用范围[5, 15]
    // 启用中断
#if !USART0_DMA_RX_ENABLE
    // 启用RBNE中断: 读取数据缓冲区不为空中断read data buffer not empty interrupt
    usart_interrupt_enable(USART0, USART_INT_RBNE);
#endif
    // 启用IDLE中断: 当发送者停止发送数据, 空闲时间超过1个字符帧, 触发
    usart_interrupt_enable(USART0, USART_INT_IDLE);
    
    /* 启用USART */
    usart_enable(USART0);
}

void USART0_init() {
    USART_config();
}

// 发送多个字节(字节数字)
void USART0_send_data_array(uint8_t* arr, uint32_t len){
    
#if USART0_DMA_TX_ENABLE
    // 设置数据源地址 set DMA Memory base address 
    dma_memory_address_config(USART0_TX_DMA_PERIPH_CH, DMA_MEMORY_0, (uint32_t)arr);
    // 设置要搬运的数据个数
    dma_transfer_number_config(USART0_TX_DMA_PERIPH_CH, len);
    // CPU通知开始DMA开始干活(非阻塞) enable DMA channel
    dma_channel_enable(USART0_TX_DMA_PERIPH_CH);
    
    // 阻塞等待DMA搬运完毕(可以实现代码里, 连续打印需求)
    while(RESET == dma_flag_get(USART0_TX_DMA_PERIPH_CH, DMA_FLAG_FTF));
    dma_flag_clear(USART0_TX_DMA_PERIPH_CH, DMA_FLAG_FTF);

#else
        while(arr && len--){ // 指针不为空, 长度 > 0
        USART0_send_data(*arr);
        arr++;
    }
#endif    
}

// 发送1个字节数据
void USART0_send_data(uint8_t data){
#if USART0_DMA_TX_ENABLE
    USART0_send_data_array(&data, 1);
#else
    // 通过USART0发送字节
    usart_data_transmit(USART0, data);
    // 阻塞等待: 直到上边这个字节发送完毕(buffer为空, 返回SET, 循环结束解除阻塞)
    // Transmit data Buffer Empty 发送数据缓冲区为空
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
#endif
    
}


// 发送字符串 \0
void USART0_send_string(char* str){
#if USART0_DMA_TX_ENABLE
    USART0_send_data_array((uint8_t *)str, strlen(str));
#else
    // 循环继续条件: 1. data指针不能为空 2.要发的数据不是\0 (0x00字符串结束的标记)
    while(str && *str){ // (*str) != '\0'        
        USART0_send_data((uint8_t)*str);
        str++;
    }    
#endif
}

// 配置printf数据打印实现
int fputc(int ch, FILE *f){  
    USART0_send_data((uint8_t) ch);
    return ch;
} 

uint8_t usart0_recive_blocking(){
    // 阻塞式等待数据 Read data Buffer Not Empty
    while(RESET == usart_flag_get(USART0, USART_FLAG_RBNE));        
    // 接收数据
    return usart_data_receive(USART0);
}

// 弱函数, 提供函数的基础实现，可以被其他地方的相同函数覆盖
__attribute__((weak)) void USART0_on_recv(uint8_t* data, uint32_t len){ }

/*******
中断函数: 收到任意标记信号, 立即执行
1. 触发中断的原因(标记)很多
2. 需要区分是哪个标记触发的中断函数
3. 清理中断标记, 避免重复多次触发

recv[4]->%s\n
********/
void USART0_IRQHandler(void){
    
#if USART0_DMA_RX_ENABLE

    if(SET == usart_interrupt_flag_get(USART0, USART_INT_FLAG_IDLE)){              
        // 需要读取一次数据(数据不可用), 则会自动清理标记
        usart_data_receive(USART0);
//        printf("IDLE\n"); // 只会在接收空闲时执行(超过一个字节帧没数据)
        
        /* 获取剩余未搬运的字节数 get the number of remaining data to be transferred by the DMA */
        uint32_t remaining_cnt = dma_transfer_number_get(USART0_RX_DMA_PERIPH_CH);

//        printf("remaining_cnt: %d\n", remaining_cnt);
//        printf("g_rx_buffer: %s\n", g_rx_buffer);
        
        // 计算已搬运的字节数量
        g_rx_cnt = RX_BUF_LEN - remaining_cnt;
        
        // 添加字符串结束标记
        g_rx_buffer[g_rx_cnt] = '\0';
        
//        printf("recv[%d]->%s\n", g_rx_cnt, g_rx_buffer);
        USART0_on_recv(g_rx_buffer, g_rx_cnt);
        
        // 重新给DMA安排接收任务
        dma_channel_disable(USART0_RX_DMA_PERIPH_CH);
        // 清理FTF标记
        dma_flag_clear(USART0_RX_DMA_PERIPH_CH, DMA_FLAG_FTF);
        // 启用DMA搬运
        dma_channel_enable(USART0_RX_DMA_PERIPH_CH);
    }

#else
    // 需要区分是哪个标记触发的中断函数
    if(SET == usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE)){         
//        printf("RBNE\n"); 收到N个字节就会执行N次
        // 清理中断标记, 避免重复多次触发
        usart_interrupt_flag_clear(USART0, USART_INT_FLAG_RBNE);
        
        // 读取寄存器里的数据
        uint8_t data = usart_data_receive(USART0);
        
        // 缓存到buffer里
        g_rx_buffer[g_rx_cnt++] = data;
        
        // 避免缓冲溢出
        if(g_rx_cnt >= RX_BUF_LEN) g_rx_cnt = 0;
//        USART0_send_data(data); // 把每个字节原样返回      
        
    }
    
    // 需要区分是哪个标记触发的中断函数
    if(SET == usart_interrupt_flag_get(USART0, USART_INT_FLAG_IDLE)){  
//        printf("IDLE\n"); // 只会在接收空闲时执行(超过一个字节帧没数据)
        // 不能通过clear函数清理标记, 需要读取一次数据(数据不可用), 则会自动清理标记
        usart_data_receive(USART0);
        
        // 添加字符串结束标记
        g_rx_buffer[g_rx_cnt] = '\0';
    
//        printf("recv[%d]->%s\n", g_rx_cnt, g_rx_buffer);
        USART0_on_recv(g_rx_buffer, g_rx_cnt);
        
        // 把缓冲区收的数据清理为0x00 (可选)       memcpy     
//        memset(g_rx_buffer, 0x00, g_rx_cnt);
        g_rx_cnt = 0;
    }
    
#endif
    
}






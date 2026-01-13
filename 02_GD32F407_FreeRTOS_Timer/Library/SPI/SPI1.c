#include "SPI1.h"

#if USE_SPI_1

#if SPI1_SOFT


#define CLK(bit) 	gpio_bit_write(SPI1_CLK_PORT, SPI1_CLK_PIN, ((bit) == 0) ? RESET : SET)
#define MOSI(bit) 	gpio_bit_write(SPI1_MOSI_PORT,SPI1_MOSI_PIN, ((bit) == 0) ? RESET : SET)

#if SPI1_MISO_ENABLE
#define MISO_STATE()  gpio_input_bit_get(SPI1_MISO_PORT, SPI1_MISO_PIN)
#endif

void SPI1_init() {
    /*********** GPIO config ***************/
    // clk
    rcu_periph_clock_enable(SPI1_CLK_PORT_RCU);
    gpio_mode_set(SPI1_CLK_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, SPI1_CLK_PIN);
    gpio_output_options_set(SPI1_CLK_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, SPI1_CLK_PIN);

#if SPI1_MOSI_ENABLE
    // mosi
    rcu_periph_clock_enable(SPI1_MOSI_PORT_RCU);
    gpio_mode_set(SPI1_MOSI_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, SPI1_MOSI_PIN);
    gpio_output_options_set(SPI1_MOSI_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, SPI1_MOSI_PIN);
#endif
#if SPI1_MISO_ENABLE
    // miso
    rcu_periph_clock_enable(SPI1_MISO_PORT_RCU);
    gpio_mode_set(SPI1_MISO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, SPI1_MISO_PIN);
    gpio_output_options_set(SPI1_MISO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, SPI1_MISO_PIN);
#endif
}


#if SPI1_MOSI_ENABLE
void SPI1_write(uint8_t dat) {
    uint8_t i;
    for(i=0; i<8; i++)
    {
        CLK(0);
        if(dat&0x80)
        {
            MOSI(1);
        }
        else
        {
            MOSI(0);
        }
        dat<<=1;
        CLK(1);
    }
    CLK(0);//拉低
}
#endif
#if SPI1_MISO_ENABLE
uint8_t SPI1_read() {
    uint8_t i,read=0;
    for(i=0; i<8; i++)
    {
        CLK(0);
        read<<=1;
        if(MISO_STATE())
        {
            read++;
        }
        CLK(1);
    }
    CLK(0);//拉低
    return read;
}
#endif

#if SPI1_MOSI_ENABLE && SPI1_MISO_ENABLE
uint8_t SPI1_write_read(uint8_t dat) {
    uint8_t i,read=0;
    for(i=0; i<8; i++)
    {
        CLK(0);
        MOSI(dat&0x80);
        dat<<=1;

        read<<=1;
        if(MISO_STATE())
        {
            read++;
        }
        CLK(1);
    }
    CLK(0);//拉低
    return read;
}
#endif


#else

#define SPIx_RCU		RCU_SPI1
#define SPIx			SPI1

#define SPI1_CLK_AF		GPIO_AF_5

#if SPI1_MOSI_ENABLE
#define SPI1_MOSI_AF	GPIO_AF_5
#endif

#if SPI1_MISO_ENABLE
#define SPI1_MISO_AF    GPIO_AF_5
#endif


#if SPI1_DMA_TX_ENABLE
#define SPIx_DMAx_TX_RCU       RCU_DMA0
#define SPIx_DMAx_TX_CHx       DMA0, DMA_CH4
#define SPIx_DMAx_TX_SUBPERI   DMA_SUBPERI0

#if SPI1_DMA_TX_CALLBACK
#define SPIx_DMAx_TX_IRQ       DMA0_Channel4_IRQn
#define SPIx_INT_FUNC          DMA0_Channel4_IRQHandler
#endif

static void dma_tx_init() {
    /***************** DMA m2p *******************/
    // 时钟
    rcu_periph_clock_enable(SPIx_DMAx_TX_RCU);
    // 重置dma
    dma_deinit(SPIx_DMAx_TX_CHx);

    //////// dma 配置
    dma_single_data_parameter_struct dsdps;
    dma_single_data_para_struct_init(&dsdps);
    // 方向
    dsdps.direction = DMA_MEMORY_TO_PERIPH;
    // 内存: src 
    // dsdps.memory0_addr = (uint32_t)src;
    dsdps.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    // 外设: dst
    dsdps.periph_addr = (uint32_t)(&SPI_DATA(SPIx));
    dsdps.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    // 数据长度
    // dsdps.number = ARR_LEN;
    // 数据宽度
    dsdps.periph_memory_width = DMA_MEMORY_WIDTH_8BIT;
    // 传输优先级
    dsdps.priority = DMA_PRIORITY_ULTRA_HIGH;
//    // 循环模式
//    dsdps.circular_mode = DMA_CIRCULAR_MODE_ENABLE;
    dma_single_data_mode_init(SPIx_DMAx_TX_CHx, &dsdps);

    //////// 配置 dma 子连接
    dma_channel_subperipheral_select(SPIx_DMAx_TX_CHx, SPIx_DMAx_TX_SUBPERI);
    
    dma_channel_disable(SPIx_DMAx_TX_CHx);
    dma_flag_clear(SPIx_DMAx_TX_CHx, DMA_FLAG_FTF);
    
#if SPI1_DMA_TX_CALLBACK
    dma_interrupt_enable(SPIx_DMAx_TX_CHx, DMA_INT_FTF);
    nvic_irq_enable(SPIx_DMAx_TX_IRQ, SPI1_DMA_TX_NVIC);
#endif
}

void SPI1_dma_tx_start(uint8_t* data, uint32_t len, uint8_t src_increment) {
    
     /* 关闭DMA通道 */
    dma_channel_disable(SPIx_DMAx_TX_CHx);
    dma_flag_clear(SPIx_DMAx_TX_CHx, DMA_FLAG_FTF);
    /* 配置DMA传输的内存地址和传输数据大小 */
    dma_memory_address_config(SPIx_DMAx_TX_CHx, DMA_MEMORY_0, (uint32_t)data);
    dma_transfer_number_config(SPIx_DMAx_TX_CHx, len);
    if(src_increment) {
        dma_memory_address_generation_config(SPIx_DMAx_TX_CHx, DMA_MEMORY_INCREASE_ENABLE);
    } else {
        dma_memory_address_generation_config(SPIx_DMAx_TX_CHx, DMA_MEMORY_INCREASE_DISABLE);
    }
        /* 使能DMA通道 */
    dma_channel_enable(SPIx_DMAx_TX_CHx);
}

void SPI1_dma_tx_stop() {
    dma_channel_disable(SPIx_DMAx_TX_CHx);
}

#if SPI1_DMA_TX_CALLBACK
void SPIx_INT_FUNC() {
    if(SET == dma_interrupt_flag_get(SPIx_DMAx_TX_CHx, DMA_INT_FLAG_FTF)) {
        dma_interrupt_flag_clear(SPIx_DMAx_TX_CHx, DMA_INT_FLAG_FTF);
        SPI1_dma_tx_on_finish();
    }
}
#endif

#endif

void SPI1_init() {

    /*********** GPIO config ***************/
    // clk
    rcu_periph_clock_enable(SPI1_CLK_PORT_RCU);
    gpio_mode_set(SPI1_CLK_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, SPI1_CLK_PIN);
    gpio_output_options_set(SPI1_CLK_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, SPI1_CLK_PIN);
    gpio_af_set(SPI1_CLK_PORT, SPI1_CLK_AF, SPI1_CLK_PIN);

#if SPI1_MOSI_ENABLE
    // mosi
    rcu_periph_clock_enable(SPI1_MOSI_PORT_RCU);
    gpio_mode_set(SPI1_MOSI_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, SPI1_MOSI_PIN);
    gpio_output_options_set(SPI1_MOSI_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, SPI1_MOSI_PIN);
    gpio_af_set(SPI1_MOSI_PORT, SPI1_MOSI_AF, SPI1_MOSI_PIN);
#endif
#if SPI1_MISO_ENABLE
    // miso
    rcu_periph_clock_enable(SPI1_MISO_PORT_RCU);
    gpio_mode_set(SPI1_MISO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, SPI1_MISO_PIN);
    gpio_output_options_set(SPI1_MISO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, SPI1_MISO_PIN);
    gpio_af_set(SPI1_MISO_PORT, SPI1_MISO_AF, SPI1_MISO_PIN);
#endif
    /*************** spi config **************/
    // 时钟
    rcu_periph_clock_enable(SPIx_RCU);

    spi_parameter_struct spi_init_struct;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    // negative slave select
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.clock_polarity_phase = SPI1_PARAM_CPOL_CPHA;
    spi_init_struct.prescale             = SPI1_PARAM_PSC;
    spi_init_struct.endian               = SPI1_PARAM_ENDIAN;
    spi_init(SPIx, &spi_init_struct);

    spi_init(SPIx, &spi_init_struct);
    
#if SPI1_DMA_TX_ENABLE
    dma_tx_init();
    
    spi_dma_enable(SPIx, SPI_DMA_TRANSMIT);
#endif

    spi_enable(SPIx);
}

#if SPI1_MOSI_ENABLE
void SPI1_write(uint8_t data) {
    while(RESET == spi_i2s_flag_get(SPIx, SPI_FLAG_TBE));
    //硬件电路，发送数据，放到寄存器
    spi_i2s_data_transmit(SPIx, data);

    while(RESET == spi_i2s_flag_get(SPIx, SPI_FLAG_RBNE));
    //硬件电路，发送数据，放到寄存器
    spi_i2s_data_receive(SPIx);
}
#endif
#if SPI1_MISO_ENABLE
uint8_t SPI1_read() {
    while(RESET == spi_i2s_flag_get(SPIx, SPI_FLAG_TBE));
    //硬件电路，发送数据，放到寄存器
    spi_i2s_data_transmit(SPIx, 0x00);

    while(RESET == spi_i2s_flag_get(SPIx, SPI_FLAG_RBNE));
    //硬件电路，发送数据，放到寄存器
    return spi_i2s_data_receive(SPIx);
}
#endif


#if SPI01_MOSI_ENABLE && SPI1_MISO_ENABLE
uint8_t SPI1_write_read(uint8_t dat) {
    while(RESET == spi_i2s_flag_get(SPIx, SPI_FLAG_TBE));
    //硬件电路，发送数据，放到寄存器
    spi_i2s_data_transmit(SPIx, dat);

    while(RESET == spi_i2s_flag_get(SPIx, SPI_FLAG_RBNE));
    //硬件电路，发送数据，放到寄存器
    return spi_i2s_data_receive(SPIx);
}
#endif



#endif
#endif
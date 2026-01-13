#include "SPI4.h"

#if USE_SPI4

#if SPI4_SOFT

//////////////////////////// 软实现 ///////////////////////////////////

#define SCL(bit)		gpio_bit_write(SPI4_SCL_PORT, SPI4_SCL_PIN, bit ? SET : RESET)

#define MOSI(bit)		gpio_bit_write(SPI4_MOSI_PORT, SPI4_MOSI_PIN, bit ? SET : RESET)

#define MISO()    	gpio_input_bit_get(SPI4_MISO_PORT,SPI4_MISO_PIN)//FS0
void SPI4_init(){
	// SCL
	// MOSI
	
	// gpio output -------------------------------------------------
	rcu_periph_clock_enable(SPI4_SCL_RCU);
	gpio_mode_set(SPI4_SCL_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, SPI4_SCL_PIN);
	gpio_output_options_set(SPI4_SCL_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, SPI4_SCL_PIN);
	
	rcu_periph_clock_enable(SPI4_MOSI_RCU);
	gpio_mode_set(SPI4_MOSI_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, SPI4_MOSI_PIN);
	gpio_output_options_set(SPI4_MOSI_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, SPI4_MOSI_PIN);
	
	// gpio input --------------------------------------------------
	rcu_periph_clock_enable(SPI4_MISO_RCU);
	gpio_mode_set(SPI4_MISO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, SPI4_MISO_PIN);
//	gpio_output_options_set(MISO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, MISO_PIN);

	// IO init
	SCL(1);
	MOSI(1);
	
	// CPOL 1
	// CPHA 1
	
}

void SPI4_write(uint8_t dat){
	// MSB大端模式
	// 1101 0011
	for(uint8_t i=0;i<8;i++)
	{			  
		// SCL拉低
		SCL(0);
		
		// 输出
		MOSI(dat&0x80);
		
		// 左移1位
		dat<<=1;   
		
		// SCL拉高
		SCL(1);
	}				 	
}

uint8_t SPI4_read(){
	// 大端模式
	//  0000 0000 <- 1101 0101
	
	uint8_t i,read=0;
	for(i=0;i<8;i++)
	{
		// SCL拉低
		SCL(0);
		
		read <<= 1;
		if(MISO()) read++;
		
		// SCL拉高
		SCL(1);
  }
	return read;
}

uint8_t SPI4_read_write(uint8_t dat){
	uint8_t i,read=0;
	for(i=0;i<8;i++)
	{
		// SCL拉低
		SCL(0);
		
		// -------------------------- 输出MOSI
		MOSI(dat&0x80);
		// 左移1位
		dat<<=1;   
		
		// -------------------------- 输入MISO
		read <<= 1;
		if(MISO()) read++;
		
		// SCL拉高
		SCL(1);
  }
	return read;
}
#else

//////////////////////////// 硬实现 ///////////////////////////////////


void SPI4_init(){
	// GPIO ------------------------------------
	// rcu: SCL, MOSI, MISO (CS 和设备业务挂钩， 不需要在这里配置)
	rcu_periph_clock_enable(SPI4_SCL_RCU);
	gpio_mode_set(SPI4_SCL_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, SPI4_SCL_PIN);
	gpio_output_options_set(SPI4_SCL_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, SPI4_SCL_PIN);
	gpio_af_set(SPI4_SCL_PORT,GPIO_AF_5,SPI4_SCL_PIN);
	
	rcu_periph_clock_enable(SPI4_MOSI_RCU);
	gpio_mode_set(SPI4_MOSI_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, SPI4_MOSI_PIN);
	gpio_output_options_set(SPI4_MOSI_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, SPI4_MOSI_PIN);
	gpio_af_set(SPI4_MOSI_PORT,GPIO_AF_5,SPI4_MOSI_PIN);
	
	rcu_periph_clock_enable(SPI4_MISO_RCU);
	gpio_mode_set(SPI4_MISO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, SPI4_MISO_PIN);
	gpio_af_set(SPI4_MISO_PORT,GPIO_AF_5,SPI4_MISO_PIN);
	
	// SPI -------------------------------------
	// rcu
	rcu_periph_clock_enable(RCU_SPI4);
	
	spi_parameter_struct spi_struct;
	/* initialize the parameters of SPI struct with default values */
  spi_struct_para_init(&spi_struct);
	/* configure the structure with default value */
	spi_struct.device_mode          = SPI_MASTER;								// 设备模式：主机 
	spi_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;	// 传输模式：default
	spi_struct.frame_size           = SPI_FRAMESIZE_8BIT;				// 每一帧的bit数: default
	spi_struct.nss                  = SPI_NSS_SOFT;							// 软片选
	spi_struct.clock_polarity_phase = SPI4_CLOCK_POLARITY_PHASE;	// CPOL=1, CPHA=1
	spi_struct.prescale             = SPI4_PRESCALE;								// 120M/2 分频系数，越大越慢！！！！
	spi_struct.endian               = SPI_ENDIAN_MSB;						// 默认：大端模式
	
	/* initialize SPI parameter */
	spi_init(SPI4, &spi_struct);
	/* enable SPI */
	spi_enable(SPI4);
}

void SPI4_write(uint8_t dat){
	/* 循环等待发送缓冲区, 直到为空 */
	while(RESET == spi_i2s_flag_get(SPI4, SPI_FLAG_TBE));
	/* 通知外设电路发数据 SPI transmit data */
	spi_i2s_data_transmit(SPI4, dat);
	
	/* 循环等待接收缓冲区，直到不为空 */
	while(RESET == spi_i2s_flag_get(SPI4, SPI_FLAG_RBNE));
	/* SPI receive data */
	spi_i2s_data_receive(SPI4);
}

uint8_t SPI4_read(){
	/* 循环等待发送缓冲区, 直到为空 */
	while(RESET == spi_i2s_flag_get(SPI4, SPI_FLAG_TBE));
	/* 通知外设电路发数据 SPI transmit data */
	spi_i2s_data_transmit(SPI4, 0x00);
	
	/* 循环等待接收缓冲区，直到不为空 */
	while(RESET == spi_i2s_flag_get(SPI4, SPI_FLAG_RBNE));
	/* SPI receive data */
	uint8_t dat = spi_i2s_data_receive(SPI4);
	return dat;
}

uint8_t SPI4_read_write(uint8_t dat){
	/* 循环等待发送缓冲区, 直到为空 */
	while(RESET == spi_i2s_flag_get(SPI4, SPI_FLAG_TBE));
	/* 通知外设电路发数据 SPI transmit data */
	spi_i2s_data_transmit(SPI4, dat);
	
	/* 循环等待接收缓冲区，直到不为空 */
	while(RESET == spi_i2s_flag_get(SPI4, SPI_FLAG_RBNE));
	/* SPI receive data */
	return spi_i2s_data_receive(SPI4);
}


#endif





#endif



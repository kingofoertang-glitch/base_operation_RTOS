#include "I2C.h"

void I2C_init(){

#if USE_I2C0
    I2C0_init();
#endif
    
#if USE_I2C1
    I2C1_init();
#endif
    
#if USE_I2C2
    I2C2_init();
#endif
}
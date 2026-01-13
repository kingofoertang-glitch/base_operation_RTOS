#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "USART0.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bsp_keys.h"

/************************
模板工程: 

串口中断(代表所有其他中断) 收到任何消息, 任务恢复 

- 开启Timer: 每隔1s打印一个日志Timer

1. task1: 每隔1s打印一个不断打印task1

2. 按键任务taskKey: 
    - PC0: 按下 关闭所有中断
    - PC1: 按下 开启所有中断 


中断的数字越小，优先级越高，范围【0，15】
任务的数字越大，优先级越高，范围【0，31】
*************************/


TaskHandle_t xStartTask_Handle;
TaskHandle_t xTaskKey_Handle;
TaskHandle_t xTask1_Handle;


void USART0_on_recv(uint8_t* data, uint32_t len){
    printf("recv[%d]-> %s\n", len, data);
     
}

void vTaskKey(){

    while(1){
        bsp_keys_scan();
        
        delay_1ms(10);     
        
        //vTaskDelay(10);  // 该函数内部会自动启用所有的中断, 有禁用操作后, 如果忘了启用，此函数会自动启用中断，使代码正常运行   
    }
    // 如果任务有可能结束, 一定要销毁自己
    vTaskDelete(NULL);
}

int8_t count = 1;

void vTask1(){
    uint32_t cnt = 0;
    while(1){
        printf("task1: %d\n", cnt++);  
        vTaskDelay(1000); 
    }
    // 如果任务有可能结束, 一定要销毁自己
    vTaskDelete(NULL);
}

void GPIO_init(){
    // 1. 时钟初始化
    rcu_periph_clock_enable(RCU_GPIOA);
    // 2. 配置GPIO 输入输出模式
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_0);
}

static void TIMER_config() {
    // 时钟配置
    rcu_periph_clock_enable(RCU_TIMER5);

    // 复位定时器
    timer_deinit(TIMER5);
    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);
    timer_parameter_struct tps;
    timer_struct_para_init(&tps);
    tps.prescaler = 5000 - 1; // 分频系数  168 000 000  分母 >= 2567
    tps.period = SystemCoreClock / 5000 / 1 - 1; // 周期

    timer_init(TIMER5, &tps);
    nvic_irq_enable(TIMER5_DAC_IRQn, 7, 0);
    
    timer_interrupt_enable(TIMER5, TIMER_INT_UP);
    timer_enable(TIMER5);
}

void TIMER5_DAC_IRQHandler(void) {
    if(SET == timer_interrupt_flag_get(TIMER5, TIMER_INT_FLAG_UP)) {
        //清除中断标志位
        timer_interrupt_flag_clear(TIMER5,TIMER_INT_FLAG_UP);
        
        printf("Timer5\n");
    }
}

void Keys_on_keydown(uint8_t key){

    
    switch (key)
    {
    	case 0: 
            printf("关闭中断\n");
            portDISABLE_INTERRUPTS();
            
    		break;
    	case 1: 
            printf("启用中断\n");
            portENABLE_INTERRUPTS();
            break;
    	case 2: 
            break;
    	default:
    		break;
    }
    
/*********************************
通过portDISABLE_INTERRUPTS()禁用终端， 会导致以下两个中断也被停止，   
    1. 系统滴答定时器（SysTick）中断
    2. PendSV中断（用于任务切换）
    
FreeRTOS无法进行任务调度。其他任务也无法正常运行和切换。
    
通常禁用中断要和启用中断连续使用，中间的操作尽可能少，不能间隔太久时间。    
*********************************/
}

void sys_init(){
    // 1. 初始化外设
    GPIO_init();
    USART0_init();
    TIMER_config();
    
    // hardware
    bsp_keys_init();
}


void vTaskFunc(uint8_t *pvParameters){
    // 1. 初始化外设 
    sys_init();
    
    printf("Init Complete!\n");
    
    // 进入临界区, 所有任务会被禁止切换，中断被屏蔽 ---------------------------
    taskENTER_CRITICAL();
    // a. 当前任务继续运行
    // b. 任务切换被禁止
    // c. 部分中断被屏蔽（优先级低于 configMAX_SYSCALL_INTERRUPT_PRIORITY ）
    
    // 2. 创建3个独立的任务
    xTaskCreate( vTaskKey,  "vTaskKey", 64, NULL, 3, &xTaskKey_Handle );
    xTaskCreate( vTask1,    "vTask1",   64, NULL, 4, &xTask1_Handle   );
        
    // 退出临界区, 所有任务允许切换 -------------------------------------------
    taskEXIT_CRITICAL();
    
    // 3. 删除当前任务 鲁棒性(健壮性)
    vTaskDelete(NULL); // xStartTask_Handle    
}


int main(void) {
    // 全局优先级分配规则：4抢占[0,15], 0响应
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);

    // 创建第一个任务(初始化外设, 创建其他任务)
    BaseType_t rst = xTaskCreate(
        (TaskFunction_t)   vTaskFunc,   // 任务函数的指针
        (const char *)   "task_func",   // 任务名称, 最大长度取决于 configMAX_TASK_NAME_LEN
        (uint16_t)               128,   // 任务栈大小, 单位: Word字(32位 4字节)  128 x 4字节, 任务越复杂所需空间越大
        (uint8_t *)             NULL,   // 任务函数参数, 通常使用NULL
        (UBaseType_t)              1,   // 任务的优先级, 数值越大, 优先级越高, 最大值为configMAX_PRIORITIES-1
        (TaskHandle_t * ) &xStartTask_Handle    // 任务句柄, 用于在需要时操作该任务
    ); 
    //  rst:  pdPASS创建成功，pdFAIL失败 (通常栈空间不足时,才会失败)
    
    // 开启任务调度
    vTaskStartScheduler();
  
    while(1);  
}

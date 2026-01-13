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
*************************/


TaskHandle_t xStartTask_Handle;
TaskHandle_t xTaskKey_Handle;
TaskHandle_t xTask1_Handle;
TaskHandle_t xTask2_Handle;


void USART0_on_recv(uint8_t* data, uint32_t len){
    printf("recv[%d]-> %s\n", len, data);
    
    if(xTask1_Handle == NULL) return;

    // 在中断里恢复任务， 要求当前【中断的优先级】要小于等于 ucMaxSysCallPriority 即数字要大一些
    // 要求 ucCurrentPriority >= ucMaxSysCallPriority 成立，参见配置 configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    
    BaseType_t xYieldRequired = xTaskResumeFromISR(xTask1_Handle);    
    
    // pdTRUE: 恢复任务后，需要进行上下文切换
    // 即: 恢复的【任务的优先级】 >= 当前运行任务的优先级
    // 当ISR退出时, 需要切换到这个更高优先级的任务
    portYIELD_FROM_ISR(xYieldRequired);  // 延迟到ISR退出时切换(taskYIELD()是立即切换)    
}



void vTaskKey(){

    while(1){
        bsp_keys_scan();
        vTaskDelay(10);        
    }
    // 如果任务有可能结束, 一定要销毁自己
    vTaskDelete(NULL);
}

int8_t count = 1;

void vTask1(){
    uint32_t cnt = 0;
    while(1){
        printf("task1: %d\n", cnt++);  // n Us
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
    tps.prescaler = 5000 - 1; // 分频系数  168 000 000
    tps.period = SystemCoreClock  / 5000 /1 - 1; // 周期

    timer_init(TIMER5, &tps);
    nvic_irq_enable(TIMER5_DAC_IRQn, 2, 2);
    timer_interrupt_enable(TIMER5, TIMER_INT_UP);
    timer_enable(TIMER5);
}


void TIMER5_DAC_IRQHandler(void) {
    if(SET == timer_interrupt_flag_get(TIMER5, TIMER_INT_FLAG_UP)) {
    	// TODO: 
        //清除中断标志位
        timer_interrupt_flag_clear(TIMER5,TIMER_INT_FLAG_UP);
			printf("Timer5\n");
    }
}


void Keys_on_keydown(uint8_t key){

//    printf("key -> %d\n", key);
    
    switch (key)
    {
    	case 0: // 挂起  (可以恢复)
          printf("KEY1\n");
    		break;
    	case 1: // 删除  (不可恢复)
          printf("KEY2\n");
            break;
    	case 2: // 恢复
        
			
            break;
    	default:
    		break;
    }
}

void sys_init(){
    // 1. 初始化GPIO
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
    xTaskCreate( vTaskKey,  "vTaskKey", 64, NULL, 1, &xTaskKey_Handle );
    xTaskCreate( vTask1,    "vTask1",   64, NULL, 4, &xTask1_Handle   );
 //   xTaskCreate( vTask2,    "vTask2",   64, NULL, 3, &xTask2_Handle   );
        
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

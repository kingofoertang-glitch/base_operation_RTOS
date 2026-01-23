#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "USART0.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bsp_keys.h"

#include "semphr.h"
#include "timers.h"
/************************
模板工程: 
3个独立的任务

1.task1:等待接受信号   优先级1   	500ms				
2,task2:等待接受信号		 优先级2		 3000ms

-----------------------------
创建互斥信号量(信号量)semphr.h
- 创建完就发送一个信号

*************************/


TaskHandle_t xStartTask_Handle;
TaskHandle_t xTask1_Handle;
TaskHandle_t xTask2_Handle;

SemaphoreHandle_t xSemaphore = NULL;



void USART0_on_recv(uint8_t* data, uint32_t len){

}





void vTask1(void *pvParameters) {
  BaseType_t result;
  while(1) {
    printf("task1	take 0\n");
    xSemaphoreTakeRecursive(xSemaphore, portMAX_DELAY);

    printf("task1	take 1\n");
    xSemaphoreTakeRecursive(xSemaphore, portMAX_DELAY);

    printf("task1	give\n");
    xSemaphoreGiveRecursive(xSemaphore);
		
    printf("task1	give\n");
    xSemaphoreGiveRecursive(xSemaphore);
    vTaskDelay(1000);
  }
}



void vTask2(void *pvParameters) {
  BaseType_t result;
  while(1) {
    printf("task2	take 0\n");
    xSemaphoreTakeRecursive(xSemaphore, portMAX_DELAY);

    printf("task2	take 1\n");
    xSemaphoreTakeRecursive(xSemaphore, portMAX_DELAY);

    printf("task2	give\n");
    xSemaphoreGiveRecursive(xSemaphore);
		
    printf("task2	give\n");
    xSemaphoreGiveRecursive(xSemaphore);

    vTaskDelay(1000);
  }
}
void GPIO_init(){
    // 1. 时钟初始化
    rcu_periph_clock_enable(RCU_GPIOA);
    // 2. 配置GPIO 输入输出模式
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_0);
}

void sys_init(){
    // 1. 初始化外设
    GPIO_init();
    USART0_init();
}



void vTaskFunc(uint8_t *pvParameters){
    // 1. 初始化外设 
    sys_init();
    
    printf("Init Complete!\n");

    
    // 创建互斥信号量（自带1个信号）
    xSemaphore = xSemaphoreCreateRecursiveMutex();
    if(xSemaphore != NULL){    
        printf("semaphore was created successfully.\n");
    }    
    
    // 进入临界区, 所有任务会被禁止切换，中断被屏蔽 ---------------------------
    taskENTER_CRITICAL();
    
    // 2. 创建1个独立的任务
    xTaskCreate( vTask1,    "vTask1",   64, NULL, 2, &xTask1_Handle   );
    xTaskCreate( vTask2,    "vTask2",   64, NULL, 3, &xTask2_Handle   );

        
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

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

1.按键任务:PA0按下,发送信号
2,task1:等待接受信号
3,task2:等待接受信号

-----------------------------
创建二值信号量(二进制信号量)semphr.h
-task1:等待信号
-taskKey:发送信号

*************************/


TaskHandle_t xStartTask_Handle;
TaskHandle_t xTaskKey_Handle;
TaskHandle_t xTask1_Handle;
TaskHandle_t xTask2_Handle;

SemaphoreHandle_t xSemaphore = NULL;

TimerHandle_t  timer1_handle;
TimerHandle_t  timer2_handle;

void USART0_on_recv(uint8_t* data, uint32_t len){
    
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	
		xSemaphoreGiveFromISR(xSemaphore,&xHigherPriorityTaskWoken);
	
	printf("recv[%d]-> %s\n -> woken: %ld\n", len, data,xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void vTaskKey(){

		FlagStatus pre_state =RESET;
		BaseType_t xReturn;
    while(1){
        FlagStatus cur_state =gpio_input_bit_get(GPIOA,GPIO_PIN_0);
				if(pre_state !=cur_state){
					pre_state = cur_state;
					
					if(cur_state == SET){
						
						printf("Give: %ld\n",xReturn);
						xSemaphoreGive(xSemaphore);
					}
				}
        vTaskDelay(10);  // 该函数内部会自动启用所有的中断, 有禁用操作后, 如果忘了启用，此函数会自动启用中断，使代码正常运行   
    }
    // 如果任务有可能结束, 一定要销毁自己
    vTaskDelete(NULL);
}

int8_t count = 1;

void vTask1(){
    uint32_t cnt = 0;
		BaseType_t xReturn =pdFALSE; //pdTRUE
    while(1){
		//阻塞等待,直到获取到信号
			xSemaphoreTake(xSemaphore,portMAX_DELAY);//		pdMS_TO_TICKS(2000);
      printf("task1: %d count: %ld\n", cnt++,uxSemaphoreGetCount(xSemaphore));   
			vTaskDelay(pdMS_TO_TICKS(500));
		}
}


void vTask2(){
    uint32_t cnt = 0;

    BaseType_t xReturn = pdFALSE; // pdTRUE
    while(1){
        // 阻塞等待，直到获取到信号（一直等）
        printf("task2 waiting...\n");
        xReturn = xSemaphoreTake(xSemaphore, portMAX_DELAY); //portMAX_DELAY, pdMS_TO_TICKS(2000)
        
        printf("task2: %d xReturn: %ld\n",cnt++,uxSemaphoreGetCount(xSemaphore));  // n Us
        
        vTaskDelay(pdMS_TO_TICKS(3000));
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

	  // 创建计数型信号量
    const UBaseType_t uxMaxCount = 5, uxInitialCount = 3;
    // uxMaxCount 最大的消息数量
    // uxInitialCount 初始已有的消息个数
    xSemaphore = xSemaphoreCreateCounting(uxMaxCount, uxInitialCount);
    if(xSemaphore != NULL){
    
        printf("semaphore was created successfully.\n");
    }
		
		//进入临界区,所有任务都会被禁止切换,中断被屏蔽---
		taskENTER_CRITICAL();

    // 2. 创建1个独立的任务
    xTaskCreate( vTaskKey,  "vTaskKey", 64, NULL, 1, &xTaskKey_Handle );
    xTaskCreate( vTask1,    "vTask1",   64, NULL, 2, &xTask1_Handle   );
    xTaskCreate( vTask2,  	"vTask2", 	64, NULL, 3, &xTaskKey_Handle );

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

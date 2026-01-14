#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "USART0.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bsp_keys.h"

#include "timers.h"
/************************
模板工程: 


创建FreeRTOS的Timer定时器, 每秒打一个日志

按键任务: 扫描4个独立按键
按键1: 停止Timer
按键2: 启用Timer
按键3: 删除Timer


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


TimerHandle_t  timer1_handle;
TimerHandle_t  timer2_handle;

void Keys_on_keydown(uint8_t key){


    switch (key)
    {
    	case 0: 
            printf("停止Timer\n");
						if(timer2_handle != NULL){
						xTimerStop(timer2_handle, 0); // pdMS_TO_TICKS
						}
    		break;
    	case 1: 
            printf("启用Timer\n");
						if(timer2_handle != NULL){
							xTimerStart(timer2_handle,0);
            }
						break;
    	case 2: 
						printf("删除Timer\n");
					
						if(timer2_handle != NULL){
								xTimerDelete(timer2_handle,0);
								timer2_handle = NULL;
						}		
						break;
    	default:
    		break;
    }
	}    

void sys_init(){
    // 1. 初始化外设
    GPIO_init();
    USART0_init();

    // hardware
    bsp_keys_init();
}



uint32_t timer1_cnt = 0;
uint32_t timer2_cnt = 0;

void timer_cb(TimerHandle_t pxTimer){
    int timerId = (int)pvTimerGetTimerID(pxTimer);

    if(timerId == 1){
        printf("timer1: %d\n", timer1_cnt++);
    }else if(timerId == 2){
        printf("timer2: %d\n", timer2_cnt++);
    }
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

		//创建Timer
#if 0

		TimerHandle_t xTimerCreate( const char * const pcTimerName, /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
				const TickType_t xTimerPeriodInTicks,
				const BaseType_t xAutoReload,
				void * const pvTimerID,
				TimerCallbackFunction_t pxCallbackFunction )

#endif
		//创建Timer,间隔1000ms
 timer1_handle = xTimerCreate( "Timer1",
        pdMS_TO_TICKS(1000),    // 间隔时间(周期), 单位ticks, 通过宏把ms转成ticks
        pdFALSE,                 // 是否自动重载
        (void*) 1,              // 定时器 Id
        timer_cb
    );		


		//创建Timer,间隔1000ms
 timer2_handle = xTimerCreate( "Timer2",
        pdMS_TO_TICKS(500),    // 间隔时间(周期), 单位ticks, 通过宏把ms转成ticks
        pdTRUE,                 // 是否自动重载
        (void*) 2,              // 定时器 Id
        timer_cb
    );
			//启用Timer
				//参数2:  xTicksToWait:当Timer任务队列已满时,最大等待时间为0
				xTimerStart(timer1_handle,0);
  			xTimerStart(timer2_handle,0);



    // 2. 创建1个独立的任务
    xTaskCreate( vTaskKey,  "vTaskKey", 64, NULL, 1, &xTaskKey_Handle );
//    xTaskCreate( vTask1,    "vTask1",   64, NULL, 4, &xTask1_Handle   );

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

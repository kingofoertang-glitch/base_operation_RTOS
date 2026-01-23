
#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

/* Ensure stdint is only used by the compiler, and not the assembler. */
#include <stdio.h>

#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
	#include <stdint.h>
	extern uint32_t SystemCoreClock;
#endif

/* 带默认值参数 */
#define	configSUPPORT_DYNAMIC_ALLOCATION				1							  /* 支持动态创建任务，默认为1 */
#define configSUPPORT_STATIC_ALLOCATION   				0   						  /* 1: 支持静态申请内存, 默认: 0 */

#define configUSE_PREEMPTION							1							  /* 为 1 时使用抢占式调度器，为 0 时使用协程 */
#define configUSE_IDLE_HOOK								0							  /* 为 1 时使用空闲任务钩子函数，用户需要实现空闲任务钩子函数 */
#define configUSE_TICK_HOOK								0							  /* 为 1 时使能时间片钩子函数，用户需要实现时间片钩子函数 */
#define configCPU_CLOCK_HZ								( SystemCoreClock )			  /* CPU主频 */
#define configTICK_RATE_HZ								( ( TickType_t ) 1000 )		  /* 系统时钟节拍频率：1秒中断的次数 */
#define configMAX_PRIORITIES							( 32 )						  /* 最大优先级数(Max: 32)：最大优先级为configMAX_PRIORITIES - 1 */
#define configMINIMAL_STACK_SIZE						( ( unsigned short ) 130 )	  /* 空闲任务的栈大小：单位（Word字），字节数130x4 */
#define configTOTAL_HEAP_SIZE							( ( size_t ) ( 75 * 1024 ) )  /* 任务堆大小75KByte */
#define configMAX_TASK_NAME_LEN							( 16 )						  /* 任务名最大字符数量：默认16 */
#define configUSE_TRACE_FACILITY						1							  /* 为 1 启用可视化跟踪调试 */
#define configUSE_16_BIT_TICKS							0							  /* 系统时钟节拍计数器数据类型:1表示16bit无符号整型，0表示32bit无符号整型 */
#define configIDLE_SHOULD_YIELD							1                             /* 为 1，说明空闲任务需要让出时间片给同优先级的其他就绪任务。*/
#define configUSE_MUTEXES								1                             /* 为 1 时使用互斥信号量，相关的 API 函数会被编译。*/
#define configQUEUE_REGISTRY_SIZE						8                             /* 设置可以注册的队列和信号量的最大数量，在使用内核调试器查看信号量和队列时需要设置此宏*/
#define configCHECK_FOR_STACK_OVERFLOW					0                             /* 设置堆栈溢出检测规则 */
#define configUSE_RECURSIVE_MUTEXES						1                             /* 为 1 时使用递归互斥信号量，相关的 API 函数会被编译。 */
#define configUSE_MALLOC_FAILED_HOOK					0                             /* 为 1 时使用内存分配失败钩子函数，用户需要实现内存分配失败钩子函数 */
#define configUSE_APPLICATION_TASK_TAG					0                             /* 为 1 时configUSE_APPLICATION_TASK_TAGF() 和xTaskCallApplicationTaskHook()会被编译*/
#define configUSE_COUNTING_SEMAPHORES					1                             /* 设置为 1 的时候启用计数型信号量，相关的 API 函数会被编译。*/
#define configGENERATE_RUN_TIME_STATS					0                             /* 为 1 时开启时间统计功能，相应的 API 函数会被编译。 */


/* Software timer definitions. */
#define configUSE_TIMERS								1									/* 使能定时器 */
#define configTIMER_TASK_PRIORITY				  		( 2 )								/* 守护任务的优先级, 尽可能高一些 */
#define configTIMER_QUEUE_LENGTH						10									/* Timer命令队列长度 */
#define configTIMER_TASK_STACK_DEPTH					( configMINIMAL_STACK_SIZE * 2 )	/* 守护任务的栈大小 */

/* Set the following definitions to 1 to include the API function, 
or zero to exclude the API function. */
#define INCLUDE_vTaskPrioritySet			            1	/* 设置任务优先级 */
#define INCLUDE_uxTaskPriorityGet			            1	/* 获取任务优先级 */
#define INCLUDE_vTaskDelete					            1	/* 删除任务 */
#define INCLUDE_vTaskCleanUpResources		            1   /* 清理资源 */
#define INCLUDE_vTaskSuspend				            1	/* 挂起任务 */
#define INCLUDE_xTaskResumeFromISR    		            1   /* 恢复在中断中挂起的任务 */                
#define INCLUDE_vTaskDelayUntil				            1	/* 延时直到 */
#define INCLUDE_vTaskDelay					            1   /* 延时 */

/* Cortex-M specific definitions. */
#ifdef __NVIC_PRIO_BITS
	/* __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
	#define configPRIO_BITS       		__NVIC_PRIO_BITS
#else
	#define configPRIO_BITS       		4        /* 15 priority levels */
#endif

/* The lowest interrupt priority that can be used in a call to a "set priority"
function. */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY			0x0f

/* The highest interrupt priority that can be used by any interrupt service
routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT CALL
INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
PRIORITY THAN THIS! (higher priorities are lower numeric values. */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	5

/* Interrupt priorities used by the kernel port layer itself.  These are generic
to all Cortex-M ports, and do not rely on any particular library functions. */
#define configKERNEL_INTERRUPT_PRIORITY 		( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/* Normal assert() semantics without relying on the provision of an assert.h
header file. */
/* 断言结果通过printf输出 */
#define vAssertCalled(char, int) printf("-> Error: %s, %d\r\n", char, int)
#define configASSERT( x ) if( ( x ) == 0 ) vAssertCalled( __FILE__, __LINE__ )
	
//#define configASSERT( x ) if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for( ;; ); }
//#define configASSERT( x ) 0
/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
standard names. */

#define vPortSVCHandler SVC_Handler
#define xPortPendSVHandler PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

#endif /* FREERTOS_CONFIG_H */
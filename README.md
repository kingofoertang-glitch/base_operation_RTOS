# FreeRTOS入门和基本操作

## 下载源代码

1. FreeRTOS源码地址为: https://github.com/FreeRTOS/FreeRTOS/releases
2. 可以直接在官网下载：https://freertos.org/

## 准备开发环境

- 拷贝Source目录源代码
- 裁剪代码, 保留`portable`目录只保留`GCC`和`MemMang`, `RVDS`目录
- 创建三个Group
  - FreeRTOS/Core: 根目录的所有c文件
  - FreeRTOS/Port:  存放portable里的heap_4.c和port.c
  - FreeRTOS : 存放配置文件FreeRTOSConfig.h
- 添加需要的include目录

## 管理任务

### 创建任务

```c
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
```

在第一个任务里初始化外设, 其他任务

```c
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
    xTaskCreate( vTask2,    "vTask2",   64, NULL, 3, &xTask2_Handle   );
        
    // 退出临界区, 所有任务允许切换 -------------------------------------------
    taskEXIT_CRITICAL();
    
    // 3. 删除当前任务 鲁棒性(健壮性)
    vTaskDelete(NULL); // xStartTask_Handle    
}
```

### 任务的挂起, 恢复, 删除

```c
// 挂起
vTaskSuspend(xTask1_Handle);
// 恢复
vTaskResume(xTask1_Handle);   
// 删除
vTaskDelete(xTask1_Handle); // 重复删除才会报错
```

### 在中断里恢复任务

```c
BaseType_t xYieldRequired = xTaskResumeFromISR(xTask1_Handle);    

// pdTRUE: 恢复任务后，需要进行上下文切换
// 即: 恢复的【任务的优先级】 >= 当前运行任务的优先级
// 当ISR退出时, 需要切换到这个更高优先级的任务
portYIELD_FROM_ISR(xYieldRequired);  // 延迟到ISR退出时切换(taskYIELD()是立即切换)    
```

## 操作系统里的睡眠

```c
vTaskDelay(1000)
```

# 
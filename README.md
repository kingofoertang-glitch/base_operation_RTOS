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

# ------------------------------------------------------------------------------------

# FreeRTOS进阶

## FreeRTOS管理中断

- 中断要求抢占优先级[5, 15]: 否则无法被管理, 也无法在中断函数里执行FreeRTOS的API

- 使用要的禁用和启用要放在一起, 中间的操作尽可能短

  ```c
  printf("关闭中断\n");
  portDISABLE_INTERRUPTS();
  // 执行一些寄存器或变量的操作(此阶段不会被中断或其他任务打断)
  
  printf("启用中断\n");
  portENABLE_INTERRUPTS();
  ```

## FreeRTOS软件定时器

- 创建Timer

```c
void timerCallback(TimerHandle_t xTimer) {
    printf("timer\r\n");
}

timer1 = xTimerCreate("Timer1", pdMS_TO_TICKS(1000), pdTRUE, (void *)1, timerCallback);
```

- Timer的基本操作

```c
// 启动
xTimerStart(xTimer, xTicksToWait);
// 在中断中启动Timer，需要特殊的函数
xTimerStartFromISR(timer2_handle, pxHigherPriorityTaskWoken);

// 停止
xTimerStop(xTimer, xTicksToWait);

// 删除
xTimerDelete(xTimer, xTickToWait);
```

## 信号量

### 二值信号量

```c
// 创建
sema_handler = xSemaphoreCreateBinary()
// 等待
xSemaphoreTake(sema_handler, portMAX_DELAY)
// 给出
xSemaphoreGive(sema_handler);
```

### 计数型信号量

```c
// 创建: 参数指定最大消息数量和初始的信号个数
sema_handler = xSemaphoreCreateCounting(10, 6);
// 等待
xSemaphoreTake(sema_handler, portMAX_DELAY);
// 给出
xSemaphoreGive(sema_handler);
// 获取剩余的信号数量
uxSemaphoreGetCount(sema_handler);
```

### 互斥信号量

与二进制信号量不同 `互斥锁`采用**优先继承**。 这意味着，如果高优先级任务在尝试获取当前由较低优先级任务持有的互斥锁（令牌）时阻塞， 则持有令牌的任务的**优先级会暂时提高到阻塞任务的优先级**。 这一机制 旨在确保较高优先级的任务保持阻塞状态的时间尽可能短， 从而最大限度地**减少已经发生的“优先级反转”**现象。

相关API

```c
// 创建互斥信号量（自带1个信号）
xSemaphore = xSemaphoreCreateMutex();
if(xSemaphore != NULL){    
    printf("semaphore was created successfully.\n");
}    
// 阻塞等待
xSemaphoreTake(xSemaphore, portMAX_DELAY);
// 给出信号
xSemaphoreGive(xSemaphore);  
```

### 递归互斥信号量

用户可对一把递归互斥锁重复加锁Take。但是需要释放Give同样次数的锁, 其他任务才能获取并加锁.

```c
// 创建
sema_handler = xSemaphoreCreateRecursiveMutex();

// 获取2次信号(加锁)
printf("task2	take 0\n");
xSemaphoreTakeRecursive(sema_handler, portMAX_DELAY);
printf("task2	take 1\n");
xSemaphoreTakeRecursive(sema_handler, portMAX_DELAY);

// 释放2次信号(解锁)
printf("task2	give\n");
xSemaphoreGiveRecursive(sema_handler);
printf("task2	give\n");
xSemaphoreGiveRecursive(sema_handler);
```

# **消息队列🌟🌟🌟**

### 队列

**队列**（Queue）是一种数据结构，用于存储和管理元素的线性集合。它遵循先进先出（FIFO，First-In-First-Out）的原则，即最先进入队列的元素将首先被移出队列。

队列通常具有两个基本操作：

1. 入队（Enqueue）：将元素添加到队列的末尾。新元素进入队列后成为新的队尾。
2. 出队（Dequeue）：从队列的头部移除并返回元素。被移除的元素为队列中存在时间最长的元素，即最先入队的元素。

![060.png](https://cdn.nlark.com/yuque/0/2023/png/27903758/1688130483003-3c9756fa-d327-41bd-a04a-68df4bdedf02.png?x-oss-process=image%2Fformat%2Cwebp)

### 消息队列

![061.png](https://cdn.nlark.com/yuque/0/2023/png/27903758/1688130696125-4e7363d0-00fd-4872-9292-32df15383f24.png?x-oss-process=image%2Fformat%2Cwebp)

### 功能介绍

基础API如下

| 功能          | 描述             |
| ------------- | ---------------- |
| xQueueCreate  | 创建队列         |
| xQueueSend    | 向队列中添加元素 |
| xQueueReceive | 从队列中取出元素 |

#### 创建队列

```c
QueueHandle_t xQueueCreate(const UBaseType_t uxQueueLength,
                        	const UBaseType_t uxItemSize);
```

参数说明：

1. `const UBaseType_t uxQueueLength`消息队列的长度
2. `const UBaseType_t uxItemSize`队列中单个元素的所占字节数。

返回值：

`QueueHandle_t`为消息队列的句柄。

#### 入队

```c
BaseType_t xQueueSend( QueueHandle_t xQueue,
                    	const void * const pvItemToQueue,
                        TickType_t xTicksToWait);
```

参数说明：

1. `QueueHandle_t xQueue`：队列句柄。
2. `const void * const pvItemToQueue`：数据。
3. `TickType_t xTicksToWait`：入队列等待时间。

返回值:

```
BaseType_t`：队列是否成功或者失败，`pdTrue`或者`pdFalse
```

#### 出队

```c
BaseType_t xQueueReceive( QueueHandle_t xQueue,
                          void * const pvBuffer,
                          TickType_t xTicksToWait);
```

参数说明：

1. `QueueHandle_t xQueue`：队列句柄。
2. `void * const pvBuffer`：数据。
3. `TickType_t xTicksToWait`：出队列等待时间。

返回值:

```
BaseType_t`：出队列是否成功或者失败，`pdTrue`或者`pdFalse
```



### 基本类型数据

构建时定义，定义数据的长度。

```c
QueueHandle_t queue = xQueueCreate(128, sizeof(uint8_t));
```

入队列操作

```c
uint8_t cnt = 0;
xQueueSend(queue, &cnt, portMAX_DELAY);
cnt++;
```

出队列操作

```c
uint8_t data;
xQueueReceive(queue, &data, portMAX_DELAY);
printf("queue recv: %d\r\n", data);
```

### 复杂类型数据

通常复杂类型用`struct`表示。

例如可以定义一个结构体：

```c
typedef struct {
	char buffer[64];
	uint32_t len;
} MyData_t;
```

构建时定义，定义数据的长度。

```c
QueueHandle_t queue = xQueueCreate(128, sizeof(MyData_t));
```

入队列操作

```c
MyData_t data;

char strbuffer[100];
sprintf(strbuffer, "struct_%d", queue_num);
data.len = strlen(strbuffer);
strcpy(data.buffer, strbuffer);
//data.buffer[data.len] = '\0';
xQueueSend(queue, &data, portMAX_DELAY);
```

出队列操作

```c
MyData_t data;
xQueueReceive(queue, &data, portMAX_DELAY);
printf("queue recv: %d %s\r\n", data.len, data.buffer);
```

### 信号量和消息队列

1. 从数据结构上进行比较。

消息队列是一种数据结构，用于在任务之间传递和共享数据或消息。它可以存储多个消息，并且可以按照先进先出（FIFO）的顺序进行访问。

信号量是一种同步机制，用于控制对共享资源的访问和任务之间的同步。它可以表示可用资源的数量或资源的占用状态。

1. 从操作方式上进行比较。

消息队列可以存储和获取多个消息。

信号量只有获取和释放两种操作。

1. 从特性上进行比较。

消息队列可以按照先进先出的顺序处理消息。

信号量只表示资源的可用状态或占用状态。



|            | **信号量**                                             | **队列**                          |
| ---------- | ------------------------------------------------------ | --------------------------------- |
| **初始化** | 仅有计数值，信号量结构体                               | 可存储队列项目，队列结构体        |
| **Input**  | Give信号量：计数值++，不阻塞计数值已经最大时，返回Fail | Send队列项，队列满时，**可阻塞**  |
| **Output** | Take信号量：计数值--计数值为0时，**可阻塞**            | Receive队列，队列空时，**可阻塞** |



注意，函数内部如果有多层函数调用， 或者变量字节数较大，会导致栈溢出，可调整任务最大的栈大小。

例如：将64改成128

​    xTaskCreate( vTask2,    "vTask2",  64, NULL, 3, &xTask2_Handle   );改为

​    xTaskCreate( vTask2,    "vTask2",  128, NULL, 3, &xTask2_Handle   )

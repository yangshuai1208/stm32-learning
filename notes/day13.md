# Day13 学习笔记：FreeRTOS 基础任务 + OLED 固定显示

## 今天做了什么
今天重新整理了第二阶段第13天的工程，不再继续在混乱版本上修补，而是从稳定的 FreeRTOS 基础工程重新出发，逐步把 OLED 接入到任务系统中。

## 今天完成的内容
1. 确认主函数只负责初始化和启动调度器
2. 理解 `MX_FREERTOS_Init()` 的作用是创建任务
3. 理解 `osKernelStart()` 的作用是启动调度器
4. 在 `freertos.c` 中创建两个任务：
   - `ledTask`
   - `uartTask`
5. 在 `ledTask` 中实现 OLED 固定字符串显示
6. 在 `uartTask` 中实现串口周期输出和 LED 翻转

## 今天的核心知识点

### 1. FreeRTOS 程序运行流程
主函数的主要流程是：

- 初始化 HAL
- 配置系统时钟
- 初始化外设
- 调用 `MX_FREERTOS_Init()` 创建任务
- 调用 `osKernelStart()` 启动调度器

任务真正执行的地方不是 `main()` 里的 `while(1)`，而是在各个任务函数中。

### 2. 任务函数才是真正运行的业务逻辑
今天最关键的理解是：

- `main()` 只是启动入口
- `StartLEDTask()`、`StartUartTask()` 才是任务真正执行的位置

### 3. 为什么要先做最小系统
前面排错时发现，一上来把 I2C、OLED、DHT11、多个任务一起加进去，很容易不知道问题到底出在哪。  
所以今天重新开始时，先从“最小可运行的 FreeRTOS 工程”入手，再逐步接回 OLED。

### 4. OLED 固定显示的意义
今天 OLED 先不显示动态数据，也不显示传感器值，只显示固定字符串。  
这样做的目的不是功能复杂，而是先验证：

- I2C 通信是否正常
- OLED 初始化是否成功
- 显示函数是否可用
- FreeRTOS 任务中能否正常调用 OLED 驱动

### 5. 串口心跳任务的意义
串口任务的作用不仅是打印信息，更重要的是：

- 作为系统运行状态的可视化反馈
- 辅助判断调度器是否正常工作
- 辅助判断任务是否真的在执行

## 今天的实验现象
程序下载后，观察到：

- OLED 显示固定字符串
- 串口助手周期输出 `RTOS uart ok`
- LED 周期翻转

这些现象说明：

- FreeRTOS 调度器已经正常启动
- 任务创建成功
- OLED 基础显示链路已经打通
- 串口调试链路正常

## 今天遇到的问题
1. 程序下载后完全没有现象
2. OLED 初始化有可能卡住
3. 一开始任务过多，不容易定位问题
4. 头文件包含路径风格不统一，容易编译报错
5. 对“谁在运行程序”这个问题理解不清晰

## 今天是怎么解决的
- 先从最小 FreeRTOS 版本做验证
- 保留最少任务数量
- 先确认串口任务能跑
- 再把 OLED 显示挂到任务里
- 用实验现象判断问题层级，而不是盲猜

## 今日收获
今天最大的收获不是多学了一个新外设，而是建立了更清晰的工程思路：

- 主函数负责启动系统
- 任务函数负责执行逻辑
- 调试时先最小化，再逐步恢复模块
- 遇到“没有现象”时，不要直接怀疑硬件，要先拆分路径逐个验证

## 明天继续的方向
在今天这版稳定工程基础上，后面可以继续做：

1. 在 FreeRTOS 任务中接回 DHT11
2. 让 OLED 显示动态数据
3. 再继续做任务通信，比如队列 Queue

## 今日总结
第13天重新开始以后，重点已经从“堆功能”变成“搭框架”。  
今天把 FreeRTOS 基础任务系统重新跑稳，并把 OLED 成功挂接到任务里，这是后续多任务项目能继续推进的前提。




1. 真正决定实验现象的是任务，不是 OLED_Init()
在原程序里，main() 只做了外设初始化和 OLED_Init()，并没有在这里直接显示字符串。真正显示 STM32 / FreeRTOS / OLED OK 和发送串口的是两个 RTOS 任务。

看这里：
main.c 先 MX_FREERTOS_Init()，再 osKernelStart()
freertos.c StartLEDTask() 里才写 OLED
freertos.c StartUartTAsk() 里才发串口

所以原程序一旦 RTOS 没真正跑起来，你就会同时看到：

OLED 不显示
串口不发送
2. 原来的 FreeRTOS 堆太小，这是最核心的问题
原来 configTOTAL_HEAP_SIZE 是 3072，现在我改成了 8192：
FreeRTOSConfig.h

而你原来申请的任务栈是：

defaultTask：128
ledTask：256
uartTask：256
位置在：
freertos.c
freertos.c
freertos.c

这些值在 FreeRTOS 里是“栈深度”，对 F1 来说基本按 4 字节算。光任务栈就已经接近把 3072 用满了，再加上：

idle task 的栈
每个任务的 TCB
FreeRTOS 内部开销
原来的堆配置其实很容易导致 osThreadCreate() 失败。
一旦线程创建失败，就直接进 Error_Handler()，后面当然什么现象都没有。

这也是为什么我后来加了这些错误提示：
freertos.c
freertos.c
freertos.c

3. 原来的 OLED 探测方式不够稳
原来你的 OLED_CheckDevice() 是这样做探测的：

uint8_t dummy = 0;
HAL_I2C_Master_Transmit(&hi2c1, (slave_addr << 1), &dummy, 0, IIC_TIMEOUT)
现在改成了：
User/oled/bsp_i2c_oled.c

HAL_I2C_IsDeviceReady(...)
区别是：

原来是“拿 0 字节发送去碰运气试探”
现在是“用 HAL 专门的设备应答检测”
这个改动让 OLED 在线判断更可靠，而且我还顺手加了 0x3C/0x3D 两个地址探测：
User/oled/bsp_i2c_oled.c

它不是原程序唯一的问题，但确实是一个隐患。

4. 原来的 ledTask 栈也偏紧
原来 ledTask 只有 256，现在我改成了 512：
freertos.c

原因是 OLED 刷屏过程中会走到 OLED_WriteBuffer()，原来这里还有一个较大的局部缓冲区。任务栈太小的话，任务一跑就有爆栈风险。
我后来把这里也收紧了：
User/oled/bsp_i2c_oled.c

一句话总结
原程序最主要的错误不是“某一行 OLED 命令写错了”，而是：

RTOS 可用堆太小，任务大概率起不来
OLED 和串口现象都依赖任务运行，所以看起来像“整机没反应”
同时 OLED 在线检测和任务栈配置也偏危险





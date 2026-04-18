# Day13 FreeRTOS + OLED 基础显示实验

## 一、实验目标
在 STM32F103 平台上重新搭建一个稳定的 FreeRTOS 基础工程，实现以下两个目标：

1. FreeRTOS 任务能够正常创建和运行
2. 在任务中完成 OLED 固定字符串显示与串口心跳输出

## 二、实验内容
本次实验完成了以下内容：

- 初始化 GPIO
- 初始化 USART1 串口
- 初始化 I2C1
- 初始化 OLED 显示屏
- 创建 FreeRTOS 任务
- 在 OLED 任务中显示固定字符串
- 在串口任务中周期输出调试信息
- 使用 LED 翻转作为任务运行指示

## 三、工程结构说明
本实验主要由以下部分组成：

- `main.c`
  - 完成系统时钟配置
  - 初始化 GPIO、USART1、I2C1
  - 调用 `MX_FREERTOS_Init()`
  - 调用 `osKernelStart()`

- `freertos.c`
  - 创建 `ledTask`
  - 创建 `uartTask`
  - `ledTask` 负责 OLED 固定显示
  - `uartTask` 负责串口打印和 LED 翻转

- `bsp_i2c_oled.c / bsp_i2c_oled.h`
  - OLED 底层驱动
  - OLED 初始化
  - 写命令、写数据、显示字符串

- `bsp_fonts.c / bsp_fonts.h`
  - ASCII 字库
  - 中文字库

## 四、硬件连接
### 1. OLED（I2C）
- SCL -> PB6
- SDA -> PB7
- VCC -> 3.3V
- GND -> GND

### 2. 串口
- USART1_TX -> PA9
- USART1_RX -> PA10
- 波特率 -> 115200

### 3. LED
- 使用开发板板载 LED 作为任务运行指示

## 五、程序流程
1. 上电后初始化 HAL 库
2. 配置系统时钟
3. 初始化 GPIO
4. 初始化 USART1
5. 初始化 I2C1
6. 初始化 OLED
7. 创建 FreeRTOS 任务
8. 启动调度器
9. `ledTask` 周期刷新 OLED 固定显示内容
10. `uartTask` 周期打印串口信息，并翻转 LED

## 六、任务设计
### 1. ledTask
功能：
- 清屏
- 显示固定字符串：
  - `STM32`
  - `FreeRTOS`
  - `OLED OK`

周期：
- 1000ms

### 2. uartTask
功能：
- 串口输出运行状态
- 翻转 LED，作为任务心跳

周期：
- 1000ms

## 七、实验现象
程序下载后，出现以下现象：

- OLED 屏幕显示固定字符串：
  - `STM32`
  - `FreeRTOS`
  - `OLED OK`
- 串口助手周期输出：
  - `RTOS uart ok`
- LED 周期翻转，表明任务正在运行

## 八、关键知识点
通过本实验，主要学习了以下内容：

- FreeRTOS 工程的基本结构
- `MX_FREERTOS_Init()` 与 `osKernelStart()` 的作用
- 任务创建与任务入口函数的关系
- FreeRTOS 中任务周期执行的基本方式
- I2C OLED 的基础接入方法
- 用最小任务系统逐步扩展功能的调试思路

## 九、遇到的问题
本次实验中遇到的主要问题有：

- 程序下载后没有实验现象
- OLED 初始化中设备检测可能导致程序卡死
- FreeRTOS 与 OLED 同时接入时不容易定位问题
- 头文件引用路径不统一导致编译报错
- 任务过多时排错困难

## 十、解决思路
- 先从最小 FreeRTOS 工程开始验证
- 先验证串口任务，再接入 OLED
- 将 OLED 从系统中临时移除，逐步定位问题
- 统一头文件包含方式
- 保留最小任务数量，减少干扰项

## 十一、实验总结
第13天的重点不是继续堆外设，而是把 FreeRTOS 基础任务系统重新搭稳，并成功挂接 OLED 模块。  
通过这次实验，我重新梳理了：

- 主函数负责初始化和启动调度器
- 任务函数负责真正业务逻辑
- 调试时应先验证最小系统，再逐步加模块

这一步为后续继续接入 DHT11、队列通信、多任务协作打下了基础。

# Day19 系统稳定性测试与异常观察 v1

## 一、项目目标

本项目基于前几天已经完成的 FreeRTOS + DHT11 + OLED + 串口输出系统，继续进行稳定性测试和异常观察整理。

第19天的重点不是继续新增外设，而是让当前系统：

- 结构更稳定
- 数据流更清晰
- 运行状态更容易观察
- 异常情况更容易判断

## 二、今天完成的内容

今天主要完成了以下内容：

- 保留三任务架构
- 保留共享结构体 `SensorData`
- 保留消息结构体 `SensorMessage`
- 使用长度为 1 的 FreeRTOS 队列传递最新采样结果
- `sensorTask` 负责采集 DHT11 并写入队列
- `displayTask` 负责 OLED 显示当前状态
- `uartTask` 负责从队列接收消息并串口输出
- 整理串口输出格式，方便观察系统运行状态
- 进行系统稳定性观察

## 三、工程结构说明

### 1. main.c

`main.c` 负责系统初始化和启动 FreeRTOS 调度器，主要完成：

- HAL 初始化
- GPIO 初始化
- 系统时钟配置
- USART1 初始化
- I2C1 初始化
- DWT 初始化
- DHT11 数据引脚初始化
- OLED 初始化
- FreeRTOS 初始化
- 启动调度器

主函数不再处理具体业务逻辑，真正的采集、显示、输出逻辑都放在 FreeRTOS 任务中。

### 2. freertos.c

`freertos.c` 是当前项目的核心逻辑文件，主要包含：

- `SensorData`：保存系统当前状态
- `SensorMessage`：作为队列消息结构体
- `sensorTask`：采集任务
- `displayTask`：OLED 显示任务
- `uartTask`：串口输出任务
- `g_sensor_msg_queue`：传感器消息队列

## 四、当前系统架构

当前系统采用：

```text
共享结构体 + 消息队列

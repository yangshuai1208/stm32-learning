# Day23 温湿度阈值报警状态显示 v1

## 一、项目目标

本项目基于前面已经完成的 STM32F103 + FreeRTOS 温湿度监测系统，继续增加温湿度阈值报警功能。

第23天的重点是：

- 根据温湿度数据判断当前环境状态
- 增加报警状态枚举
- 在 OLED 上显示报警状态
- 在串口中输出完整报警信息
- 将温湿度监测从“只显示数据”升级为“能判断状态”

---

## 二、当前系统功能

当前系统已经具备：

- DHT11 温湿度采集
- OLED 实时显示
- 串口状态输出
- FreeRTOS 三任务架构
- 共享结构体保存当前状态
- 消息队列传递最新采样结果
- Flash 配置参数读取
- IWDG 看门狗任务健康检测
- 温湿度报警状态判断

---

## 三、任务架构

当前系统包含 4 个主要任务：

### 1. sensorTask

负责：

- 读取 DHT11 温湿度数据
- 更新共享结构体 `g_sensor_data`
- 判断当前报警状态
- 构造 `SensorMessage`
- 使用队列发送给串口任务

### 2. displayTask

负责：

- 读取共享结构体快照
- 在 OLED 上显示温度、湿度和报警状态

### 3. uartTask

负责：

- 从消息队列接收最新采样结果
- 串口输出温湿度、成功次数、失败次数和报警状态
- 翻转 LED 作为运行心跳

### 4. watchdogTask

负责：

- 检查 sensorTask、displayTask、uartTask 是否持续运行
- 三个任务都正常时才喂 IWDG

---

## 四、报警状态设计

本次新增报警状态枚举：

```c
typedef enum
{
    ALARM_NONE = 0,
    ALARM_TEMP_HIGH,
    ALARM_HUMI_LOW,
    ALARM_BOTH
} AlarmState;

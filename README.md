# STM32 + FreeRTOS 30天学习项目

## 一、项目简介

本仓库记录了我第二阶段 STM32 + FreeRTOS 的 30 天学习过程。

本阶段以 STM32F103C8T6 为主控，基于 HAL 库、STM32CubeMX 和 Keil MDK 进行开发，从 GPIO、USART、I2C、DHT11、OLED 等基础外设入手，逐步引入 FreeRTOS 多任务、队列、互斥量、事件标志组、Flash 配置保存、IWDG 看门狗和 ESP8266 AT 指令通信框架，最终整合为一个基于 STM32 + FreeRTOS 的环境监测与报警系统。

项目目标不是单纯学习外设 API，而是完成一个能够展示、能够写进简历、能够在面试中讲清楚的嵌入式综合项目。

---

## 二、最终项目名称

**基于 STM32F103 + FreeRTOS 的环境监测与报警系统**

---

## 三、开发环境

| 项目 | 内容 |
|---|---|
| 主控芯片 | STM32F103C8T6 |
| 开发方式 | STM32CubeMX + HAL 库 |
| IDE | Keil MDK |
| RTOS | FreeRTOS / CMSIS-RTOS v1 |
| 调试方式 | USART1 串口调试 |
| 代码管理 | Git + GitHub |
| 编译器 | ARMCC V5 |

---

## 四、硬件模块

| 模块 | 作用 |
|---|---|
| STM32F103C8T6 | 主控芯片 |
| DHT11 | 温湿度采集 |
| OLED 0.96 寸 I2C | 显示温湿度和报警状态 |
| 按键 | 修改报警阈值 |
| LED | 系统运行状态指示 |
| Flash | 保存配置参数 |
| IWDG | 系统异常恢复 |
| ESP8266 | WiFi 通信扩展 |
| USART1 | 电脑串口调试 |
| USART2 | ESP8266 AT 指令通信 |

---

## 五、最终系统功能

当前项目已经实现或预留以下功能：

- DHT11 温湿度采集
- OLED 实时显示温湿度和报警状态
- USART1 串口输出系统运行日志
- 温湿度报警状态判断
- 按键修改报警阈值
- Flash 掉电保存配置参数
- FreeRTOS 多任务调度
- FreeRTOS 队列传递传感器数据
- FreeRTOS 互斥量保护共享资源
- FreeRTOS 事件标志组监控系统状态
- IWDG 独立看门狗任务健康检测
- ESP8266 AT 指令通信框架
- WiFi 状态事件监控框架

说明：ESP8266 当前已完成 AT 指令通信框架，后续需要继续确认模块 AT 固件、波特率和 WiFi 连接状态，再扩展 HTTP / TCP / MQTT 数据上传。

---

## 六、系统整体架构

```text
DHT11
  ↓
sensorTask
  ↓
SensorMessage Queue
  ↓
uartTask → USART1 → 串口助手

sensorTask
  ↓
g_sensor_data
  ↓
displayTask → OLED 显示

keyTask
  ↓
g_app_config
  ↓
Flash 保存配置

sensorTask / displayTask / uartTask / keyTask / espTask
  ↓
EventGroup
  ↓
monitorTask → 输出系统状态

watchdogTask
  ↓
检查任务心跳
  ↓
IWDG 喂狗

espTask
  ↓
USART2
  ↓
ESP8266

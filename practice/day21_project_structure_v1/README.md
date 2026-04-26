# Day21 FreeRTOS 温湿度监测系统工程整理版

## 一、项目目标

本项目基于 STM32F103 + FreeRTOS，实现一个基础的温湿度监测系统。

当前系统已经完成：

- DHT11 温湿度采集
- OLED 实时显示
- 串口状态输出
- FreeRTOS 多任务调度
- 共享结构体保存当前状态
- 消息队列传递最新采样结果
- 成功次数和失败次数统计
- 基本系统稳定性观察

第21天的重点不是继续新增外设，而是对前面已经完成的功能进行工程整理，形成一个结构清晰、便于后续扩展和 GitHub 展示的项目版本。

---

## 二、硬件环境

### 主控芯片

- STM32F103C8T6

### 外设模块

- DHT11 温湿度传感器
- 0.96 寸 I2C OLED 显示屏
- 串口调试模块
- LED 指示灯

### 开发工具

- STM32CubeMX
- Keil MDK
- HAL 库
- FreeRTOS
- 串口调试助手

---

## 三、硬件连接

### OLED

| OLED 引脚 | STM32 引脚 |
|---|---|
| SCL | PB6 |
| SDA | PB7 |
| VCC | 3.3V |
| GND | GND |

### DHT11

| DHT11 引脚 | STM32 引脚 |
|---|---|
| DATA | PB12 |
| VCC | 3.3V 或 5V |
| GND | GND |

### 串口

| 功能 | STM32 引脚 |
|---|---|
| USART1_TX | PA9 |
| USART1_RX | PA10 |
| 波特率 | 115200 |

---

## 四、工程结构

当前工程主要包含以下部分：

```text
day21_project_structure_v1/
├── Core/
│   ├── Inc/
│   └── Src/
├── Drivers/
├── Middlewares/
├── User/
│   ├── dht11/
│   ├── dwt/
│   ├── oled/
│   └── fonts/
├── notes/
│   └── day21.md
├── README.md
└── Fire_F103_2.ioc

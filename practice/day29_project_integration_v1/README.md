# Day29 STM32 + FreeRTOS 环境监测系统整合 v1

## 一、项目目标

本项目基于前 28 天完成的功能模块，对 STM32 + FreeRTOS 环境监测系统进行最终整合。

第29天的目标不是继续增加大量新功能，而是对已有模块进行统一整理、异常状态输出和演示现象准备。

当前项目已经从单一外设实验，整合为一个具备多任务调度、传感器采集、显示、报警、配置保存、系统监控和 WiFi 通信框架的综合项目。

---

## 二、当前系统功能

当前系统已经具备以下功能：

- DHT11 温湿度采集
- OLED 实时显示温湿度和报警状态
- USART1 串口调试输出
- 按键修改报警阈值
- Flash 掉电保存配置参数
- FreeRTOS 多任务调度
- 队列传递传感器数据
- 互斥量保护串口、OLED 和配置结构体
- 事件标志组监控系统状态
- IWDG 独立看门狗任务健康检测
- ESP8266 AT 指令通信框架
- WiFi 状态事件监控

---

## 三、系统硬件组成

| 模块 | 作用 |
|---|---|
| STM32F103C8T6 | 主控芯片 |
| DHT11 | 温湿度采集 |
| OLED 0.96 寸 I2C | 显示温湿度和报警状态 |
| 按键 | 修改报警阈值 |
| Flash | 保存配置参数 |
| ESP8266 | WiFi 通信扩展 |
| USART1 | 电脑串口调试 |
| USART2 | ESP8266 AT 指令通信 |
| IWDG | 系统异常恢复 |

---

## 四、串口分配

| 串口 | 作用 |
|---|---|
| USART1 | 连接电脑串口助手，输出系统日志 |
| USART2 | 连接 ESP8266，发送 AT 指令 |

USART2 引脚：

| STM32F103 | ESP8266 |
|---|---|
| PA2 / USART2_TX | ESP8266 RX |
| PA3 / USART2_RX | ESP8266 TX |
| GND | GND |

---

## 五、FreeRTOS 任务设计

### 1. sensorTask

负责：

- 读取 DHT11 温湿度数据
- 判断 DHT11 读取是否成功
- 根据配置阈值判断报警状态
- 更新共享结构体 `g_sensor_data`
- 构造 `SensorMessage`
- 通过队列发送给 uartTask
- 设置事件标志位 `EVT_SENSOR_RUN`
- 设置 `EVT_DHT11_OK` 或 `EVT_DHT11_ERR`
- 设置或清除 `EVT_ALARM_ACTIVE`

---

### 2. displayTask

负责：

- 读取 `g_sensor_data` 快照
- OLED 显示温度、湿度、报警状态
- 显示 DHT11 读取错误信息
- 使用 OLED 互斥量保护 I2C / OLED 显示资源
- 设置 `EVT_DISPLAY_RUN`

---

### 3. uartTask

负责：

- 从队列中接收 `SensorMessage`
- 串口输出温湿度、报警状态、阈值、错误次数
- 输出格式化系统运行日志
- 使用串口互斥量保护 USART1
- 设置 `EVT_UART_RUN`

---

### 4. keyTask

负责：

- 扫描 MODE / UP / DOWN 按键
- 切换当前设置项
- 修改温度报警上限
- 修改湿度报警下限
- 修改后保存到 Flash
- 配置保存成功后设置 `EVT_CONFIG_SAVED`

---

### 5. monitorTask

负责：

- 监控 sensorTask、displayTask、uartTask 是否正常运行
- 输出 `SYS TASK OK` 或 `SYS TASK TIMEOUT`
- 检测报警状态
- 检测配置保存事件
- 检测 WiFi 状态

---

### 6. watchdogTask

负责：

- 周期性检查 sensorTask、displayTask、uartTask 的心跳
- 如果关键任务都在运行，则刷新 IWDG
- 如果任务异常卡死，则不喂狗，等待系统复位

---

### 7. espTask

负责：

- 启动 ESP8266 通信流程
- 发送 AT 指令测试模块是否响应
- 尝试连接 WiFi
- 设置 `EVT_WIFI_OK` 或 `EVT_WIFI_ERR`
- 输出 ESP8266 当前状态

---

## 六、任务间通信设计

### 1. 队列

使用队列传递传感器消息：

```c
g_sensor_msg_queue = xQueueCreate(1, sizeof(SensorMessage));

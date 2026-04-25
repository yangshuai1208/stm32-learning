# Day20 IWDG 独立看门狗与任务健康检测

## 一、项目目标

本项目基于前几天完成的 FreeRTOS + DHT11 + OLED + 串口输出系统，继续增加 IWDG 独立看门狗和任务健康检测机制。

第20天的核心目标是：

- 学习 IWDG 独立看门狗的基本使用
- 在 FreeRTOS 工程中加入 watchdogTask
- 使用任务心跳判断系统是否正常运行
- 只有关键任务都正常运行时才喂狗
- 提高系统异常自恢复能力

## 二、今天完成的内容

今天主要完成了以下内容：

- 在 CubeMX 中开启 IWDG 独立看门狗
- 生成 `iwdg.c` 和 `iwdg.h`
- 在 `main.c` 中调用 `MX_IWDG_Init()`
- 在 `freertos.c` 中新增 `watchdogTask`
- 为 `displayTask` 增加显示任务心跳
- 为 `uartTask` 增加串口任务心跳
- 使用 `sensorTask` 的成功/失败次数作为采集任务心跳
- 在 watchdogTask 中统一判断任务健康状态
- 只有系统健康时才调用 `HAL_IWDG_Refresh()`

## 三、当前系统结构

当前系统主要包含 4 个任务：

### 1. sensorTask

负责：

- 周期读取 DHT11 温湿度
- 更新共享结构体 `g_sensor_data`
- 统计成功采集次数 `update_count`
- 统计失败次数 `error_count`
- 构造 `SensorMessage`
- 通过队列发送最新采样结果

### 2. displayTask

负责：

- 从共享结构体读取当前状态
- OLED 显示温湿度或错误信息
- 每执行一轮，`g_display_heartbeat++`

### 3. uartTask

负责：

- 从队列中接收 `SensorMessage`
- 通过串口输出温湿度或错误状态
- 翻转 LED 作为运行提示
- 每成功输出一次，`g_uart_heartbeat++`

### 4. watchdogTask

负责：

- 周期检查 sensor/display/uart 三个任务是否正常运行
- 如果三个任务状态都发生变化，则刷新 IWDG
- 如果某个任务卡死或长时间无变化，则停止喂狗
- IWDG 超时后自动复位系统

## 四、IWDG 配置说明

当前 IWDG 配置如下：

```c
hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
hiwdg.Init.Reload = 4095;

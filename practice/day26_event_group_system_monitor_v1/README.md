# Day26 FreeRTOS 事件标志组管理系统状态 v1

## 一、项目目标

本项目基于前面完成的 STM32F103 + FreeRTOS 温湿度监测系统，继续引入 FreeRTOS 事件标志组，用于统一管理系统运行状态。

第26天的重点是：

- 使用事件标志组记录任务运行状态
- 记录 DHT11 读取成功或失败
- 记录当前是否存在报警状态
- 记录配置是否保存成功
- 新增 monitorTask 周期性检查系统状态

---

## 二、当前系统功能

当前系统已经具备：

- DHT11 温湿度采集
- OLED 实时显示
- 串口状态输出
- 温湿度报警判断
- 按键修改报警阈值
- Flash 保存配置参数
- IWDG 看门狗健康检测
- 互斥量保护串口、OLED 和配置资源
- 事件标志组管理系统状态

---

## 三、事件标志位设计

本项目定义了以下事件位：

```c
#define EVT_SENSOR_RUN    (1U << 0)
#define EVT_DISPLAY_RUN   (1U << 1)
#define EVT_UART_RUN      (1U << 2)
#define EVT_DHT11_OK      (1U << 3)
#define EVT_DHT11_ERR     (1U << 4)
#define EVT_ALARM_ACTIVE  (1U << 5)
#define EVT_CONFIG_SAVED  (1U << 6)

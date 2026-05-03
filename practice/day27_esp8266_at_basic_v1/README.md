# Day27 ESP8266 AT 指令通信框架和 WiFi 状态监控 v1

## 一、项目目标

本项目基于 STM32F103 + FreeRTOS 温湿度监测系统，新增 ESP8266 WiFi 模块通信框架。

第27天目标：

- 使用 USART2 连接 ESP8266
- 新增 ESP8266 AT 指令驱动文件
- 通过 AT 指令测试 ESP8266 是否响应
- 新增 espTask 管理 ESP8266 初始化流程
- 将 WiFi 状态接入 FreeRTOS 事件标志组
- 通过 monitorTask 输出 WiFi 正常或异常状态

本日重点不是上云，也不是 MQTT，而是先完成 STM32 与 ESP8266 的 AT 指令通信框架。

---

## 二、当前系统功能

当前系统已经具备：

- DHT11 温湿度采集
- OLED 实时显示
- USART1 串口调试输出
- 温湿度报警判断
- 按键修改报警阈值
- Flash 保存配置参数
- IWDG 看门狗任务健康检测
- FreeRTOS 队列传递传感器数据
- FreeRTOS 互斥量保护共享资源
- FreeRTOS 事件标志组监控系统状态
- USART2 与 ESP8266 通信框架
- WiFi 状态事件监控

---

## 三、硬件连接

### STM32 与 ESP8266 接线

本项目使用 USART2 连接 ESP8266。

| STM32F103 | ESP8266 开发板 |
|---|---|
| PA2 / USART2_TX | RX-D0 / RXD |
| PA3 / USART2_RX | TX-D1 / TXD |
| GND | GND |

注意：

```text
STM32 TX 接 ESP8266 RX
STM32 RX 接 ESP8266 TX
GND 必须共地

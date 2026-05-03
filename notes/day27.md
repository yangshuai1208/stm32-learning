
---

# 四、`notes/day27.md`

```md
# Day27 学习笔记：ESP8266 AT 指令通信框架和 WiFi 状态监控

## 一、今天做了什么

今天在 STM32F103 + FreeRTOS 温湿度监测项目基础上，新增 ESP8266 通信模块。

主要完成：

- CubeMX 开启 USART2
- PA2 配置为 USART2_TX
- PA3 配置为 USART2_RX
- 原 PA2 / PA3 上的 LED_G / LED_B 暂时放弃
- 新增 `bsp_esp8266.c`
- 新增 `bsp_esp8266.h`
- 实现 `ESP8266_SendCmd()`
- 实现 `ESP8266_BasicInit()`
- 实现 `ESP8266_ConnectWiFi()`
- 新增 `espTask`
- 新增 `EVT_WIFI_OK`
- 新增 `EVT_WIFI_ERR`
- monitorTask 增加 WiFi 状态检测

---

## 二、为什么使用 USART2

USART1 已经用于电脑串口调试。

因此本项目使用：

```text
USART1 -> 电脑串口助手，查看系统日志
USART2 -> ESP8266，发送 AT 指令

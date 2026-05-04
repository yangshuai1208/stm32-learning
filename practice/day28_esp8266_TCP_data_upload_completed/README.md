# 环境监测系统 (Environment Monitor)

基于 STM32F103C8T6 + FreeRTOS 的物联网环境监测终端，实时采集温湿度，本地 OLED 显示，并支持按键设置报警阈值，通过 ESP8266 将数据上报至远程服务器。

## 功能特点
- 实时采集 DHT11 温湿度，每秒刷新 OLED 显示
- 三键设置报警阈值（温度上限 / 湿度下限），自动保存至 Flash
- ESP8266 连接 WiFi，TCP 上传传感器数据
- 独立看门狗 + 任务监控，系统异常自动复位
- FreeRTOS 多任务架构，临界区互斥保护，模块化驱动设计

## 硬件连接
| 模块       | 引脚      | 说明           |
|------------|-----------|----------------|
| OLED (I2C) | PB6/SCL, PB7/SDA | 0.96 寸，地址 0x3C |
| DHT11      | PB8       | 单总线温湿度传感器 |
| ESP8266    | PA2/TX, PA3/RX | 串口2，波特率 115200 |
| 按键       | PB0(MODE), PB1(UP), PB10(DOWN) | 上拉输入，低有效 |
| LED        | PA1       | 运行指示灯       |

## 软件架构
- **LedTask**：OLED 显示刷新（1s）
- **UartTask**：串口打印传感器数据（从队列获取）
- **SensorTask**：读取 DHT11，发布消息，报警判断
- **KeyTask**：按键消抖，修改报警阈值，保存到 Flash
- **WifiTask**：ESP8266 初始化，连接 WiFi，TCP 发送数据
- **WatchdogTask**：监控所有任务心跳，喂狗
- **MonitorTask**：事件组监控各任务运行状态

## 关键数据流
- 传感器 → 全局变量（临界区保护） → OLED 显示
- 传感器 → 消息队列 → 串口打印
- 传感器 → 全局变量快照 → ESP8266 TCP 发送 → 服务器

## 编译与使用
1. STM32CubeMX 生成基础工程，启用 FreeRTOS、USART1/2、I2C1、IWDG
2. Keil 导入所有源文件，编译下载
3. 修改 `freertos.c` 中的 `WIFI_SSID`、`WIFI_PASSWORD` 为你自己的热点
4. 修改 `ESP8266_TCPConnect` 的目标 IP 和端口
5. 网络调试助手开启 TCP Server，端口监听，查看上报数据

## 注意事项
- ESP8266 需独立 3.3V 供电（电流 >300mA，STM32 板载 LDO 可能供电不足）
- I2C 引脚必须外接 4.7kΩ 上拉电阻
- 首次上电会写入默认报警阈值（温度 35℃、湿度 20%），后续按键修改自动保存

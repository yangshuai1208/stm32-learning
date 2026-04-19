# Day14 FreeRTOS + DHT11 + OLED 温湿度显示实验

## 一、实验目标
在 STM32F103 平台上，基于 FreeRTOS 实现 DHT11 温湿度采集，并通过 OLED 和串口分别显示结果，构建一个最小可运行的多任务温湿度监测系统。

## 二、实验内容
本次实验完成了以下内容：

- 初始化 GPIO
- 初始化 USART1 串口
- 初始化 I2C1
- 初始化 OLED 屏幕
- 初始化 DHT11 数据引脚
- 使用 FreeRTOS 创建多个任务
- 在采集任务中读取 DHT11 温湿度数据
- 在 OLED 任务中显示温湿度
- 在串口任务中输出温湿度
- 使用 LED 作为系统运行心跳指示

## 三、工程结构说明
### 1. main.c
负责完成以下工作：
- 系统时钟初始化
- GPIO 初始化
- USART1 初始化
- I2C1 初始化
- OLED 初始化
- 启动 FreeRTOS 调度器

### 2. freertos.c
负责：
- 创建 `defaultTask`
- 创建 `sensorTask`
- 创建 `ledTask`
- 创建 `uartTask`

其中：
- `sensorTask`：负责采集 DHT11 数据
- `ledTask`：负责 OLED 显示
- `uartTask`：负责串口输出与 LED 心跳

### 3. bsp_dht11.c / bsp_dht11.h
负责：
- DHT11 数据引脚初始化
- DHT11 时序读取
- 温湿度数据校验

### 4. bsp_i2c_oled.c / bsp_i2c_oled.h
负责：
- OLED 初始化
- OLED 命令与数据写入
- OLED 字符串显示

### 5. bsp_dwt.c / bsp_dwt.h
负责：
- 微秒级精确延时
- 提供 DHT11 读取所需时序基础

## 四、硬件连接
### 1. OLED
- SCL -> PB6
- SDA -> PB7
- VCC -> 3.3V
- GND -> GND

### 2. DHT11
- DATA -> PB12
- VCC -> 3.3V 或 5V
- GND -> GND

### 3. 串口
- USART1_TX -> PA9
- USART1_RX -> PA10
- 波特率 -> 115200

## 五、程序流程
1. 上电后初始化 HAL 库
2. 配置系统时钟
3. 初始化 GPIO
4. 初始化 USART1
5. 初始化 I2C1
6. 初始化 OLED
7. 创建 FreeRTOS 任务
8. 启动调度器
9. `sensorTask` 周期读取 DHT11 数据
10. `ledTask` 周期刷新 OLED 显示
11. `uartTask` 周期输出串口信息，并翻转 LED

## 六、任务设计
### 1. sensorTask
功能：
- 调用 `DHT11_ReadData()`
- 成功则更新共享数据结构体
- 失败则标记数据无效

周期：
- 2000ms

### 2. ledTask
功能：
- 读取共享温湿度数据
- 在 OLED 上显示：
  - `DHT11`
  - `TEMP:xx.xC`
  - `HUMI:xx.x%`
- 若读取失败，则显示 `READ ERROR`

周期：
- 1000ms

### 3. uartTask
功能：
- 读取共享温湿度数据
- 串口输出温湿度
- 翻转红灯作为运行指示

周期：
- 2000ms

## 七、实验现象
### 1. 串口现象
上电后串口会输出：
- `BOOT`
- `OLED READY` 或 `OLED FAIL`

正常运行时，串口周期打印：
- `TEMP:xx.xC  HUMI:xx.x%`

如果 DHT11 读取失败，则打印：
- `DHT11 READ ERROR`

### 2. OLED 现象
正常读取时，OLED 显示：
- `DHT11`
- `TEMP:xx.xC`
- `HUMI:xx.x%`

读取失败时，OLED 显示：
- `DHT11`
- `READ ERROR`

### 3. LED 现象
红灯周期翻转，作为系统运行心跳。

## 八、关键知识点
通过本实验，主要学习了以下内容：

- FreeRTOS 多任务基本结构
- 采集任务、显示任务、串口任务的职责拆分
- 共享数据结构体在任务间的数据传递思路
- DHT11 的时序读取与校验
- OLED 的 I2C 初始化与字符串显示
- 从单任务到多任务系统整合的方法

## 九、实验总结
本实验完成了一个基于 FreeRTOS 的最小温湿度监测系统，实现了：

- DHT11 采集
- OLED 显示
- 串口输出
- 任务拆分运行

这一步让项目从“单外设实验”升级成了“多任务小系统”，为后续继续加入按键、中断、队列、联网模块打下基础。

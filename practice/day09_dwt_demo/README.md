# STM32 DWT 精确延时与性能测量

## 功能
- 使用 DWT（Data Watchpoint and Trace）外设实现微秒级精确延时
- 测量代码块执行时间（CPU 周期数 → 微秒）
- 演示 RGB LED 闪烁，并计算亮灭周期耗时

## 硬件连接
- MCU：STM32F103C8T6（野火开发板）
- LED：RGB 共阳极，低电平点亮（PA0-R, PA1-G, PA2-B）
- 系统时钟：72MHz

## 使用方法
1. 用 Keil 5 打开 `MDK-ARM/` 下的工程文件
2. 编译下载到开发板
3. 观察 RGB LED 交替亮灭（亮 1s，灭 1s）
4. 可在调试窗口查看变量 `task_us`（任务实际耗时，单位微秒）

## 代码说明
- `bsp_dwt.c`：DWT 初始化、获取时钟计数、周期转微秒、微秒/毫秒/秒延时函数
- `bsp_led.c`：LED GPIO 初始化及控制函数
- `main.c`：主循环中记录开始/结束时间，计算任务耗时并转换为微秒

## 注意事项
- 必须先调用 `DWT_Init()` 使能 DWT 计数器
- DWT 延时基于系统时钟频率 `SystemCoreClock`，需确保时钟配置正确
- 测量结果存储在 `task_us` 变量，可在调试器中查看

## 参考
- 野火 STM32 开发板教程

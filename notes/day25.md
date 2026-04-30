
---

# 三、`notes/day25.md`

```md
# Day25 学习笔记：FreeRTOS 互斥量保护共享资源 v1

## 一、今天做了什么

今天在第24天按键修改报警阈值并保存 Flash 的基础上，加入了 FreeRTOS 互斥量机制。

主要完成：

- 新增 `g_uart_mutex`
- 新增 `g_oled_mutex`
- 新增 `g_config_mutex`
- 使用互斥量保护串口输出
- 使用互斥量保护 OLED 显示
- 使用互斥量保护配置结构体 `g_app_config`
- 使用配置快照减少共享数据访问冲突

今天的重点不是新增外设，而是让多任务系统访问共享资源更规范。

---

## 二、为什么需要互斥量

当前系统中已经有多个 FreeRTOS 任务：

```text
sensorTask
displayTask
uartTask
keyTask
watchdogTask

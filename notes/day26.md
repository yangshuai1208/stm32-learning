
---

# 六、notes/day26.md

```md
# Day26 学习笔记：FreeRTOS 事件标志组管理系统状态

## 一、今天做了什么

今天在第25天互斥量保护共享资源的基础上，引入了 FreeRTOS 事件标志组。

主要完成：

- 引入 `event_groups.h`
- 定义系统事件位
- 创建 `g_system_event_group`
- 在 sensorTask / displayTask / uartTask / keyTask 中设置事件位
- 新增 monitorTask
- 使用事件标志组统一监控系统状态

---

## 二、为什么需要事件标志组

前面的系统已经有多个任务：

```text
sensorTask
displayTask
uartTask
keyTask
watchdogTask

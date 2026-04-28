
---

# 四、`notes/day23.md`

```md
# Day23 学习笔记：温湿度阈值报警状态显示 v1

## 一、今天做了什么

今天在前面 FreeRTOS 温湿度监测系统的基础上，增加了温湿度阈值报警功能。

主要完成：

- 新增 `AlarmState` 报警状态枚举
- 给 `SensorData` 增加 `alarm_state`
- 给 `SensorMessage` 增加 `alarm_state`
- 新增报警判断函数
- OLED 第 4 行显示报警状态
- 串口输出中增加 `ALARM:%s`
- 将温湿度数据从“只显示数值”升级为“能判断状态”

---

## 二、为什么要做报警状态

前面的系统已经可以显示：

- 温度
- 湿度
- 成功次数
- 失败次数

但是只能看到数据，不能直接判断当前环境是否异常。

今天加入报警状态后，系统可以判断：

- 温度是否过高
- 湿度是否过低
- 是否同时出现温度高和湿度低

这样项目更像一个真实的环境监测系统。

---

## 三、报警状态枚举

今天新增：

```c
typedef enum
{
    ALARM_NONE = 0,
    ALARM_TEMP_HIGH,
    ALARM_HUMI_LOW,
    ALARM_BOTH
} AlarmState;

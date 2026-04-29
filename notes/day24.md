
---

# 三、`notes/day24.md`

```md
# Day24 学习笔记：按键修改报警阈值并保存 Flash v1

## 一、今天做了什么

今天在第23天温湿度阈值报警系统的基础上，增加了按键修改配置的功能。

主要完成：

- 新增设置项枚举 `SettingItem`
- 新增 `keyTask`
- 新增按键消抖函数 `Key_IsPressed`
- 使用 MODE 按键切换设置项
- 使用 UP 按键增加阈值
- 使用 DOWN 按键减少阈值
- 修改后调用 `AppConfig_Save()` 保存到 Flash
- 报警判断使用 `g_app_config` 中的配置阈值

---

## 二、为什么要做按键配置

第23天的报警阈值是固定的。

如果阈值写死在代码中：

```c
if (temp_int >= 30)
if (humi_int <= 40)

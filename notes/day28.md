# Day28：ESP8266 TCP 数据上报

## 学习目标
- 使用 AT 指令控制 ESP8266 连接 WiFi 并建立 TCP 连接
- 完成 FreeRTOS 多任务间传感器数据的传递（临界区快照 vs 队列）
- 实现从传感器采集到云端（服务器）的完整数据链路

## 关键代码
- `ESP8266_BasicInit()` / `ESP8266_ConnectWiFi()` / `ESP8266_TCPConnect()` / `ESP8266_TCPSend()`
  - 封装 AT 指令，带超时、期望应答字符串匹配
- `StartWiFiTask()` 使用 `taskENTER_CRITICAL` 快照全局变量 `g_sensor_data`，避免与 UartTask 竞争队列
- `ESP8266_TCPSend` 中先发 `AT+CIPSEND=<len>`，等待 `>` 提示，再发送实际数据，最后判断 `SEND OK`

## 踩坑记录
- **队列竞争**：如果 WiFi 任务也读 `g_sensor_msg_queue`，UartTask 会永久收不到消息。解决方案：WiFi 任务直接读全局变量快照（无需队列）
- **sprintf 字段错位**：原本 `TEMP=%d.%d` 写成了 `msg.humi_int, msg.temp_dec`，导致数据完全错误
- **AT+CWJAP 重复连接**：模块已连接时返回 `ALREADY CONNECTED`，`ESP8266_TCPConnect` 需兼容此情况
- **`AT+CIPSEND` 必须等 `>`**：发送完指令后不能用死等，要解析到 `>` 后方可传输数据，否则模块不识别

## 验证方法
- 电脑打开网络调试助手，监听 12345 端口（与代码中一致）
- 上电后串口助手查看状态：`ESP8266 AT OK` → `WIFI CONNECT OK` → `TCP CONNECT OK` → `SEND TO WIFI` → `TCP SEND OK`
- 服务器端收到格式如 `TEMP=25.5,HUMI=60.0` 的数据
- OLED 同时显示温湿度和报警状态

## 下一步优化
- 添加断线重连机制（TCP 发送失败时重置模块）
- 使用 JSON 格式上报数据，方便服务器解析
- 加入 MQTT 协议替代原始 TCP 透传

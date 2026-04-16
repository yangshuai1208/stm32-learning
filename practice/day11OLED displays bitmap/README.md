# OLED 显示位图（SSD1306，I2C/SPI）

## 功能
- 在 0.96 寸 OLED（SSD1306）上显示自定义位图（如 Logo、图标、汉字）
- 支持从字模软件（如 PCtoLCD2002）提取的位图数据
- 提供简单的画点、画线、显示图片函数

## 硬件连接
- MCU：STM32F103C8T6（或其他型号）
- OLED 接口：
  - I2C 方式：SCL→PB6，SDA→PB7
  - SPI 方式：CS→PA4，DC→PA5，RST→PA6，DIN→PA7，CLK→PA5
- 电源：3.3V 或 5V（视模块而定）

## 使用方法
1. 用 Keil 5 打开工程，编译下载
2. OLED 会显示预置的位图（如开机 Logo）
3. 可在代码中调用 `OLED_ShowBitmap(x, y, width, height, bitmap)` 显示任意位图

## 位图制作方法
1. 使用 **PCtoLCD2002** 或 **Image2Lcd** 软件
2. 设置图像尺寸（如 128x64，或自定义）
3. 取模方式：**阴码 + 逐行式 + 十六进制**
4. 将生成的数组复制到代码中，例如：
   ```c
   const uint8_t logo[] = {
       0x00, 0x00, ... // 位图数据
   };

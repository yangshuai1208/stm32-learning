
#include "bmp280/bsp_spi_bmp280.h" 
#include "spi/bsp_spi.h" 
#include "usart/bsp_usart.h"

BMP280_CalibData bmp280_calib = {0};	//BMP280 校准系数结构体变量，存储读取到的校准数据

/**
  * @brief  初始化 BMP280 片选引脚GPIO
  * @note   配置 BMP280_CSB 引脚为推挽输出，初始为低电平
  * @retval 无
  */
void BMP280_CSB_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* 使能 GPIOA 时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* 初始化 BMP280_CSB 引脚为低电平 */
    HAL_GPIO_WritePin(BMP280_CSB_GPIO_Port, BMP280_CSB_Pin, GPIO_PIN_RESET);

    /* 配置 BMP280_CSB 引脚为推挽输出，无上下拉，低速 */
    GPIO_InitStruct.Pin = BMP280_CSB_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(BMP280_CSB_GPIO_Port, &GPIO_InitStruct);
}

/**
  * @brief  读取 SPI_BMP280 指定寄存器的值（单字节）
  * @param  read_addr：寄存器地址 
  * @note   读操作时寄存器地址的最高位需置1（0x80）
  * @retval 寄存器的值
  */
uint8_t SPI_BMP280_ReadReg(uint8_t read_addr)
{
    uint8_t tx_data = read_addr | 0x80;   // 将寄存器地址最高位置1，表示SPI读操作
    uint8_t rx_data = 0;                  // 用于存放读取到的寄存器值

    BMP280_CSB_LOW();  // 拉低片选信号，通知BMP280开始SPI通信

    HAL_SPI_Transmit(&hspi1, &tx_data, 1, HAL_MAX_DELAY);  
    // 发送寄存器地址（带读标志位）

    HAL_SPI_TransmitReceive(&hspi1, (uint8_t[]){0xFF}, &rx_data, 1, HAL_MAX_DELAY);  
    // 同时发送一个无意义字节（0xFF），以产生时钟并接收寄存器数据

    BMP280_CSB_HIGH();  // 拉高片选信号，结束SPI通信

    return rx_data;     // 返回读取到的寄存器数据
}

/**
  * @brief  连续读取 SPI_BMP280 寄存器的值
  * @param  read_addr   读取的寄存器地址
  * @param  buffer      存储读出数据的指针
  * @param  size        读取数据的长度
  * @note   无
  * @retval 无
  */
void SPI_BMP280_ReadBuffer(uint8_t read_addr, uint8_t* buffer, uint32_t size)
{
    uint8_t tx_data = read_addr | 0x80;    // 读操作：最高位置 1

    BMP280_CSB_LOW();                      // 拉低片选，开始通信
    HAL_SPI_Transmit(&hspi1, &tx_data, 1, HAL_MAX_DELAY);   // 发送寄存器地址
    HAL_SPI_Receive(&hspi1, buffer, size, HAL_MAX_DELAY);   // 接收连续数据
    BMP280_CSB_HIGH();                     // 拉高片选，结束通信
}

/**
  * @brief  对 SPI_BMP280 写入数据
  * @param  write_addr  写入的寄存器首地址
  * @param  buffer      存储写入数据的指针
  * @param  size        写入数据的长度
  * @note   无
  * @retval 无
  */
void SPI_BMP280_WriteBuffer(uint8_t write_addr, uint8_t* buffer, uint32_t size)
{
    uint8_t tx_addr = write_addr & 0x7F;   // 写操作：最高位清 0

    BMP280_CSB_LOW();                      // 拉低片选，开始通信
    HAL_SPI_Transmit(&hspi1, &tx_addr, 1, HAL_MAX_DELAY);   // 发送写寄存器地址
    HAL_SPI_Transmit(&hspi1, buffer, size, HAL_MAX_DELAY);  // 发送写入数据
    BMP280_CSB_HIGH();                     // 拉高片选，结束通信
}

/**
  * @brief  读取 BMP280 ID
  * @param  无
  * @note   无
  * @retval BMP280 ID
  */
uint8_t SPI_BMP280_ReadID(void)
{
    return SPI_BMP280_ReadReg(BMP280_REGADDR_ID);    // 读取 ID 寄存器
}

/**
  * @brief  获取 BMP280 状态
  * @param  无
  * @note   无
  * @retval 0 空闲
  *         1 正在测量(bit3) 或 正在复制(bit0)
  */
uint8_t SPI_BMP280_GetStatus(void)
{
    uint8_t device_status = 0;   // 定义变量存放状态值

    device_status = SPI_BMP280_ReadReg(BMP280_REGADDR_STATUS);   // 读取状态寄存器

    if (device_status &= 0x09)   // 判断是否正在测量(bit3=1) 或 正在复制(bit0=1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
  * @brief  重启设备（启动时间约 2 毫秒）
  * @param  无
  * @note   无
  * @retval 无
  */
void SPI_BMP280_reset(void)
{
    uint8_t buffer = BME280_SOFT_RESET_COMMAND;   // 复位命令
    SPI_BMP280_WriteBuffer(BMP280_REGADDR_SOFT_RESET, &buffer, 1);   // 发送复位命令
    HAL_Delay(2);                                 // 等待 2 毫秒
    while (SPI_BMP280_GetStatus() != 0);          // 等待设备空闲
}

/**
  * @brief  获取校准系数
  * @param  calib  存储系数的结构体指针
  * @retval 无
  */
void SPI_BMP280_GetCalibration(BMP280_CalibData *calib)
{
    uint8_t buffer[24];

    // 从 0x88 开始读取 24 字节的校正数据
    SPI_BMP280_ReadBuffer(BMP280_REGADDR_DIGT1_LSB, buffer, 24);

    // 解析温度校正系数
    calib->dig_t1 = (uint16_t)(buffer[1] << 8) | buffer[0];   // 0x88~0x89
    calib->dig_t2 = (int16_t) (buffer[3] << 8) | buffer[2];   // 0x8A~0x8B
    calib->dig_t3 = (int16_t) (buffer[5] << 8) | buffer[4];   // 0x8C~0x8D

    // 解析气压校正系数
    calib->dig_p1 = (uint16_t)(buffer[7] << 8) | buffer[6];   // 0x8E~0x8F
    calib->dig_p2 = (int16_t) (buffer[9] << 8) | buffer[8];   // 0x90~0x91
    calib->dig_p3 = (int16_t)(buffer[11] << 8) | buffer[10];  // 0x92~0x93
    calib->dig_p4 = (int16_t)(buffer[13] << 8) | buffer[12];  // 0x94~0x95
    calib->dig_p5 = (int16_t)(buffer[15] << 8) | buffer[14];  // 0x96~0x97
    calib->dig_p6 = (int16_t)(buffer[17] << 8) | buffer[16];  // 0x98~0x99
    calib->dig_p7 = (int16_t)(buffer[19] << 8) | buffer[18];  // 0x9A~0x9B
    calib->dig_p8 = (int16_t)(buffer[21] << 8) | buffer[20];  // 0x9C~0x9D
    calib->dig_p9 = (int16_t)(buffer[23] << 8) | buffer[22];  // 0x9E~0x9F
	
//    // 校准系数调试打印
//    printf("dig_t1：%d\r\n", calib->dig_t1);
//    printf("dig_t2：%d\r\n", calib->dig_t2);
//    printf("dig_t3：%d\r\n", calib->dig_t3);
//    printf("dig_p1：%d\r\n", calib->dig_p1);
//    printf("dig_p2：%d\r\n", calib->dig_p2);
//    printf("dig_p3：%d\r\n", calib->dig_p3);
//    printf("dig_p4：%d\r\n", calib->dig_p4);
//    printf("dig_p5：%d\r\n", calib->dig_p5);
//    printf("dig_p6：%d\r\n", calib->dig_p6);
//    printf("dig_p7：%d\r\n", calib->dig_p7);
//    printf("dig_p8：%d\r\n", calib->dig_p8);
//    printf("dig_p9：%d\r\n", calib->dig_p9);
}

/**
  * @brief  过采样设置
  * @param  osrs_p  气压过采样设置（0~5）
  * @param  osrs_t  温度过采样设置（0~5）
  * @retval 0 设置成功
  *         1 设置失败
  */
uint8_t SPI_BMP280_SetOverSampling(uint8_t osrs_p, uint8_t osrs_t)
{
    uint8_t ctrl_meas = 0;

    // 参数校验：最大支持 5（即 16 倍过采样）
    if (osrs_p > 5 || osrs_t > 5) {
        return 1;
    }

    // 读取 CTRL_MEAS 寄存器当前值
    ctrl_meas = SPI_BMP280_ReadReg(BMP280_REGADDR_CTRL_MEAS_POWER_MODE);

    // 清除原有过采样配置位（bit7~5 为温度，bit4~2 为气压）
    ctrl_meas &= ~((0x07 << 5) | (0x07 << 2));

    // 设置新的过采样值
    ctrl_meas |= ((osrs_t << 5) & (0x07 << 5)); // 温度
    ctrl_meas |= ((osrs_p << 2) & (0x07 << 2)); // 气压

    // 写回寄存器
    SPI_BMP280_WriteBuffer(BMP280_REGADDR_CTRL_MEAS_POWER_MODE, &ctrl_meas, 1);

    return 0;
}

/**
  * @brief  滤波器系数和采样间隔时间设置
  * @param  t_standby  采样间隔时间
  *                    0: 0.5ms
  *                    1: 62.5ms
  *                    2: 125ms
  *                    3: 250ms
  *                    4: 500ms
  *                    5: 1000ms
  *                    6: 2000ms
  *                    7: 4000ms
  * @param  filter     滤波器系数
  *                    0: 关闭
  *                    1: 2
  *                    2: 4
  *                    3: 8
  *                    4: 16
  * @retval 无
  */
void SPI_BMP280_SetConfig(uint8_t t_standby, uint8_t filter)
{
    uint8_t config_temp = 0;

    // 限制参数范围
    t_standby &= 0x07;
    filter &= 0x07;

    // 组合配置
    config_temp = (t_standby << 5) | (filter << 2);
	
    SPI_BMP280_WriteBuffer(BMP280_REGADDR_CONFIG, &config_temp, 1);
}

/**
  * @brief  工作模式设置
  * @param  mode  工作模式
  *               0: 睡眠模式
  *               1-2: 单次测量模式（测量完成后回到休眠）
  *               3: 连续测量模式
  * @retval 无
  */
void SPI_BMP280_SetPowerMode(uint8_t mode)
{
		// 读取 CTRL_MEAS 寄存器的当前值（包含工作模式、过采样设置等）
    uint8_t mode_temp = SPI_BMP280_ReadReg(BMP280_REGADDR_CTRL_MEAS_POWER_MODE);

    // 清除原工作模式位(bit1~0)
    mode_temp &= 0xFC;

    // 写入新模式
    mode_temp |= (mode & 0x03);

		// 将修改后的工作模式写回 CTRL_MEAS 寄存器，更新传感器工作状态
    SPI_BMP280_WriteBuffer(BMP280_REGADDR_CTRL_MEAS_POWER_MODE, &mode_temp, 1);
}

/**
  * @brief  SPI_BMP280 初始化
  * @param  无
  * @retval 无
  */
void SPI_BMP280_Init(void)
{
    MX_SPI1_Init();                // 初始化 SPI1
    BMP280_CSB_GPIO_Init();        // 初始化 BMP280 CS 引脚

    // 检测设备 ID
    while (1)
    {
        if (BMP280_CHIP_ID == SPI_BMP280_ReadID())
        {
            printf("\r\n 检测到 BMP280 !\r\n");
            printf("BMP280_ID：0x%X\r\n", BMP280_CHIP_ID);
            break;
        }
    }

    // 软件复位
    SPI_BMP280_reset();

    // 读取校准系数
    SPI_BMP280_GetCalibration(&bmp280_calib);

    // 设置过采样
    while (SPI_BMP280_SetOverSampling(5, BMP280_OVERSAMPLING_8X));

    // 设置滤波器和采样间隔
    SPI_BMP280_SetConfig(BMP280_CONFIG_STADBY_125_5, 4);

    // 设置电源模式为连续模式
    SPI_BMP280_SetPowerMode(BMP280_POWER_MODE_NORMAL);
}

/**
  * @brief  读取 BMP280 原始气压与温度 ADC 数据
  * @param  sensor_data：存储传感器数据的结构体指针
  * @note   从寄存器 0xF7 开始连续读取 6 字节数据
  * @retval 无
  */
void BMP280_ReadRawData(BMP280_SensorData *sensor_data)
{
    uint8_t buffer[6] = {0};    // 定义缓存数组，用于存放读取的寄存器数据

    // 读取 BMP280 寄存器数据（0xF7 开始，共 6 个字节）
    SPI_BMP280_ReadBuffer(BME280_REGADDR_DATA, buffer, 6);

    // 解析气压（20 位数据，对应寄存器 0xF7~0xF9）
    sensor_data->pressure_adc = (int32_t)((buffer[0] << 12) | (buffer[1] << 4) | (buffer[2] >> 4));

    // 解析温度（20 位数据，对应寄存器 0xFA~0xFC）
    sensor_data->temperature_adc = (int32_t)((buffer[3] << 12) | (buffer[4] << 4) | (buffer[5] >> 4));
}


/**
  * @brief  温度补偿计算（返回单位为 0.01°C）
  * @param  sensor_data：存储传感器数据的结构体指针
  * @note   补偿计算公式来源于 BMP280 数据手册
  * @retval 无
  */
void BMP280_Compensate_T(BMP280_SensorData *sensor_data) 
{
    double var1, var2;

    // 计算温度补偿中间值 var1
    var1 = (((double)sensor_data->temperature_adc) / 16384.0 - ((double)bmp280_calib.dig_t1) / 1024.0) *
           ((double)bmp280_calib.dig_t2);

    // 计算温度补偿中间值 var2
    var2 = ((((double)sensor_data->temperature_adc) / 131072.0 - ((double)bmp280_calib.dig_t1) / 8192.0) *
            (((double)sensor_data->temperature_adc) / 131072.0 - ((double)bmp280_calib.dig_t1) / 8192.0)) *
            ((double)bmp280_calib.dig_t3);

    // 计算 t_fine（气压补偿计算依赖该值）
    sensor_data->t_fine = (int32_t)(var1 + var2);

    // 计算实际温度（单位：摄氏度）
    sensor_data->temperature = (var1 + var2) / 5120.0;

    // 转换为 0.01°C 单位
    sensor_data->temperature = sensor_data->temperature * 100;
}


/**
  * @brief  气压补偿计算（返回单位为 Pa）
  * @param  sensor_data：存储传感器数据的结构体指针
  * @note   必须先计算温度，得到 t_fine 后才能计算气压
  * @retval 无
  */
void BMP280_Compensate_P(BMP280_SensorData *sensor_data) 
{
    double var1, var2;

    // 计算气压补偿中间值 var1、var2
    var1 = ((double)sensor_data->t_fine / 2.0) - 64000.0;
    var2 = var1 * var1 * ((double)bmp280_calib.dig_p6) / 32768.0;
    var2 = var2 + var1 * ((double)bmp280_calib.dig_p5) * 2.0;
    var2 = (var2 / 4.0) + (((double)bmp280_calib.dig_p4) * 65536.0);

    var1 = (((double)bmp280_calib.dig_p3) * var1 * var1 / 524288.0 +
            ((double)bmp280_calib.dig_p2) * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0) * ((double)bmp280_calib.dig_p1);

    // 计算实际气压值（Pa）
    sensor_data->pressure = 1048576.0 - (double)sensor_data->pressure_adc;
    sensor_data->pressure = (sensor_data->pressure - (var2 / 4096.0)) * 6250.0 / var1;

    // 气压补偿后修正值
    var1 = ((double)bmp280_calib.dig_p9) * sensor_data->pressure * sensor_data->pressure / 2147483648.0;
    var2 = sensor_data->pressure * ((double)bmp280_calib.dig_p8) / 32768.0;

    // 得到最终气压值（Pa）
    sensor_data->pressure = sensor_data->pressure + (var1 + var2 + ((double)bmp280_calib.dig_p7)) / 16.0;
}


/*********************************************END OF FILE**********************/

#include "max30102/bsp_max30102.h"
#include "led/bsp_led.h"
#include "usart/bsp_usart.h"
#include "i2c/bsp_i2c.h"




uint8_t max30102_int_flag=0;

void MAX30102_INT_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin : MAX30102_INT_Pin */
  GPIO_InitStruct.Pin = MAX30102_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(MAX30102_INT_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}
/**
  * @brief  GPIO EXTI 中断回调函数（由 HAL 自动调用）
  * @param  GPIO_Pin: 产生中断的引脚编号
  * @retval 无
  * @note   此函数在 HAL_GPIO_EXTI_IRQHandler() 中被调用，用于处理中断事件。
  *         此处用于检测 MAX30102 的中断引脚（PB8），并设置数据读取标志位。
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == MAX30102_INT_Pin)
    {
        // 设置中断标志位，主循环中读取 MAX30102 数据时使用
        max30102_int_flag = 1;

        // 翻转红色 LED，作为中断触发指示（调试用）
        R_LED_TOGGLE();
    }
}
/**
  * @brief  检测I2C总线MAX30102从设备
  * @param  hi2c：I2C句柄指针
  * @param  slave_addr：从设备地址（7位地址左移1位）
  * @retval SUCCESS：存在，ERROR：不存在
  */
ErrorStatus MAX30102_CheckDevice(void)
{
    if (HAL_I2C_IsDeviceReady(&hi2c2, (MAX30102_SLAVE_ADDR << 1) , 1, HAL_MAX_DELAY) == HAL_OK)
    {
        printf("MAX30102 通信正常\r\n");
        return SUCCESS;
    }
    else
    {
        printf("MAX30102 通信失败\r\n");
        return ERROR;
    }
}
/**
  * @brief  向 MAX30102 写入一个寄存器和一个数据
  * @param  hi2c：I2C 句柄
  * @param  reg：寄存器地址（命令）
  * @param  data：写入数据
  * @retval SUCCESS 写入成功，ERROR 写入失败
  */
ErrorStatus MAX30102_WriteByte(uint8_t reg, uint8_t data)
{
    if (HAL_I2C_Mem_Write(&hi2c2, MAX30102_WRITE_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY) == HAL_OK)
    {
        return SUCCESS;
    }
    else
    {
        printf("MAX30102 写入寄存器 0x%02X 失败\r\n", reg);
        return ERROR;
    }
}
/**
  * @brief  向 MAX30102 写入多个字节数据
  * @param  hi2c：I2C 句柄
  * @param  reg：起始寄存器地址（命令）
  * @param  buffer：数据缓冲区指针
  * @param  len：要写入的字节数
  * @retval SUCCESS 写入成功，ERROR 写入失败
  */
ErrorStatus MAX30102_WriteBuffer(uint8_t reg, uint8_t *buffer, uint16_t len)
{
    if (HAL_I2C_Mem_Write(&hi2c2, MAX30102_WRITE_ADDR, reg, I2C_MEMADD_SIZE_8BIT, buffer, len, HAL_MAX_DELAY) == HAL_OK)
    {
        return SUCCESS;
    }
    else
    {
        printf("MAX30102 多字节写入失败\r\n");
        return ERROR;
    }
}

/**
  * @brief  从 MAX30102 指定寄存器读取多个字节
  * @param  hi2c：I2C 句柄
  * @param  addr：起始寄存器地址
  * @param  rbuffer：读取数据存放缓冲区指针
  * @param  len：读取字节数
  * @retval SUCCESS：读取成功，ERROR：读取失败
  */
ErrorStatus MAX30102_ReadBuffer(uint8_t addr, uint8_t *rbuffer, uint16_t len)
{
    if (HAL_I2C_Mem_Read(&hi2c2, MAX30102_READ_ADDR, addr, I2C_MEMADD_SIZE_8BIT, rbuffer, len, HAL_MAX_DELAY) == HAL_OK)
    {
        return SUCCESS;
    }
    else
    {
        printf("MAX30102 多字节读取失败\r\n");
        return ERROR;
    }
}

/**
  * @brief  初始化 MAX30102
  * @param  无
  * @retval 无
  */
void MAX30102_Init(void)
{
    uint8_t data;

		MX_I2C2_Init();				//初始化IIC
	
    HAL_Delay(500);  // 等待 500ms，确保电源上电稳定

    // 检测设备是否连接，直到检测成功
    while (MAX30102_CheckDevice() == ERROR)
    {
        HAL_Delay(300);  //  等待检测成功300ms
    }

    MAX30102_INT_Init();  // 初始化中断引脚

    // 发送复位命令，直到成功
    while (MAX30102_WriteByte(MODE_CONFIGURATION, 0x40) != SUCCESS);
    HAL_Delay(50);  // 等待 50ms，确保复位完成

    // 配置中断：启用 FIFO 满中断，关闭温度中断
    while (MAX30102_WriteByte(INTERRUPT_ENABLE1, 0x80) != SUCCESS);
    while (MAX30102_WriteByte(INTERRUPT_ENABLE2, 0x00) != SUCCESS);

    // 初始化 FIFO 指针和计数器
    while (MAX30102_WriteByte(FIFO_WR_POINTER,     0x00) != SUCCESS);  // 写指针清零
    while (MAX30102_WriteByte(FIFO_OV_COUNTER,     0x00) != SUCCESS);  // 溢出计数器清零
    while (MAX30102_WriteByte(FIFO_RD_POINTER,     0x00) != SUCCESS);  // 读指针清零

    // 配置 FIFO：平均 4 次、不更新 FIFO 直到读完、FIFO 满才中断
    while (MAX30102_WriteByte(FIFO_CONFIGURATION,  0x40) != SUCCESS);

    // 设置工作模式为 SpO2 模式
    while (MAX30102_WriteByte(MODE_CONFIGURATION,  0x03) != SUCCESS);

    // 配置 SpO2 模式参数，ACD分辨率：15.63pA，采样率控制：200Hz，LED脉冲宽度：215us 
    while (MAX30102_WriteByte(SPO2_CONFIGURATION,  0x2A) != SUCCESS);

    // 设置 LED 电流（脉冲幅度）
    while (MAX30102_WriteByte(LED1_PULSE_AMPLITUDE, 0x2F) != SUCCESS);  // IR LED
    while (MAX30102_WriteByte(LED2_PULSE_AMPLITUDE, 0x2F) != SUCCESS);  // RED LED

    // 禁用温度测量
    while (MAX30102_WriteByte(TEMPERATURE_CONFIG,  0x00) != SUCCESS);

    // 读取中断状态寄存器，清除上电时的中断标志
    while (MAX30102_ReadBuffer(INTERRUPT_STATUS1, &data, 1) != SUCCESS);
}

/**
  * @brief  读取 FIFO 中的数据
  * @param  *output_data：用于存放读取结果的指针（长度至少为2的 float 数组）
  * @retval 无
  */
void MAX30102_fifo_read(float *output_data)
{
    uint8_t receive_data[6];  // 存储从 FIFO 中读取的 6 字节原始数据
    uint32_t data[2];         // 存储两个 18 位采样数据（IR 和 RED）

    // 从 FIFO 寄存器读取 6 字节数据（每个通道 3 字节）
    MAX30102_ReadBuffer(FIFO_DATA, receive_data, 6);

    // 解析 IR 通道数据，取高 18 位有效数据（bit23~bit6）
    data[0] = ((receive_data[0] << 16) | (receive_data[1] << 8) | receive_data[2]) & 0x03FFFF;

    // 解析 RED 通道数据，取高 18 位有效数据（bit23~bit6）
    data[1] = ((receive_data[3] << 16) | (receive_data[4] << 8) | receive_data[5]) & 0x03FFFF;

    // 存储为浮点数格式
    *output_data       = data[0];
    *(output_data + 1) = data[1];
}

/**
  * @brief  获取心率数据
  * @param  *input_data：存放 IR 数据的指针
  * @param  cache_nums：缓存数据的数量
  * @retval 心率值（单位：次/分钟），若无效则返回 0
  */
uint16_t MAX30102_getHeartRate(float *input_data, uint16_t cache_nums)
{
    float input_data_sum_aver = 0;  // IR 数据的平均值
    uint16_t i, temp;

    // 计算所有数据的平均值
    for (i = 0; i < cache_nums; i++)
    {
        input_data_sum_aver += *(input_data + i);
    }
    input_data_sum_aver = input_data_sum_aver / cache_nums;

    // 寻找第一个峰值后下降（判断一次心跳的起始）
    for (i = 0; i < cache_nums; i++)
    {
        if ((*(input_data + i) > input_data_sum_aver) && (*(input_data + i + 1) < input_data_sum_aver))
        {
            temp = i;  // 记录第一次心跳峰位置
            break;
        }
    }

    i++;  // 从下一个点继续寻找第二次心跳
    for (; i < cache_nums; i++)
    {
        if ((*(input_data + i) > input_data_sum_aver) && (*(input_data + i + 1) < input_data_sum_aver))
        {
            temp = i - temp;  // 计算两次心跳之间的采样点数
            break;
        }
    }

    // 若计算结果在合理范围内，换算出心率（单位：次/分钟）
    if ((temp > 14) && (temp < 100))
    {
        // 采样频率为 200Hz，平均 4 次后有效采样频率为 50Hz
        // 一次心跳持续 temp / 50 秒，则每分钟心跳数为 60 / (temp / 50) = 3000 / temp
        return 3000 / temp;
    }
    else
    {
        return 0;  // 无法识别有效心跳
    }
}

/**
  * @brief  获取血氧饱和度（SpO2）数据
  * @param  *ir_input_data：存放 IR 数据的指针
  * @param  *red_input_data：存放 RED 数据的指针
  * @param  cache_nums：数据数量
  * @retval SpO2 值（百分比）
  */
float MAX30102_getSpO2(float *ir_input_data, float *red_input_data, uint16_t cache_nums)
{
    float ir_max = *ir_input_data, ir_min = *ir_input_data;       // IR 信号最大值和最小值
    float red_max = *red_input_data, red_min = *red_input_data;   // RED 信号最大值和最小值
    float R;
    uint16_t i;

    // 遍历数据，找出 IR 和 RED 的最大值与最小值
    for (i = 1; i < cache_nums; i++)
    {
        if (ir_max < *(ir_input_data + i))
        {
            ir_max = *(ir_input_data + i);
        }
        if (ir_min > *(ir_input_data + i))
        {
            ir_min = *(ir_input_data + i);
        }
        if (red_max < *(red_input_data + i))
        {
            red_max = *(red_input_data + i);
        }
        if (red_min > *(red_input_data + i))
        {
            red_min = *(red_input_data + i);
        }
    }

    // 计算比值 R，用于 SpO2 计算
    R = ((ir_max - ir_min) * red_min) / ((red_max - red_min) * ir_min);

		//经典线性公式（经典的简化模型，计算简单，误差偏大）
		//return	( 104 - 1.7 * R );
		
    // 使用 Maxim 官方提供的经验公式计算 SpO2 并返回
    return ( (-45.060f) * R * R + 30.354f * R + 94.845f );
}

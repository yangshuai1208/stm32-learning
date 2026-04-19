#include "mpu6050/bsp_i2c_mpu6050.h" 
#include "i2c/bsp_i2c.h"
#include "led/bsp_led.h"
#include "usart/bsp_usart.h"

uint8_t mpu6050_int_flag = 0;                 // mpu6050 中断标志

/**
  * @brief  MPU6050 所需 GPIO 初始化
  * @note   包括 AD0 引脚和中断引脚配置
  * @retval 无
  */
void MPU6050_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* 使能 GPIOB 时钟 */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* 设置 AD0 引脚初始电平为低 */
  HAL_GPIO_WritePin(MPU6050_AD0_GPIO_Port, MPU6050_AD0_Pin, GPIO_PIN_RESET);

  /* 配置 AD0 引脚为推挽输出 */
  GPIO_InitStruct.Pin = MPU6050_AD0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(MPU6050_AD0_GPIO_Port, &GPIO_InitStruct);

  /* 配置 INT 引脚为下降沿中断输入 */
  GPIO_InitStruct.Pin = MPU6050_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(MPU6050_INT_GPIO_Port, &GPIO_InitStruct);

  /* 使能 EXTI 中断 */
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}


/**
  * @brief  GPIO EXTI 中断回调函数（由 HAL 自动调用）
  * @param  GPIO_Pin: 产生中断的引脚编号
  * @retval 无
  * @note   此函数在 HAL_GPIO_EXTI_IRQHandler() 中被调用，用于处理中断事件。
  *         此处用于检测 MPU6050 的中断引脚（PB8），并设置数据读取标志位。
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == MPU6050_INT_Pin)
    {
        // 设置中断标志位，主循环中读取 MPU6050 数据时使用
        mpu6050_int_flag = 1;

        // 翻转红色 LED，作为中断触发指示（调试用）
        R_LED_TOGGLE();
    }
}

/**
  * @brief  写入 MPU6050 指定寄存器的值（单字节）
  * @param  write_addr：写入地址
  * @param  byte：具体数据
  * @retval HAL 状态（HAL_OK表示成功，其他表示失败）
  */
HAL_StatusTypeDef MPU6050_WriteReg(uint8_t write_addr, uint8_t byte)
{
    uint8_t data[2];
    data[0] = write_addr;
    data[1] = byte;
    // 使用 HAL_I2C_Master_Transmit 发送寄存器地址和数据
    if (HAL_I2C_Master_Transmit(&hi2c2, MPU6050_AD0D_SLAVER_ARRD << 1, data, 2, 1000) == HAL_OK)
    {
        return HAL_OK;
    }
    else
    {
        return HAL_ERROR;
    }
}

/**
  * @brief  读取 MPU6050 指定寄存器的值（单字节）
  * @param  read_addr：读取地址
  * @retval 读取到的数据
  */
uint8_t MPU6050_ReadReg(uint8_t read_addr)
{
    uint8_t data = 0;
    // 先发送寄存器地址
    if (HAL_I2C_Master_Transmit(&hi2c2, MPU6050_AD0D_SLAVER_ARRD << 1, &read_addr, 1, 1000) != HAL_OK)
    {
        return 0;
    }
    // 读取寄存器数据
    if (HAL_I2C_Master_Receive(&hi2c2, MPU6050_AD0D_SLAVER_ARRD << 1, &data, 1, 1000) != HAL_OK)
    {
        return 0;
    }
    return data;
}

/**
  * @brief  连续读取 MPU6050 寄存器数据
  * @param  read_addr：起始读取地址
  * @param  buffer：存储数据的指针
  * @param  size：读取长度
  * @retval HAL 状态
  */
HAL_StatusTypeDef MPU6050_ReadBuffer(uint8_t read_addr, uint8_t* buffer, uint16_t size)
{
    // 发送起始寄存器地址
    if (HAL_I2C_Master_Transmit(&hi2c2, MPU6050_AD0D_SLAVER_ARRD << 1, &read_addr, 1, 1000) != HAL_OK)
    {
        return HAL_ERROR;
    }
    // 读取指定长度数据
    if (HAL_I2C_Master_Receive(&hi2c2, MPU6050_AD0D_SLAVER_ARRD << 1, buffer, size, 1000) != HAL_OK)
    {
        return HAL_ERROR;
    }
    return HAL_OK;
}

/**
  * @brief  连续写入 MPU6050 寄存器数据
  * @param  write_addr：起始写入地址
  * @param  buffer：数据指针
  * @param  size：写入长度
  * @retval HAL 状态
  */
HAL_StatusTypeDef MPU6050_WriteBuffer(uint8_t write_addr, uint8_t* buffer, uint16_t size)
{
    // 动态申请 size + 1 字节内存（首字节用于写入地址）
    uint8_t *data = malloc(size + 1);
    if (data == NULL)
    {
        // 内存申请失败
        return HAL_ERROR;
    }

    // 第一个字节为寄存器地址
    data[0] = write_addr;

    // 将外部数据复制到 data 中（从 data[1] 开始）
    memcpy(&data[1], buffer, size);

    // 通过 I2C 发送 data 数据，长度为 size+1
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(&hi2c2, MPU6050_AD0D_SLAVER_ARRD << 1, data, size + 1, 1000);

    // 释放申请的内存
    free(data);

    // 返回发送状态
    return status;
}


/**
  * @brief  读取 MPU6050 ID
  * @retval MPU6050 WHO_AM_I 寄存器值
  */
uint8_t MPU6050_ReadID(void)
{
    // 读取 WHO_AM_I 寄存器内容
    return MPU6050_ReadReg(MPU6050_WHO_AM_I);
}

/**
  * @brief  清除 MPU6050 中断标志（通过读取 INT_STATUS 寄存器）
  * @retval 无
  */
void MPU6050_ClearInterrupt(void)
{
    // 读取 INT_STATUS 寄存器，读取即清除中断标志
    uint8_t int_status = MPU6050_ReadReg(MPU6050_RA_INT_STATUS);
}

/**
  * @brief  初始化 MPU6050
  * @retval 无
  */
void MPU6050_Init(void)
{
    HAL_Delay(1000); // 延时1秒，确保设备上电稳定
	
		MX_I2C2_Init();	//初始化 MPU6050 硬件 I2C 引脚
	
		MPU6050_GPIO_Init();// 初始化 MPU6050 使用的 GPIO 引脚
	
    while (MPU6050_ReadID() != MPU6050_ID)
    {
        printf("未检测到 MPU6050，请检查连接！\r\n");
        HAL_Delay(500);
    }
    printf("检测到 MPU6050，ID:0x%X\r\n", MPU6050_ID);

    MPU6050_WriteReg(MPU6050_RA_PWR_MGMT_1, 0x00);    // 解除休眠
    MPU6050_WriteReg(MPU6050_RA_SMPLRT_DIV, 0x07);    // 设置采样率为1kHz/(7+1)=125Hz
    MPU6050_WriteReg(MPU6050_RA_CONFIG, 0x06);        // 配置低通滤波，带宽5Hz
    MPU6050_WriteReg(MPU6050_RA_ACCEL_CONFIG, 0x00);  // 加速度计2G模式
    MPU6050_WriteReg(MPU6050_RA_GYRO_CONFIG, 0x18);   // 陀螺仪±2000°/s
		MPU6050_WriteReg(MPU6050_RA_INT_ENABLE, 0x01); 		// 使能数据准备好中断
		MPU6050_WriteReg(MPU6050_RA_INT_PIN_CFG, 0xA0);		// 配置中断引脚：低电平有效，推挽输出，保持模式，读取中断清除
}

/**
  * @brief  读取 MPU6050 加速度数据
  * @param  AccData：存放加速度数据的数组指针，长度至少为3
  * @retval 无
  */
void MPU6050_ReadAcc(short *AccData)
{
    uint8_t buf[6];  // 用于存放从MPU6050读取的6个字节加速度数据
    if (MPU6050_ReadBuffer(MPU6050_ACC_OUT, buf, 6) == HAL_OK)
    {
        // 将高低字节合并为16位有符号整数
        AccData[0] = (int16_t)((buf[0] << 8) | buf[1]);  // X轴加速度
        AccData[1] = (int16_t)((buf[2] << 8) | buf[3]);  // Y轴加速度
        AccData[2] = (int16_t)((buf[4] << 8) | buf[5]);  // Z轴加速度
    }
}

/**
  * @brief  读取 MPU6050 陀螺仪数据
  * @param  GyroData：存放角速度数据的数组指针，长度至少为3
  * @retval 无
  */
void MPU6050_ReadGyro(short *GyroData)
{
    uint8_t buf[6];  // 用于存放从MPU6050读取的6个字节角速度数据
    if (MPU6050_ReadBuffer(MPU6050_GYRO_OUT, buf, 6) == HAL_OK)
    {
        // 将高低字节合并为16位有符号整数
        GyroData[0] = (int16_t)((buf[0] << 8) | buf[1]);  // X轴角速度
        GyroData[1] = (int16_t)((buf[2] << 8) | buf[3]);  // Y轴角速度
        GyroData[2] = (int16_t)((buf[4] << 8) | buf[5]);  // Z轴角速度
    }
}

/**
  * @brief  读取 MPU6050 温度原始数据
  * @param  TempData：存放温度原始值的指针
  * @retval 无
  */
void MPU6050_ReadTemp(short *TempData)
{
    uint8_t buf[2];  // 用于存放从MPU6050读取的2个字节温度数据
    if (MPU6050_ReadBuffer(MPU6050_RA_TEMP_OUT_H, buf, 2) == HAL_OK)
    {
        // 将高低字节合并为16位有符号整数
        *TempData = (int16_t)((buf[0] << 8) | buf[1]);
    }
}

/**
  * @brief  读取 MPU6050 温度并转换为摄氏度
  * @param  Temperature：存放摄氏度值的指针
  * @retval 无
  */
void MPU6050_ReturnTemp(float *Temperature)
{
    short tempRaw;  // 原始温度值
    MPU6050_ReadTemp(&tempRaw);  // 读取原始温度
    *Temperature = (float)tempRaw / 340.0f + 36.53f;  // 转换为摄氏温度
}

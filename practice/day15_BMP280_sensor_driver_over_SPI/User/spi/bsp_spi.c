
  
#include "spi/bsp_spi.h"
	
SPI_HandleTypeDef hspi1;  // 定义 SPI1 句柄结构体，用于管理 SPI 配置和状态

/**
  * @brief  初始化 SPI1 外设
  * @note   配置为主机模式，8 位数据帧，低空闲电平，第一边沿采样
  *         用于与 BMP280 等 SPI 设备通信
  */
void MX_SPI1_Init(void)
{
  hspi1.Instance = SPI1;                           // 选择 SPI1 外设
  hspi1.Init.Mode = SPI_MODE_MASTER;               // 配置为主机模式
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;     // 全双工（2线）通信
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;          // 数据长度为 8 位
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;        // 时钟空闲时为低电平
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;            // 第一个时钟边沿采样
  hspi1.Init.NSS = SPI_NSS_SOFT;                    // 软件管理片选信号
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4; // SCK 时钟 = PCLK / 4
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;           // 高位先传输
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;           // 禁用 TI 协议模式
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE; // 禁用 CRC 校验
  hspi1.Init.CRCPolynomial = 10;                    // CRC 多项式（未使用）

  // 初始化 SPI1，失败则进入错误处理
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  SPI1 硬件底层初始化
  * @note   配置 SPI1 的 GPIO 引脚和外设时钟
  * @param  spiHandle SPI 句柄
  */
void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if (spiHandle->Instance == SPI1)
  {
    // 1. 使能 SPI1 外设时钟
    __HAL_RCC_SPI1_CLK_ENABLE();

    // 2. 使能 GPIOA 时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /**
      * SPI1 引脚功能映射
      * PA5 -> SPI1_SCK   (BMP280_SCK_Pin)
      * PA6 -> SPI1_MISO  (BMP280_MISO_Pin)
      * PA7 -> SPI1_MOSI  (BMP280_MOSI_Pin)
      */
    GPIO_InitStruct.Pin = BMP280_SCK_Pin | BMP280_MOSI_Pin; // SCK 和 MOSI
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;                 // 复用推挽输出
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;           // 高速模式
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = BMP280_MISO_Pin;                   // MISO
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;                  // 输入模式
    GPIO_InitStruct.Pull = GPIO_NOPULL;                      // 无上下拉
    HAL_GPIO_Init(BMP280_MISO_GPIO_Port, &GPIO_InitStruct);
  }
}

/**
  * @brief  SPI1 硬件底层反初始化
  * @note   关闭 SPI1 时钟并释放引脚
  * @param  spiHandle SPI 句柄
  */
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{
  if (spiHandle->Instance == SPI1)
  {
    // 1. 关闭 SPI1 外设时钟
    __HAL_RCC_SPI1_CLK_DISABLE();

    // 2. 释放 GPIO 配置
    HAL_GPIO_DeInit(GPIOA, BMP280_SCK_Pin | BMP280_MISO_Pin | BMP280_MOSI_Pin);
  }
}


/******************************** END OF FILE *********************************/

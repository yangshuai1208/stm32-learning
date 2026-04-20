

#include "bmp280/app_bmp280.h"
#include "bmp280/bsp_spi_bmp280.h" 

/* BMP280 传感器数据结构体变量初始化为 0 */
BMP280_SensorData bmp280_data = {0};

/**
  * @brief  BMP280 传感器任务函数，每 500ms 读取一次数据并打印
  * @param  无
  * @retval 无
  */
void BMP280_Task(void)
{
    /* 初始化 BMP280 SPI 接口与配置 */
    SPI_BMP280_Init();

    while (1)
    {
        /* 读取 BMP280 原始数据（气压与温度 ADC 值） */
        BMP280_ReadRawData(&bmp280_data);

        /* 温度补偿计算 */
        BMP280_Compensate_T(&bmp280_data);

        /* 气压补偿计算 */
        BMP280_Compensate_P(&bmp280_data);
        
        /* 打印温度数据（单位：℃，保留 2 位小数） */
        printf("\r\n温度: %.2f ℃\r\n", bmp280_data.temperature / 100.0);

        /* 打印气压数据（单位：KPa，保留 3 位小数） */
        printf("气压: %.3f KPa\r\n", bmp280_data.pressure / 1000.0);

        /* 延时 1000ms */
        HAL_Delay(1000); 
    }
}


/*****************************END OF FILE***************************************/

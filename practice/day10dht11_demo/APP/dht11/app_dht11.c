#include "dht11/app_dht11.h"
#include "dht11/bsp_dht11.h" 


static DHT11_DATA_TYPEDEF dht11_data = {0};

/**
  * @brief  读取DHT11传感器数据并打印结果
  * @param  无
  * @retval 读取成功返回1，失败返回0
  */
void Dht11_ReadAndPrint(void)
{
    if(DHT11_ReadData(&dht11_data) == HAL_OK)
    {
				printf("当前数据传输校验正确：");
			
        if(dht11_data.humi_deci & 0x80) // 湿度负数判断（一般DHT11无负湿度，保留）
        {
            printf("湿度为 -%d.%d %%RH，", dht11_data.humi_int, dht11_data.humi_deci);
        }
        else
        {
            printf("湿度为 %d.%d %%RH，", dht11_data.humi_int, dht11_data.humi_deci);
        }

        if(dht11_data.temp_deci & 0x80) // 温度负数判断
        {
            printf("温度为 -%d.%d ℃\r\n", dht11_data.temp_int, dht11_data.temp_deci);
        }
        else
        {
            printf("温度为 %d.%d ℃\r\n", dht11_data.temp_int, dht11_data.temp_deci);
        }
    }
    else
    {
        printf("读取DHT11数据错误！\r\n"); 
    }
}
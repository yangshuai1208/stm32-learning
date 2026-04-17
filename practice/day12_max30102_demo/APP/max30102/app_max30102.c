#include "max30102/app_max30102.h"
#include "max30102/bsp_max30102.h"
#include "max30102/max30102_fir.h"
#include "led/bsp_led.h"

uint16_t cache_counter = 0;  	//缓存计数器，表示当前已缓存的有效数据组数
float max30102_data[2];  			//原始采样数据：max30102_data[0]为IR红外，max30102_data[1]为RED红光
float fir_output[2];  				//FIR滤波器输出数据：fir_output[0]为IR滤波结果，fir_output[1]为RED滤波结果
float ppg_data_cache_RED[CACHE_NUMS] = {0};  //RED通道缓存区，保存RED滤波结果用于血氧计算
float ppg_data_cache_IR[CACHE_NUMS] = {0};  //IR通道缓存区，保存IR滤波结果用于心率与血氧计算


/**
  * @brief  MAX30102 处理函数
  *         检测中断标志，读取 FIFO 数据，滤波处理，缓存有效数据，
  *         满足条件时计算心率和血氧值并打印，同时控制状态指示灯。
  * @param  无
  * @retval 无
  */
void MAX30102_Task(void)
{
    uint8_t data;  // 用于临时存储中断状态寄存器读取结果

    // 判断是否接收到中断标志（通常由中断回调设置 int_flag 为 1）
    if (max30102_int_flag == 1)
    {
        // 清除中断标志，准备开始处理数据
        max30102_int_flag = 0;

        // 连续读取 32 组 FIFO 数据（每组包括 1 个红外 IR 和 1 个红光 RED 原始值）
        for (int i = 0; i < 32; i++)
        {
            // 从 MAX30102 的 FIFO 寄存器读取一组原始数据（IR 和 RED）
            MAX30102_fifo_read(max30102_data);

            // 对读取到的 IR 信号应用 FIR 滤波器，结果存入 fir_output[0]
            MAX30102_IR_FIR(&max30102_data[0], &fir_output[0]);

            // 对读取到的 RED 信号应用 FIR 滤波器，结果存入 fir_output[1]
            MAX30102_RED_FIR(&max30102_data[1], &fir_output[1]);

            // 判断滤波后的 IR 和 RED 是否大于设定阈值（保证信号强度足够）
            if ((fir_output[0] > PPG_DATA_THRESHOLD) && (fir_output[1] > PPG_DATA_THRESHOLD))
            {
                // 将当前有效的 IR 数据缓存到 IR 数据数组
                ppg_data_cache_IR[cache_counter] = fir_output[0];

                // 将当前有效的 RED 数据缓存到 RED 数据数组
                ppg_data_cache_RED[cache_counter] = fir_output[1];

                // 缓存索引自增，准备缓存下一组数据
                cache_counter++;

                // 打开绿色 LED，表示当前采集到了有效数据
                G_LED_ON();
            }
            else
            {
                // 熄灭绿色 LED，表示当前数据无效
                G_LED_OFF();
            }
        }

        // 判断是否已采集足够数量的数据（达到指定测试数量）
        if (cache_counter >= TEST_NUMS)
        {
            // 计算当前缓存的 IR 数据中对应的心率值（单位：次/分钟）
            uint16_t heart_rate = MAX30102_getHeartRate(ppg_data_cache_IR, TEST_NUMS);

            // 计算 IR 和 RED 数据对应的血氧饱和度（单位：百分比）
            float spo2 = MAX30102_getSpO2(ppg_data_cache_IR, ppg_data_cache_RED, TEST_NUMS);

            // 通过串口打印当前的心率和血氧值
            printf("心率：%d 次/min   血氧：%.2f %%\n", heart_rate, spo2);

            // 重置缓存索引，准备进行下一次数据采集
            cache_counter = 0;
        }

        // 读取中断状态寄存器，将中断标志位清除
        MAX30102_ReadBuffer(INTERRUPT_STATUS1, &data, 1);
    }
}

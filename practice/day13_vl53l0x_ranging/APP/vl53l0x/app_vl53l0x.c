#include "vl53l0x/app_vl53l0x.h"
#include "vl53l0x/bsp_i2c_vl53l0x.h"

/**
 * @brief  单次测量并读取一次距离数据
 * @param  dev: 设备I2C参数结构体
 * @param  pdata: 用于保存测距结果的结构体指针
 * @retval 状态信息
 */
VL53L0X_Error VL53L0X_Task(VL53L0X_Dev_t *dev)
{
    VL53L0X_Error status;
    char buf[VL53L0X_MAX_STRING_LENGTH] = {0};
    uint32_t timeout = 0;
		VL53L0X_RangingMeasurementData_t data;
		
    Vl53l0x_int_flag = 0;  // 清标志，准备等待新数据

    // 启动单次测量
    status = VL53L0X_StartMeasurement(dev);
    if (status != VL53L0X_ERROR_NONE)
        return status;

    // 等待中断标志位，表示测量完成
    while (Vl53l0x_int_flag == 0)
    {
        if (timeout++ > 1000)  // 超时保护，避免死循环
            return VL53L0X_ERROR_TIME_OUT;
        HAL_Delay(5);
    }

    // 读取测距数据
    status = VL53L0X_GetRangingMeasurementData(dev, &data);
    if (status != VL53L0X_ERROR_NONE)
        return status;

    // 获取状态字符串
    VL53L0X_GetRangeStatusString(data.RangeStatus, buf);

    // 清除中断标志，准备下一次测量
    status = VL53L0X_ClearInterruptMask(dev, 0);
    if (status != VL53L0X_ERROR_NONE)
        return status;

    // 打印测量结果
    printf("距离: %4d mm, 状态: %s\r\n", data.RangeMilliMeter, buf);

    return VL53L0X_ERROR_NONE;
}
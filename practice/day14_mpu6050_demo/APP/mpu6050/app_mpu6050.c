#include "mpu6050/app_mpu6050.h"
#include "usart/bsp_usart.h"
#include "mpu6050/bsp_i2c_mpu6050.h" 

/**
 * @brief  MPU6050读取数据并打印任务函数
 * @note   在检测到MPU6050中断标志时，读取加速度、陀螺仪和温度数据，
 *         并通过串口打印输出，同时清除中断标志。
 */
void MPU6050_Task(void)
{
    if (mpu6050_int_flag)               // 判断 MPU6050 中断标志是否置位
    {
        mpu6050_int_flag = 0;           // 清除中断标志

        short acc[3] = {0};             // 存放加速度原始数据的数组
        short gyro[3] = {0};            // 存放陀螺仪原始数据的数组
        short tempRaw = 0;              // 存放温度原始数据
        float temperature = 0.0f;       // 存放计算后的温度值（摄氏度）

        MPU6050_ReadAcc(acc);           // 读取加速度数据
        MPU6050_ReadGyro(gyro);         // 读取陀螺仪数据
        MPU6050_ReadTemp(&tempRaw);     // 读取温度原始数据
        MPU6050_ReturnTemp(&temperature);// 计算温度值并返回

        printf("加速度: X=%d Y=%d Z=%d\r\n", acc[0], acc[1], acc[2]);           // 打印加速度数据
        printf("陀螺仪: X=%d Y=%d Z=%d\r\n", gyro[0], gyro[1], gyro[2]);        // 打印陀螺仪数据
        printf("温度原始值: %d，温度: %.2f 摄氏度\r\n", tempRaw, temperature);    // 打印温度数据

        MPU6050_ClearInterrupt();       // 清除 MPU6050 中断标志
    }
}

#ifndef __BSP_SPI_BMP280_H
#define	__BSP_SPI_BMP280_H

#include "main.h"

/*
 * BMP280 的校正系数存储在以下寄存器地址中（均为十六进制）：
 *
 * 温度校正系数：0x88 ~ 0x8D (6 bytes)
 * 气压校正系数：0x8E ~ 0x9F (18 bytes)
 */

/* BMP280 校正系数结构体 */
typedef struct
{
    uint16_t dig_t1;   // 温度校正系数 T1
    int16_t  dig_t2;   // 温度校正系数 T2
    int16_t  dig_t3;   // 温度校正系数 T3

    uint16_t dig_p1;   // 气压校正系数 P1
    int16_t  dig_p2;   // 气压校正系数 P2
    int16_t  dig_p3;   // 气压校正系数 P3
    int16_t  dig_p4;   // 气压校正系数 P4
    int16_t  dig_p5;   // 气压校正系数 P5
    int16_t  dig_p6;   // 气压校正系数 P6
    int16_t  dig_p7;   // 气压校正系数 P7
    int16_t  dig_p8;   // 气压校正系数 P8
    int16_t  dig_p9;   // 气压校正系数 P9
} BMP280_CalibData;

/* BMP280 传感器数据结构体 */
typedef struct
{
    uint32_t pressure_adc;     // 原始气压 ADC 数据
    uint32_t temperature_adc;  // 原始温度 ADC 数据
    int32_t  t_fine;           // 温度补偿中间变量
    float    pressure;         // 计算后的气压值（单位：Pa）
    float    temperature;      // 计算后的温度值（单位：°C）
} BMP280_SensorData;


#define BMP280_CSB_Pin            GPIO_PIN_4                      // SPI Flash片选引脚，PA4
#define BMP280_CSB_GPIO_Port      GPIOA                           // 片选引脚所在GPIO端口

#define BMP280_CSB_LOW()          HAL_GPIO_WritePin(BMP280_CSB_GPIO_Port, BMP280_CSB_Pin, GPIO_PIN_RESET)  // 拉低片选，开始通信
#define BMP280_CSB_HIGH()         HAL_GPIO_WritePin(BMP280_CSB_GPIO_Port, BMP280_CSB_Pin, GPIO_PIN_SET)    // 拉高片选，结束通信

#define BMP280_CHIP_ID              0x58   //设备ID
#define BMP280_DUMMY_BYTE           0xFF   //时序空指令
#define BME280_SOFT_RESET_COMMAND   0xB6   //软件复位指令

#define BMP280_REGADDR_ID                       0xD0   //设备  ID  寄存器地址
#define BMP280_REGADDR_STATUS                   0xF3   //设备 状态 寄存器地址
#define BMP280_REGADDR_CTRL_MEAS                0xF4   //设备 数据采集选项 寄存器地址
#define BMP280_REGADDR_CTRL_MEAS_POWER_MODE     0xF4   //设备 电源模式  寄存器地址
#define BMP280_REGADDR_CONFIG                   0xF5   //设备 配置      寄存器地址
#define BME280_REGADDR_DATA                     0xF7   //设备 数据      寄存器地址
#define BMP280_REGADDR_SOFT_RESET               0xE0   //设备 软件复位  寄存器地址
#define BMP280_REGADDR_DIGT1_LSB                0x88   //设备 DIGT1 LSB 寄存器地址
#define BMP280_REGADDR_DIGT1_MSB                0x89   //设备 DIGT1 MSB 寄存器地址
#define BMP280_REGADDR_DIGT2_LSB                0x8A   //设备 DIGT2 LSB 寄存器地址
#define BMP280_REGADDR_DIGT2_MSB                0x8B   //设备 DIGT2 MSB 寄存器地址
#define BMP280_REGADDR_DIGT3_LSB                0x8C   //设备 DIGT3 LSB 寄存器地址
#define BMP280_REGADDR_DIGT3_MSB                0x8D   //设备 DIGT3 MSB 寄存器地址
#define BMP280_REGADDR_DIGP1_LSB                0x8E   //设备 DIGP1 LSB 寄存器地址
#define BMP280_REGADDR_DIGP1_MSB                0x8F   //设备 DIGP1 MSB 寄存器地址
#define BMP280_REGADDR_DIGP2_LSB                0x90   //设备 DIGP2 LSB 寄存器地址
#define BMP280_REGADDR_DIGP2_MSB                0x91   //设备 DIGP2 MSB 寄存器地址
#define BMP280_REGADDR_DIGP3_LSB                0x92   //设备 DIGP3 LSB 寄存器地址
#define BMP280_REGADDR_DIGP3_MSB                0x93   //设备 DIGP3 MSB 寄存器地址
#define BMP280_REGADDR_DIGP4_LSB                0x94   //设备 DIGP4 LSB 寄存器地址
#define BMP280_REGADDR_DIGP4_MSB                0x95   //设备 DIGP4 MSB 寄存器地址
#define BMP280_REGADDR_DIGP5_LSB                0x96   //设备 DIGP5 LSB 寄存器地址
#define BMP280_REGADDR_DIGP5_MSB                0x97   //设备 DIGP5 MSB 寄存器地址
#define BMP280_REGADDR_DIGP6_LSB                0x98   //设备 DIGP6 LSB 寄存器地址
#define BMP280_REGADDR_DIGP6_MSB                0x99   //设备 DIGP6 MSB 寄存器地址
#define BMP280_REGADDR_DIGP7_LSB                0x9A   //设备 DIGP7 LSB 寄存器地址
#define BMP280_REGADDR_DIGP7_MSB                0x9B   //设备 DIGP7 MSB 寄存器地址
#define BMP280_REGADDR_DIGP8_LSB                0x9C   //设备 DIGP8 LSB 寄存器地址
#define BMP280_REGADDR_DIGP8_MSB                0x9D   //设备 DIGP8 MSB 寄存器地址
#define BMP280_REGADDR_DIGP9_LSB                0x9E   //设备 DIGP9 LSB 寄存器地址
#define BMP280_REGADDR_DIGP9_MSB                0x9F   //设备 DIGP9 MSB 寄存器地址


/* 过采样的宏 */
#define BMP280_NO_OVERSAMPLING      0x00
#define BMP280_OVERSAMPLING_1X      0x01
#define BMP280_OVERSAMPLING_2X      0x02
#define BMP280_OVERSAMPLING_4X      0x03
#define BMP280_OVERSAMPLING_8X      0x04
#define BMP280_OVERSAMPLING_16X     0x05
#define BMP280_OVERSAMPLING_MAX     0xF

/* 状态的宏 */
#define BMP280_STATUS_IM_UPDATE     0x01
#define BMP280_STATUS_MEAS_DONE     0x08

/* 电源模式的宏 */
#define BMP280_POWER_MODE_SLEEP     0x00    //睡眠模式
#define BMP280_POWER_MODE_FORCED1   0x01    //单次测量模式，测量完成后回到休眠模式
#define BMP280_POWER_MODE_FORCED2   0x02    //单次测量模式，测量完成后回到休眠模式
#define BMP280_POWER_MODE_NORMAL    0x03    //连续测量模式

/* 待机时间(采样间隔时间)选择宏 */
#define BMP280_CONFIG_STADBY_0_5    0x00    //0.5ms 
#define BMP280_CONFIG_STADBY_62_5   0x01    //62.5ms
#define BMP280_CONFIG_STADBY_125_5  0x02    //125ms 
#define BMP280_CONFIG_STADBY_250    0x03    //250ms 
#define BMP280_CONFIG_STADBY_500    0x04    //500ms 
#define BMP280_CONFIG_STADBY_1000   0x05    //1000ms
#define BMP280_CONFIG_STADBY_2000   0x06    //2000ms
#define BMP280_CONFIG_STADBY_4000   0x07    //4000ms

/************ 函数声明 ************/
void BMP280_CSB_GPIO_Init(void);
uint8_t SPI_BMP280_ReadReg(uint8_t read_addr);
void SPI_BMP280_ReadBuffer(uint8_t read_addr, uint8_t* buffer, uint32_t size);
void SPI_BMP280_WriteBuffer(uint8_t write_addr, uint8_t* buffer, uint32_t size);
uint8_t SPI_BMP280_ReadID(void);
uint8_t SPI_BMP280_GetStatus(void);
void SPI_BMP280_reset(void);
void SPI_BMP280_GetCalibration(BMP280_CalibData *calib);
uint8_t SPI_BMP280_SetOverSampling(uint8_t osrs_p, uint8_t osrs_t);
void SPI_BMP280_SetConfig(uint8_t t_standby, uint8_t filter);
void SPI_BMP280_SetPowerMode(uint8_t mode);
void SPI_BMP280_Init(void);
void BMP280_ReadRawData(BMP280_SensorData *sensor_data);
void BMP280_Compensate_T(BMP280_SensorData *sensor_data);
void BMP280_Compensate_P(BMP280_SensorData *sensor_data);


#endif /* __BSP_SPI_BMP280_H */


#ifndef __APP_MAX30102_H
#define __APP_MAX30102_H

#include "main.h"

#define CACHE_NUMS    200  //缓存数量，用于存储滤波后的有效数据
#define TEST_NUMS     150  //每次计算心率和血氧所需的数据量
#define PPG_DATA_THRESHOLD 100000  //滤波后数据有效性判断阈值


/************ 函数声明 ************/
void MAX30102_Task(void);



#endif /* __APP_MAX30102_H */

#ifndef __MAX30102_FIR_H
#define __MAX30102_FIR_H

#include "main.h"
#include "arm_math.h"

#define FIR_BLOCK_SIZE  1         // 每次处理的样本数（一个数据块大小）
#define FIR_NUM_TAPS    32 + 1    // 滤波器阶数（滤波系数个数）

/************ 函数声明 ************/
void MAX30102_FIR_Init(void);
void MAX30102_IR_FIR(float *input,float *output);
void MAX30102_RED_FIR(float *input,float *output);



#endif /* __MAX30102_FIR_H */




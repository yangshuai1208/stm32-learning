#include "max30102/max30102_fir.h"

// FIR滤波器实例 - 分别用于红外（IR）和红光（RED）采样数据
arm_fir_instance_f32 fir_ir, fir_red;

// FIR滤波器状态缓存数组 - 大小需为 blockSize + numTaps - 1
static float firState_ir[FIR_BLOCK_SIZE + FIR_NUM_TAPS - 1];    // IR（红外）信号使用
static float firState_red[FIR_BLOCK_SIZE + FIR_NUM_TAPS - 1];   // RED（红光）信号使用


//低通滤波器系数，由 Matlab - FDATool 工具设计
const float firCoeffs[FIR_NUM_TAPS] = {
	-0.000394485056637216,  -0.00110441525051116,  -0.00220060246764154,
	-0.00377610666472349,  -0.00557640352879890,  -0.00692640755885371,
	-0.00678705667628360,  -0.00394711624196311,   0.00268393778337649,
	 0.0137496540066006,    0.0291719297024693,    0.0480079262560188,
	 0.0684913635467648,    0.0882677050094305,    0.104784282929553,
	 0.115755468431443,     0.119600651559512,     0.115755468431443,
	 0.104784282929553,     0.0882677050094305,    0.0684913635467648,
	 0.0480079262560188,    0.0291719297024693,    0.0137496540066006,
	 0.00268393778337649,  -0.00394711624196311,  -0.00678705667628360,
	-0.00692640755885371,  -0.00557640352879890,  -0.00377610666472349,
	-0.00220060246764154,  -0.00110441525051116,  -0.000394485056637216
};

/**
  * @brief  初始化FIR滤波器，分别为IR和RED通道配置滤波器实例
  * @param  无
  * @retval 无
  */
void MAX30102_FIR_Init(void)
{
    arm_fir_init_f32(&fir_ir, FIR_NUM_TAPS, (float32_t *)&firCoeffs[0], &firState_ir[0], FIR_BLOCK_SIZE);
    arm_fir_init_f32(&fir_red, FIR_NUM_TAPS, (float32_t *)&firCoeffs[0], &firState_red[0], FIR_BLOCK_SIZE);
}

/**
  * @brief  对IR通道信号进行FIR滤波
  * @param  input：输入数据指针
  * @param  output：输出数据指针
  * @retval 无
  */
void MAX30102_IR_FIR(float *input, float *output)
{
    arm_fir_f32(&fir_ir, input, output, FIR_BLOCK_SIZE);
}

/**
  * @brief  对RED通道信号进行FIR滤波
  * @param  input：输入数据指针
  * @param  output：输出数据指针
  * @retval 无
  */
void MAX30102_RED_FIR(float *input, float *output)
{
    arm_fir_f32(&fir_red, input, output, FIR_BLOCK_SIZE);
}

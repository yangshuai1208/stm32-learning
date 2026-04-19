#include "led/bsp_led.h"


/**
  * @brief  初始化控制LED的GPIO引脚
  * @note   配置为推挽输出，默认全灭
  */
void LED_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* 使能GPIOA端口时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* 设置引脚为推挽输出 */
    GPIO_InitStruct.Pin = R_LED_Pin | G_LED_Pin | B_LED_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;     // 推挽输出
    GPIO_InitStruct.Pull = GPIO_NOPULL;             // 不上拉不下拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;    // 低速输出
    HAL_GPIO_Init(LED_Port, &GPIO_InitStruct);

    /* 初始化时关闭所有LED（低电平点亮，默认全设为高电平） */
    HAL_GPIO_WritePin(LED_Port, R_LED_Pin | G_LED_Pin | B_LED_Pin, GPIO_PIN_SET);
}




#include "main.h"
#include "led/bsp_led.h"
#include "dwt/bsp_dwt.h"
#include "usart/bsp_usart.h"
#include "i2c/bsp_i2c.h"
#include "oled/bsp_i2c_oled.h"
#include "max30102/bsp_max30102.h"
#include "max30102/app_max30102.h"
#include "max30102/max30102_fir.h"



void SystemClock_Config(void);



int main(void)
{


	HAL_Init();  								// 初始化 HAL 库 
	
	SystemClock_Config();  			// 配置系统时钟，设置为 72MHz
	
	MX_USART1_UART_Init();  	 //初始化串口

	DWT_Init();								// 启动 DWT 计数器，用于精确测量程序运行时间
	
	MAX30102_Init();				//心率血氧传感器初始化
	
	MAX30102_FIR_Init();		//FIR滤波计算初始化

	printf("\r\n欢迎使用心率血氧检测模块！\r\n");
	printf("\r\n请将指尖轻贴在心率血氧检测模块上，使其完全覆盖模块检测区");
	printf("\n当绿灯亮则代表正在读取中，稍等片刻串口将会打印数据");
	printf("\n请保持手指与模块间的静止状态，以减少误差\r\n");
	
  while (1)
  {

		
	//主循环中调用MAX30102任务函数
			MAX30102_Task();
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 72000000
  *            HCLK(Hz)                       = 72000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            HSE PREDIV1                    = 1
  *            PLLMUL                         = 9
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef clkinitstruct = {0};
  RCC_OscInitTypeDef oscinitstruct = {0};
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  oscinitstruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
  oscinitstruct.HSEState        = RCC_HSE_ON;
  oscinitstruct.HSEPredivValue  = RCC_HSE_PREDIV_DIV1;
  oscinitstruct.PLL.PLLState    = RCC_PLL_ON;
  oscinitstruct.PLL.PLLSource   = RCC_PLLSOURCE_HSE;
  oscinitstruct.PLL.PLLMUL      = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&oscinitstruct)!= HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
  clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;  
  if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2)!= HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

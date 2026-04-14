/**
  ******************************************************************************
  * @file       main.c
  * @author     embedfire
  * @version    V1.0
  * @date       2025
  * @brief      模板
  ******************************************************************************
  * @attention
  *
  * 实验平台  ：野火 STM32F103C8T6-STM32开发板 
  * 论坛      ：http://www.firebbs.cn
  * 官网      ：https://embedfire.com/
  * 淘宝      ：https://yehuosm.tmall.com/
  *
  ******************************************************************************
	*/

#include "main.h"
#include "led/bsp_led.h"
#include "dwt/bsp_dwt.h"



void SystemClock_Config(void);

uint64_t begin=0;
uint64_t end=0;
uint64_t duration=0;

uint32_t task_us = 0; 
int main(void)
{


	HAL_Init();  // 初始化 HAL 库 
	
	SystemClock_Config();  // 配置系统时钟，设置为 72MHz

	DWT_Init();
	
	LED_GPIO_Config();
	
	
	
	
	
  while (1)
  {
			begin = DWT_GetTick();           // 记录第一段程序开始的时间
			
			RGB_ALL_ON();                    // 打开 RGB 灯，模拟任务运行
			DWT_DelayMs(1000);               // 精确延时 1 秒
			RGB_ALL_OFF();                   // 关闭 RGB 灯，模拟任务结束
			DWT_DelayMs(1000);               // 再延时 1 秒

			end = DWT_GetTick();             // 记录结束时间
			duration = end - begin;          // 计算任务耗时（单位为 DWT 周期）
		
			task_us = DWT_TickToMicrosecond(duration , SystemCoreClock);	
		
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

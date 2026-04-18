/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "gpio.h"
#include "usart.h"
#include "i2c.h"
#include "bsp_i2c_oled.h"
#include <string.h>



/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
volatile uint32_t led_count=0;
volatile uint32_t uart_count=0;


/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId ledTaskHandle;
osThreadId uartTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
static void DebugTaskSend(const char *text);

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartLEDTask(void const * argument);
void StartUartTAsk(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

extern volatile uint8_t g_oled_ready;

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}

static void DebugTaskSend(const char *text)
{
  if (text == NULL)
  {
    return;
  }

  HAL_UART_Transmit(&huart1, (uint8_t *)text, (uint16_t)strlen(text), 100);
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
	defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);
  if(defaultTaskHandle == NULL)
  {
    DebugTaskSend("defaultTask create fail\r\n");
    Error_Handler();
  }

  /* definition and creation of ledTask */
  osThreadDef(ledTask, StartLEDTask, osPriorityNormal, 0, 512);
  ledTaskHandle = osThreadCreate(osThread(ledTask), NULL);
	if(ledTaskHandle==NULL)
	{
    DebugTaskSend("ledTask create fail\r\n");
		Error_Handler();
	}

  /* definition and creation of uartTask */
  osThreadDef(uartTask, StartUartTAsk, osPriorityLow, 0, 256);
  uartTaskHandle = osThreadCreate(osThread(uartTask), NULL);
		if(uartTaskHandle==NULL)
	{
    DebugTaskSend("uartTask create fail\r\n");
		Error_Handler();
	}

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartLEDTask */
/**
* @brief Function implementing the ledTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartLEDTask */
void StartLEDTask(void const * argument)
{
  /* USER CODE BEGIN StartLEDTask */
  /* Infinite loop */
  for(;;)
  {
        if (g_oled_ready == 0U)
        {
          g_oled_ready = (OLED_Init() == SUCCESS) ? 1U : 0U;
          osDelay(200);
          continue;
        }

				OLED_CLS();
        OLED_ShowString_F8X16(0,0, (uint8_t*)"STM32");
        OLED_ShowString_F8X16(1,0, (uint8_t*)"FreeRTOS");
        OLED_ShowString_F8X16(2,0, (uint8_t*)"LED OK");
				osDelay(1000);
  }
  /* USER CODE END StartLEDTask */
}

/* USER CODE BEGIN Header_StartUartTAsk */
/**
* @brief Function implementing the uartTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUartTAsk */
void StartUartTAsk(void const * argument)
{
  /* USER CODE BEGIN StartUartTAsk */
	 const char msg[] = "USART1 send ok\r\n";
  /* Infinite loop */
  for(;;)
  {
		HAL_GPIO_TogglePin(GPIOA,LED_R_Pin);
	  HAL_UART_Transmit(&huart1, (uint8_t *)msg, (uint16_t)(sizeof(msg) - 1), HAL_MAX_DELAY);
    osDelay(1000);
  }
  /* USER CODE END StartUartTAsk */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void vApplicationMallocFailedHook(void)
{
  DebugTaskSend("RTOS malloc failed\r\n");
  __disable_irq();
  while (1)
  {
  }
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
  (void)xTask;
  DebugTaskSend("RTOS stack overflow: ");
  DebugTaskSend(pcTaskName);
  DebugTaskSend("\r\n");
  __disable_irq();
  while (1)
  {
  }
}

/* USER CODE END Application */


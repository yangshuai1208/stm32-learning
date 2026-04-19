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
#include <stdio.h>
#include "bsp_dht11.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
	typedef struct
	{
			uint8_t temp_int;
			uint8_t temp_dec;
			uint8_t humi_int;
			uint8_t humi_dec;
			uint8_t valid;
	} SensorData;
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
volatile SensorData g_sensor_data = {0};

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId ledTaskHandle;
osThreadId uartTaskHandle;
osThreadId sensorTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
static void DebugTaskSend(const char *text);

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartLEDTask(void const * argument);
void StartUartTask(void const * argument);
void StartSensorTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

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
		if (defaultTaskHandle == NULL)
{
    Error_Handler();
}
  /* definition and creation of ledTask */
  osThreadDef(ledTask, StartLEDTask, osPriorityNormal, 0, 256);
  ledTaskHandle = osThreadCreate(osThread(ledTask), NULL);
		if (ledTaskHandle == NULL)
	{
			Error_Handler();
	}
  /* definition and creation of uartTask */
  osThreadDef(uartTask, StartUartTask, osPriorityLow, 0, 128);
  uartTaskHandle = osThreadCreate(osThread(uartTask), NULL);
	if (uartTaskHandle == NULL)
{
    Error_Handler();
}
  /* definition and creation of sensorTask */
  osThreadDef(sensorTask, StartSensorTask, osPriorityAboveNormal, 0, 128);
  sensorTaskHandle = osThreadCreate(osThread(sensorTask), NULL);
		if (sensorTaskHandle == NULL)
	{
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
	   char temp_buf[20];
    char humi_buf[20];
  /* USER CODE BEGIN StartLEDTask */
  /* Infinite loop */
  for(;;)
  {
          OLED_CLS();

        if (g_sensor_data.valid)
        {
            sprintf(temp_buf, "TEMP:%d.%dC",
                    g_sensor_data.temp_int,
                    g_sensor_data.temp_dec);

            sprintf(humi_buf, "HUMI:%d.%d%%",
                    g_sensor_data.humi_int,
                    g_sensor_data.humi_dec);

            OLED_ShowString_F8X16(0, 0, (uint8_t *)"DHT11");
            OLED_ShowString_F8X16(2, 0, (uint8_t *)temp_buf);
            OLED_ShowString_F8X16(4, 0, (uint8_t *)humi_buf);
        }
        else
        {
            OLED_ShowString_F8X16(0, 0, (uint8_t *)"DHT11");
            OLED_ShowString_F8X16(2, 0, (uint8_t *)"READ ERROR");
        }

        osDelay(1000);
  }
  /* USER CODE END StartLEDTask */
}

/* USER CODE BEGIN Header_StartUartTask */
/**
* @brief Function implementing the uartTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUartTask */
void StartUartTask(void const * argument)
{
  /* USER CODE BEGIN StartUartTask */
  /* Infinite loop */
     char uart_buf[50];

    for (;;)
    {
        HAL_GPIO_TogglePin(GPIOA, LED_R_Pin);

        if (g_sensor_data.valid)
        {
            sprintf(uart_buf,
                    "TEMP:%d.%dC  HUMI:%d.%d%%\r\n",
                    g_sensor_data.temp_int,
                    g_sensor_data.temp_dec,
                    g_sensor_data.humi_int,
                    g_sensor_data.humi_dec);
        }
        else
        {
            sprintf(uart_buf, "DHT11 READ ERROR\r\n");
        }

        HAL_UART_Transmit(&huart1, (uint8_t *)uart_buf, strlen(uart_buf), HAL_MAX_DELAY);
        osDelay(2000);
  }
  /* USER CODE END StartUartTask */
}

/* USER CODE BEGIN Header_StartSensorTask */
/**
* @brief Function implementing the sensorTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartSensorTask */
void StartSensorTask(void const * argument)
{
	    DHT11_DATA_TYPEDEF dht11_data;
  /* USER CODE BEGIN StartSensorTask */
	
  /* Infinite loop */
  for(;;)
  {
          if (DHT11_ReadData(&dht11_data) == HAL_OK)
        {
            g_sensor_data.temp_int = dht11_data.temp_int;
            g_sensor_data.temp_dec = dht11_data.temp_deci;
            g_sensor_data.humi_int = dht11_data.humi_int;
            g_sensor_data.humi_dec = dht11_data.humi_deci;
            g_sensor_data.valid = 1;
        }
        else
        {
            g_sensor_data.valid = 0;
        }

        osDelay(2000);
  }
  /* USER CODE END StartSensorTask */
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

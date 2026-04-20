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
extern volatile uint8_t g_oled_ready;
static StaticTask_t probeTaskControlBlock;
static StackType_t probeTaskStack[128];
static StaticTask_t ledTaskControlBlock;
static StackType_t ledTaskStack[256];
static StaticTask_t uartTaskControlBlock;
static StackType_t uartTaskStack[256];
static StaticTask_t sensorTaskControlBlock;
static StackType_t sensorTaskStack[256];
/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId probeTaskHandle;
osThreadId ledTaskHandle;
osThreadId uartTaskHandle;
osThreadId sensorTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
static void DebugTaskSend(const char *text);

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartProbeTask(void const * argument);
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
 // osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  //defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);
	//	if (defaultTaskHandle == NULL)
//{
 //   Error_Handler();
//}
  /* definition and creation of probeTask */
  osThreadStaticDef(probeTask, StartProbeTask, osPriorityHigh, 0, 128, probeTaskStack, &probeTaskControlBlock);
  probeTaskHandle = osThreadCreate(osThread(probeTask), NULL);
  if (probeTaskHandle == NULL)
  {
    Error_Handler();
  }
  /* definition and creation of ledTask */
  osThreadStaticDef(ledTask, StartLEDTask, osPriorityNormal, 0, 256, ledTaskStack, &ledTaskControlBlock);
  ledTaskHandle = osThreadCreate(osThread(ledTask), NULL);
		if (ledTaskHandle == NULL)
	{
			Error_Handler();
	}
  /* definition and creation of uartTask */
  osThreadStaticDef(uartTask, StartUartTask, osPriorityNormal, 0, 256, uartTaskStack, &uartTaskControlBlock);
  uartTaskHandle = osThreadCreate(osThread(uartTask), NULL);
	if (uartTaskHandle == NULL)
{
    Error_Handler();
}
  /* definition and creation of sensorTask */
  osThreadStaticDef(sensorTask, StartSensorTask, osPriorityBelowNormal, 0, 256, sensorTaskStack, &sensorTaskControlBlock);
  sensorTaskHandle = osThreadCreate(osThread(sensorTask), NULL);
		if (sensorTaskHandle == NULL)
	{
			Error_Handler();
	}
  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

void StartProbeTask(void const * argument)
{
  (void)argument;
  DebugTaskSend("PROBE TASK START\r\n");

  for (;;)
  {
    HAL_GPIO_TogglePin(GPIOA, LED_B_Pin);
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    osDelay(500);
  }
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
    SensorData sensor_snapshot;

  /* USER CODE BEGIN StartLEDTask */
  DebugTaskSend("DISPLAY TASK START\r\n");
  /* Infinite loop */
  for(;;)
    {
        taskENTER_CRITICAL();
        sensor_snapshot.temp_int = g_sensor_data.temp_int;
        sensor_snapshot.temp_dec = g_sensor_data.temp_dec;
        sensor_snapshot.humi_int = g_sensor_data.humi_int;
        sensor_snapshot.humi_dec = g_sensor_data.humi_dec;
        sensor_snapshot.valid = g_sensor_data.valid;
        taskEXIT_CRITICAL();

        HAL_GPIO_TogglePin(GPIOA, LED_R_Pin);
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

        if (g_oled_ready)
        {
            OLED_CLS();
            if (sensor_snapshot.valid)
            {
                sprintf(temp_buf, "TEMP:%d.%dC",
                        sensor_snapshot.temp_int,
                        sensor_snapshot.temp_dec);
                sprintf(humi_buf, "HUMI:%d.%d%%",
                        sensor_snapshot.humi_int,
                        sensor_snapshot.humi_dec);
                OLED_ShowString_F8X16(0, 0, (uint8_t *)"DHT11");
                OLED_ShowString_F8X16(1, 0, (uint8_t *)temp_buf);
                OLED_ShowString_F8X16(2, 0, (uint8_t *)humi_buf);
            }
            else
            {
                OLED_ShowString_F8X16(0, 0, (uint8_t *)"DHT11");
                OLED_ShowString_F8X16(1, 0, (uint8_t *)"READ ERROR");
            }
        }
        else
        {
            HAL_GPIO_TogglePin(GPIOA, LED_G_Pin);
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
    char uart_buf[64];
    SensorData sensor_snapshot;

  /* USER CODE BEGIN StartUartTask */
  DebugTaskSend("UART TASK START\r\n");
  /* Infinite loop */
   
    for(;;)
    {
        taskENTER_CRITICAL();
        sensor_snapshot.temp_int = g_sensor_data.temp_int;
        sensor_snapshot.temp_dec = g_sensor_data.temp_dec;
        sensor_snapshot.humi_int = g_sensor_data.humi_int;
        sensor_snapshot.humi_dec = g_sensor_data.humi_dec;
        sensor_snapshot.valid = g_sensor_data.valid;
        taskEXIT_CRITICAL();

        if (sensor_snapshot.valid)
        {
            sprintf(uart_buf, "DHT11 TEMP:%d.%dC HUMI:%d.%d%%\r\n",
                    sensor_snapshot.temp_int,
                    sensor_snapshot.temp_dec,
                    sensor_snapshot.humi_int,
                    sensor_snapshot.humi_dec);
        }
        else
        {
            sprintf(uart_buf, "DHT11 READ ERROR\r\n");
        }

        DebugTaskSend(uart_buf);
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
  osDelay(500);
  DebugTaskSend("SENSOR TASK START\r\n");
	
  /* Infinite loop */
  for(;;)
  {
        if (DHT11_ReadData(&dht11_data) == HAL_OK)
        {
            taskENTER_CRITICAL();
            g_sensor_data.temp_int = dht11_data.temp_int;
            g_sensor_data.temp_dec = dht11_data.temp_deci;
            g_sensor_data.humi_int = dht11_data.humi_int;
            g_sensor_data.humi_dec = dht11_data.humi_deci;
            g_sensor_data.valid = 1;
            taskEXIT_CRITICAL();
        }
        else
        {
            taskENTER_CRITICAL();
            g_sensor_data.valid = 0;
            taskEXIT_CRITICAL();
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
  vAssertCalled("malloc", 0);
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
  (void)xTask;
  DebugTaskSend("RTOS stack overflow: ");
  DebugTaskSend(pcTaskName);
  DebugTaskSend("\r\n");
  vAssertCalled("stack", 0);
}

void vAssertCalled(const char *file, int line)
{
  char assert_buf[96];

  taskDISABLE_INTERRUPTS();
  sprintf(assert_buf, "ASSERT %s:%d\r\n", file, line);
  DebugTaskSend(assert_buf);

  while (1)
  {
    HAL_GPIO_TogglePin(GPIOA, LED_R_Pin | LED_G_Pin | LED_B_Pin);
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    for (volatile uint32_t delay = 0; delay < 250000U; delay++)
    {
      __NOP();
    }
  }
}
/* USER CODE END Application */

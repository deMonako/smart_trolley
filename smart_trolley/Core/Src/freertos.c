/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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

#include "functions.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

int what_to_display = 0;
int button = 0;
int zezwolenie = 0;

int cyfra_1 = 0;
int cyfra_2 = 0;

int sum = 0;

int ostateczna = 0;

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId czujnikHandle;
osThreadId wyswietlaczHandle;
osThreadId pilotHandle;
osSemaphoreId PermHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void czujnik_init(void const * argument);
void wyswietlacz_init(void const * argument);
void pilot_init(void const * argument);

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

  /* Create the semaphores(s) */
  /* definition and creation of Perm */
  osSemaphoreDef(Perm);
  PermHandle = osSemaphoreCreate(osSemaphore(Perm), 1);

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
  osThreadDef(defaultTask, StartDefaultTask, osPriorityBelowNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of czujnik */
  osThreadDef(czujnik, czujnik_init, osPriorityNormal, 0, 128);
  czujnikHandle = osThreadCreate(osThread(czujnik), NULL);

  /* definition and creation of wyswietlacz */
  osThreadDef(wyswietlacz, wyswietlacz_init, osPriorityAboveNormal, 0, 128);
  wyswietlaczHandle = osThreadCreate(osThread(wyswietlacz), NULL);

  /* definition and creation of pilot */
  osThreadDef(pilot, pilot_init, osPriorityHigh, 0, 128);
  pilotHandle = osThreadCreate(osThread(pilot), NULL);

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
	  //powrót do wyświetlania rzeczywistej odległości (po wpisaniu wartości zadanej)
	  if(what_to_display == 2)
	  {
		  HAL_Delay(3000);
		  what_to_display = 0;
		  ostateczna = 1;
	  }

	  if(ostateczna == 1)
	  {
		HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
		 osSemaphoreWait(PermHandle, osWaitForever);
		 while(1)
		 	{
			 sensor_time = hcsr04_read();
			 distance = sensor_time/23;

				 if(distance > sum)
				 {
					 servo_forward();
				 }
				 else if(distance < sum)
				 {
					 servo_backward();
				 }
				 else
				 {
					 servo_stop();
					 HAL_Delay(500);
				 }

			 HAL_Delay(150);

			 servo_stop();

			 HAL_Delay(10);
			 }
		 	}
		 osSemaphoreRelease(PermHandle);

    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_czujnik_init */
/**
* @brief Function implementing the czujnik thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_czujnik_init */
void czujnik_init(void const * argument)
{
  /* USER CODE BEGIN czujnik_init */
  /* Infinite loop */
  for(;;)
  {
	  osSemaphoreWait(PermHandle, osWaitForever);

	  //cykliczny odczyt odległości
	  sensor_time = hcsr04_read();
	  if(sensor_time/23 >= 2 && sensor_time/23 <= 30)
	  {
		  distance  = sensor_time / 23;
	  }
	  else if(sensor_time/23 > 30)
	  {
		  distance = 30;
	  }
	  else if (sensor_time/23 < 2)
	  {
		  distance = 1;
	  }
	  else;

	  osSemaphoreRelease(PermHandle);

	  HAL_Delay(200);

    osDelay(1);
  }
  /* USER CODE END czujnik_init */
}

/* USER CODE BEGIN Header_wyswietlacz_init */
/**
* @brief Function implementing the wyswietlacz thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_wyswietlacz_init */
void wyswietlacz_init(void const * argument)
{
  /* USER CODE BEGIN wyswietlacz_init */
  /* Infinite loop */
  for(;;)
  {
	osSemaphoreWait(PermHandle, osWaitForever);
	if(what_to_display == 0) //wartość rzeczywista
	{
		display(distance,50);
	}
	else if(what_to_display == 2) //wartość zadana wyświetlana przez 3s, resetowana w DefautTask
	{
		sum = cyfra_1*10 + cyfra_2;
		display(sum,50);
	}
	osSemaphoreRelease(PermHandle);

    osDelay(1);
  }
  /* USER CODE END wyswietlacz_init */
}

/* USER CODE BEGIN Header_pilot_init */
/**
* @brief Function implementing the pilot thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_pilot_init */
void pilot_init(void const * argument)
{
  /* USER CODE BEGIN pilot_init */
  /* Infinite loop */
  for(;;)
  {
	  if(ostateczna == 0)
	  {
		  if(zezwolenie == 1)
	  {
		  osSemaphoreWait(PermHandle, osWaitForever);
		  //odczyt z pilota
		  HAL_Delay(600);
		  while(HAL_GPIO_ReadPin (IR_DATA_GPIO_Port, IR_DATA_Pin));

	  		  data = receive_data ();

	  		if(convert_code (data)>=0 && convert_code (data) <=9)
	  		{
	  			button = convert_code (data);
	  		}
	  		else if (convert_code (data) == 100)
	  		{
	  			button = 100;
	  		}
	  		HAL_Delay(200);

	  		if(what_to_display == 0)
	  		{
	  			what_to_display = 1;
	  			cyfra_1 = button;
	  			display(cyfra_1,400);
	  			HAL_Delay(1000);
	  			screenOFF();
	  		}
	  		else if(what_to_display == 1)
	  		{
	  			what_to_display = 2;
	  			cyfra_2 = button;
	  			display(cyfra_2,400);
	  			HAL_Delay(1000);
	  			screenOFF();
	  			zezwolenie = 0;
	  		}

	  		osSemaphoreRelease(PermHandle);
	  }
	  else;

	  //zezwolenie na wpisanie wartości
	  if(HAL_GPIO_ReadPin (IR_DATA_GPIO_Port, IR_DATA_Pin) == GPIO_PIN_RESET)
	  {
		  zezwolenie = 1;
		  HAL_Delay(100);
	  }
	  else;
	  }
	  else;
    osDelay(1);
  }
  /* USER CODE END pilot_init */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

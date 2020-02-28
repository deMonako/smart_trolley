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

int what_to_display = 0; /*!< zmienna określająca tryb pracy wyswietlacza */
int button = 0; /*!< zmienna przechowująca wartość odczytaną z pilota */
int zezwolenie = 0; /*!< zezwolenie na wpisanie wartości zadanej (uruchomienie trybu odczytu) */

int cyfra_1 = 0; /*!< zmienna przechowująca cyfrę dziesiętnych sumy */
int cyfra_2 = 0; /*!< zmienna przechowująca cyfrę jedności sumy */
int sum = 0; /*!< suma (finałowa wartość zadana) */

int ostateczna = 0; /*!< zezwolenie na wykonanie algorytmu sterującego (zapętlenie programu w DefaultTask) */
int a = 0; /*!< ochrona przed przedwczesnym zezwoleniu na wpisanie wartosci zadanej (patrz 'zezwolenie') */

long ikk = 0; /*!< licznik odświeżania wyświetlacza */
int wartosc = 0; /*!< odświeżana wyświetlana wartość */

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
	servo_stop();
  /* Infinite loop */
  for(;;)
  {
	  /* powrót do wyświetlania rzeczywistej odległości (po wpisaniu wartości zadanej) */
	  if(what_to_display == 2)
	  {
		  HAL_Delay(2000);
		  what_to_display = 0;
		  ostateczna = 1;
	  }

	  /* warunek końcowy głównego algorytmu regulacji */
	  if(ostateczna == 1)
	  {
		 HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin,GPIO_PIN_SET);
		 osSemaphoreWait(PermHandle, osWaitForever);

		  /* głowna pętla algorytmu regulacji */
		 	do{

		 	/* bieżący odczyt odległości */
			 sensor_time = hcsr04_read();
			 distance = sensor_time/23;

			 /* regulator dwupołożeniowy sterujący serwomechanizmem */
				 if(distance > sum)
				 {
					 servo_forward();
				 }
				 else if(distance < sum)
				 {
					 servo_backward();
				 }
				 else if(distance < 2)
				 {
					 	//do_nothing
				 }
				 else
				 {
					 servo_stop();
					 HAL_Delay(20);
				 }

			/* eliminacja błędu z wyświetlaniem wartości 0 (czasem tak robiło nie wiem czemu) */
			if(wartosc != 0)
			 display(wartosc,10);

			 /* przypisywanie wartości zczytanej wartości wyświetlanej */
			 if(ikk == 0)
				 wartosc = distance;

			 ikk++;

			 /* aktualizacja wartości wyświetlanej (co 30 pętli) */
			 if(ikk == 30)
				 ikk = 0;

			 /* wyjście z pętli algorytmu regulacji (po wciśnięciu przycisku) */
			  if(HAL_GPIO_ReadPin (IR_DATA_GPIO_Port, IR_DATA_Pin) == GPIO_PIN_RESET)
			  {
				  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin,GPIO_PIN_SET);

				  	  /*ustawianie flag na domyślną wartość*/
				  	  what_to_display = 0;
				  	  button = 0;
				  	  zezwolenie = 0;
				  	  cyfra_1 = 0;
				  	  cyfra_2 = 0;
				  	  sum = 0;
				  	  ostateczna = 0;

				 HAL_Delay(100);

				 	 a=0;

				 HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin,GPIO_PIN_RESET);
			  }
			 }while(ostateczna != 0);
		 }
	  	HAL_Delay(500);
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin,GPIO_PIN_RESET);
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

	  /* cykliczny odczyt odległości */
	  sensor_time = hcsr04_read();
	  if(sensor_time/23 >= 2 && sensor_time/23 <= 50)
	  {
		  distance  = sensor_time / 23;
	  }
	  else if(sensor_time/23 > 50)
	  {
		  distance = 50;
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

	/*!
	* \brief wyświetlanie wartości na wyświetlaczu
	*
	* what_to_display == 0 -> wartość rzeczywista
	* what_to_display == 2 -> wartość zadana wyświetlana przez 2s, resetowana w DefautTask
	*/
	if(what_to_display == 0)
	{
		display(distance,50);
	}
	else if(what_to_display == 2)
	{
		sum = cyfra_1*10 + cyfra_2; /*!< obliczanie wartości zadanej */
		if(sum == 0)
		{
			sum = 2;
		}
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
	  /* odczyt po wyjściu z pętli algorytmu sterowania lub początku programu (tylko jednorazowo) */
	  if(ostateczna == 0)
	  {
		  /* oczekiwanie na zezwolenie wpisania wartości (patrz niżej) */
		  if(zezwolenie == 1)
		  {
		  osSemaphoreWait(PermHandle, osWaitForever);
		  HAL_Delay(600);

		  /* odczyt z pilota */
		  while(HAL_GPIO_ReadPin (IR_DATA_GPIO_Port, IR_DATA_Pin));

	  		  data = receive_data ();

	  		/* konwersja danych na cyfry */
	  		if(convert_code (data)>=0 && convert_code (data) <=9)
	  		{
	  			button = convert_code (data);
	  		}
	  		else if (convert_code (data) == 100)
	  		{
	  			button = 100;
	  		}
	  		HAL_Delay(200);

	  		/* zapisywanie kolejnych cyfr do pamięci */
	  		if(what_to_display == 0)
	  		{
	  			what_to_display = 1;
	  			cyfra_1 = button;
	  			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin,GPIO_PIN_SET);
	  			display(cyfra_1,400);
	  			HAL_Delay(500);
	  			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin,GPIO_PIN_RESET);
	  			screenOFF();
	  		}
	  		else if(what_to_display == 1)
	  		{
	  			what_to_display = 2;
	  			cyfra_2 = button;
	  			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin,GPIO_PIN_SET);
	  			display(cyfra_2,400);
	  			HAL_Delay(500);
	  			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin,GPIO_PIN_RESET);
	  			screenOFF();
	  			zezwolenie = 0;
	  			HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin,GPIO_PIN_RESET); //sygnalizacja zezwolenia wpisania wartości
	  		}

	  		osSemaphoreRelease(PermHandle);
	  }
	  else;

	  /* zezwolenie na wpisanie wartości */
	  if(HAL_GPIO_ReadPin (IR_DATA_GPIO_Port, IR_DATA_Pin) == GPIO_PIN_RESET && a == 0 && HAL_GPIO_ReadPin (LD2_GPIO_Port, LD2_Pin) == GPIO_PIN_RESET)
	  {
		  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin,GPIO_PIN_SET);
		  zezwolenie = 1;
		  a=1;
		  screenOFF();
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

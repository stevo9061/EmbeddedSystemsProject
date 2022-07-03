/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
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
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "h4lib.h"
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include "stm32l4xx_hal.h"
#include "WiFi_Click_lib.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BUFFERSIZE 11
#define RATE_SIZE 4							//Increase this for more averaging. 4 is good.
#define USE_SEM
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim7;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* Definitions for ChipTemp */
osThreadId_t ChipTempHandle;
uint32_t ChipTempBuffer[ 512 ];
osStaticThreadDef_t ChipTempControlBlock;
const osThreadAttr_t ChipTemp_attributes = {
  .name = "ChipTemp",
  .cb_mem = &ChipTempControlBlock,
  .cb_size = sizeof(ChipTempControlBlock),
  .stack_mem = &ChipTempBuffer[0],
  .stack_size = sizeof(ChipTempBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for HeartBeats */
osThreadId_t HeartBeatsHandle;
uint32_t HeartBeatsBuffer[ 512 ];
osStaticThreadDef_t HeartBeatsControlBlock;
const osThreadAttr_t HeartBeats_attributes = {
  .name = "HeartBeats",
  .cb_mem = &HeartBeatsControlBlock,
  .cb_size = sizeof(HeartBeatsControlBlock),
  .stack_mem = &HeartBeatsBuffer[0],
  .stack_size = sizeof(HeartBeatsBuffer),
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for sem_PRODUCE_Sync */
osSemaphoreId_t sem_PRODUCE_SyncHandle;
const osSemaphoreAttr_t sem_PRODUCE_Sync_attributes = {
  .name = "sem_PRODUCE_Sync"
};
/* USER CODE BEGIN PV */
unsigned int tim_elapsed = 0; // Basically our global clock/counter; counts upwards in 50ms steps

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM7_Init(void);
void StartChipTemp(void *argument);
void StartHeartBeats(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t ringBuffer[BUFFERSIZE]; // Receive input from the HAL_Uart_Receive() Function
bool read_f = false;
bool stop_f = false;
bool start_f = false;
bool no_finger_f = true;
uint32_t red_sample;

uint32_t rates[RATE_SIZE]; 				 // Array of heart rates
uint32_t rateSpot = 0;
long lastBeat = 0; 					    // Time at which the last beat occurred
uint16_t delta = 0;
float beatsPerMinute;
uint32_t beatAvg;
uint16_t timer_val;

// Print given character on UART 2. Translate '\n' to "\r\n" on the fly.
int __io_putchar(int ch) {
	int ret;
	while ((ret = HAL_UART_GetState(&huart2)) != HAL_UART_STATE_READY)
		;
	if (ch == '\n') {
		static uint8_t buf[2] = {'\r', '\n'};
		HAL_UART_Transmit_IT(&huart2, buf, sizeof(buf));
	} else {
		static char buf;
		buf = ch;
		HAL_UART_Transmit_IT(&huart2, (uint8_t *)&buf, 1);
	}
	return ch;
}

// So we do not use the printf on the stdio but bend it to UART */
int _write(int file, char *ptr, int len)
{
	for (int DataIdx = 0; DataIdx < len; DataIdx++) {
		__io_putchar(*ptr++);
	}
	return len;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	hr4_set_registers(hi2c1);
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */

  // Setup register heart rate module
  hr4_set_registers(hi2c1);

  // Connect to the wifi
  wifi_click_init();

  //	printf("nach wifi_init \n"); TODO: DELETE


  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of sem_PRODUCE_Sync */
  sem_PRODUCE_SyncHandle = osSemaphoreNew(1, 1, &sem_PRODUCE_Sync_attributes);

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
  /* creation of ChipTemp */
  ChipTempHandle = osThreadNew(StartChipTemp, NULL, &ChipTemp_attributes);

  /* creation of HeartBeats */
  HeartBeatsHandle = osThreadNew(StartHeartBeats, NULL, &HeartBeats_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
	/* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00707CBB;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 32000 - 1;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 10 - 1;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LD3_Pin */
  GPIO_InitStruct.Pin = LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD3_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartChipTemp */
/**
 * @brief  Function implementing the ChipTemp thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartChipTemp */
void StartChipTemp(void *argument)
{
  /* USER CODE BEGIN 5 */
	/* USER CODE BEGIN WHILE */


	/* Infinite loop */
	for(;;)
	{
#ifdef USE_SEM
	  if(osSemaphoreAcquire(sem_PRODUCE_SyncHandle, 100) ==osOK)
	  {
#endif

		printf("\nTask 1 is processing..\n");
	//	osDelay(1000);


		// I chose timeout duration '5000' to have enough time to type a command for update interval.
		if (HAL_UART_Receive(&huart2, ringBuffer, BUFFERSIZE, 5000) == HAL_ERROR)
		{
			Error_Handler();
		}else {

			// Function to send the chip temperature
			// Expected command is '#t,tempa\n'
			checkInput(hi2c1, huart2, ringBuffer, 10);


			// Clean the ringbuffer[BUFFERSIZE]
			for(int i = 0; i < BUFFERSIZE; i++) {
				ringBuffer[i] = 0;
			}
		}

#ifdef USE_SEM
		osSemaphoreRelease(sem_PRODUCE_SyncHandle);
	  }
#endif

		osDelay(1000);


	}
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartHeartBeats */
/**
* @brief Function implementing the HeartBeats thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartHeartBeats */
void StartHeartBeats(void *argument)
{
  /* USER CODE BEGIN StartHeartBeats */
  /* Infinite loop */
  for(;;)
  {


#ifdef USE_SEM
	  if(osSemaphoreAcquire(sem_PRODUCE_SyncHandle, 100) ==osOK)
	  {
#endif
#
		printf("Task 2 is processing..\n");
	//	osDelay(2000);

	  //Fifo is ready -> If an interrupt is set, it would be better to check the interrupt pin
	  		if(hr4_is_new_fifo_data_ready(hi2c1) != 0) // returns 1 is ready
	  		{
	  			read_f = false;


	  			// red_sample = Read RED sensor data = check if finger is on sensor
	  			// Sensor actually only measures whether a finger has just been recognized or a pulse detected. If this is the case, a value > 200k is displayed.
	  			// What follows now is that we make a time measurement in which time intervals a pulse has occurred. For our purposes, the pulse is measured 4x times.
	  			red_sample = hr4_read_red(hi2c1);



	  			// If sample pulse amplitude is under threshold value ( proximity mode )
	  			if ( red_sample > 0 && red_sample < 32000 )
	  			{
	  				stop_f = true;

	  				if ( no_finger_f )
	  				{

	  					// We do nothing more in this program due to time constraints here.
	  					char *noPulse ="\r\n -> No pulse was detected <-\r\n";

	  					if (HAL_UART_Transmit(&huart2, (uint8_t *) noPulse, strlen(noPulse), 1000) == HAL_ERROR)
	  					{
	  						Error_Handler();
	  					}

	  				}

	  				no_finger_f = true;

	  			} else if( red_sample != 0) // If finger is detected ( we are in active heart rate mode )
	  			{
	  				stop_f = false;


	  				bool ratesNotFinishedYet = true;

	  				while (ratesNotFinishedYet) {


	  		// Start timer
	          if(HAL_TIM_Base_GetState(&htim7) == HAL_TIM_STATE_READY) //TODO: Delete Timer, because Timer with interrupt is not working with Rtos
	          {

	          	HAL_TIM_Base_Start_IT(&htim7);

	          }




	  					// Get current time
	  					timer_val = tim_elapsed;
	  					//timer_val = __HAL_TIM_GET_COUNTER(&htim7);
//	  					timer_val = HAL_GetTick();
	  					delta = timer_val - lastBeat;					//Measure duration between two beats
	  					uint8_t str_tmps[50] ="";


	  				   osDelay(500);


	  					lastBeat = timer_val;

	  					beatsPerMinute = 60 / (delta / 1000.0);					//Calculating the BPM

	  					if (beatsPerMinute < 255 && beatsPerMinute > 20)
	  					{
	  						rates[rateSpot++] = (uint8_t)beatsPerMinute;
	  						rateSpot %= RATE_SIZE; //Wrap variable
	  						if(rates[3] != 0)
	  						{
	  							ratesNotFinishedYet = false;

	  						}
	  					}

//TODO: Get every time the same Pulse value?

	  				}

	  				//Take average of readings
	  				beatAvg = 0;
	  				for (uint8_t x = 0; x < RATE_SIZE ; x++)
	  				{
	  					beatAvg += rates[x];
	  				}

	  				beatAvg /= RATE_SIZE;



	  				char *pulseValue = "\r\nPulse value: ";
	  				uint8_t str_tmp[7] ="";

	  				// Print pulse value
	  				if (HAL_UART_Transmit(&huart2, (uint8_t *) pulseValue, strlen(pulseValue), 1000) == HAL_ERROR)
	  				{
	  					Error_Handler();
	  				}

	  				// Output the value when the finger was detected
	  				if (HAL_UART_Transmit(&huart2, str_tmp, sprintf((char *) str_tmp, "%d\n\r", (int) beatAvg), 1000) == HAL_ERROR)
	  				{
	  					Error_Handler();
	  				}


	  				no_finger_f = false;

	  			}
	  		}


#ifdef USE_SEM
		osSemaphoreRelease(sem_PRODUCE_SyncHandle);
	  }
#endif
	     osDelay(500);



  }

  /* USER CODE END StartHeartBeats */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

	// Here the magic happens with our custom clock (which just is a counter)
	// Because we have 32MHZ and use a pre-scaler of 32000 that mans our clock speed is 1ms
	// However because I set the period to 9 the interrupt fires every 10ms and the counter therefore increased in 10ms steps
	if (htim->Instance == TIM7) {
		tim_elapsed += htim->Instance -> ARR + 1; // 9 + 1 = 10 ms to add to the counter
	}

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
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

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */


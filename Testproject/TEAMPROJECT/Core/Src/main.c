/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 * @author		    : Stefan Bittgen, ic20b058
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 * Baudrate/Speed: 115200
 *
 **##Function Finger Print:
 *  When I put my finger on the sensor, the value increases and when I remove my finger from
 *  the sensor, the finger is not read again. The proper functionality is shown in my video.
 *
 ** One command is possible.
 *
 *
 **##Command -> #t,tempa\n
 * It generates some initialization messages from an existing temperature sensor and returns the correct temperature.
 *
 * Example:
 * PC -> STM32
 * command:#t,tempa\n -> # command: t (get the temperatur value from my sensor)
 *						  # data: a realistic temperatur value
 *
 *	STM32 -> PC
 *	STM32: ACK\n		  # positive acknowledge of receipt
 *	**** Temperature values measurement ****
 *	===> Initialize Temperature Sensor MAX30101
 *	=====> Temperature Sensor MAX30101 initialized
 *
 *	data:#a,24\n	      # send an answer: a,24\n
 *	PC: ACK\n
 *
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "h4lib.h"
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BUFFERSIZE 10
#define RATE_SIZE 4							//Increase this for more averaging. 4 is good.

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim7;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
unsigned int tim_elapsed = 0; // Basically our global clock/counter; counts upwards in 50ms steps

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM7_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t ringBuffer[BUFFERSIZE]; // I receive my input from the ISR purely
char *welcome = "\n\radmin@work:~$: ";
bool read_f = false;
bool stop_f = false;
bool start_f = false;
bool no_finger_f = true;
//static uint32_t miliseconds_counter = 0; TODO: Would be used for millisecond counting
uint32_t red_sample;


//uint8_t RATE_SIZE = 4; 			 //Increase this for more averaging. 4 is good.
uint32_t rates[RATE_SIZE]; 				 //Array of heart rates
uint32_t rateSpot = 0;
long lastBeat = 0; 					    //Time at which the last beat occurred
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
	uint16_t timer_val;

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
	MX_I2C1_Init();
	MX_USART2_UART_Init();
	MX_TIM7_Init();
	/* USER CODE BEGIN 2 */

	hr4_set_registers(hi2c1);


	/*	// Start timer
	HAL_TIM_Base_Start(&htim7);

	// Get current time
	timer_val = __HAL_TIM_GET_COUNTER(&htim7);*/

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	/*	if (HAL_UART_Transmit(&huart2, (uint8_t *)welcome, strlen(welcome), 1000) == HAL_ERROR) {

		Error_Handler();
	}*/




	//startAgain:

	while (1) {

		// If enough time has passed (1 second), toggle LED and get new timestamp
		// Vllt einfach 30 Sekunden lang zählen wie oft ein heartbeat gemessen wird, wir würden eh nur 4x brauchen
		/*
		if (__HAL_TIM_GET_COUNTER(&htim7) - timer_val >= 10000)
		{
			printf("JUHU 1 Second elapsed");
			// Get time elapsed
			timer_val =  __HAL_TIM_GET_COUNTER(&htim7);

		}
		 */



		//hr4_is_new_fifo_data_ready(hi2c1); // 1 returns



		//TODO:	if( read_f) -> If an interrupt is set, it would be better to check the interrupt pin

		//Fifo is ready
		if(hr4_is_new_fifo_data_ready(hi2c1) != 0)
		{
			read_f = false;

			// First start
			//			if ( !start_f )
			//			{
			//				start_f = true;
			//Start timer
			//			HAL_TIM_Base_Start(&htim7);
			//TODO: Here actually the timer would be started as well as the interrupt, unfortunately not implementable for time reasons any more.
			//		            InitTimer2();                    // Initializing Timer 2
			//		            EnableInterrupts();              // Enables the processor interrupt
			//		            LOG("START\r\n");                // Sending START command to uPlot
			//			}


			// Read RED sensor data
			// Sensor misst eigentlich nur ob gerade ein Puls aufgetreten ist
			// Wenn nicht ist Puls Wert im unteren Bereich
			// Wenn Puls erkannt wird ist der Wert immer > 200.000
			// TODO: Man könnte sich noch anschauen wie viel Zeit ist zwischen den Pulsmessungen immmer vergangen, Offset erzeugen,
			// einmal ist Puls oben, einmal unten wie bei EKG
			// Puls ergibt aus Hoch und Niedrig und dem Zeitverhalten und dafür benötigen wir eine Feststellung wann ein Puls festgestellt
			// worden ist und wann nicht.
			red_sample = hr4_read_red(hi2c1);



			// If sample pulse amplitude is under threshold value ( proximity mode )
			if ( red_sample > 0 && red_sample < 32000 )
			{
				stop_f = true;

				if ( no_finger_f )
				{
					// TODO: What can we do if no finger is detected?
					// We do nothing more in this program due to time constraints here.
					char *noFinger ="\r\nNo finger is detected ~.~\r\n";

					if (HAL_UART_Transmit(&huart2, (uint8_t *) noFinger, strlen(noFinger), 1000) == HAL_ERROR)
					{
						Error_Handler();
					}




				}

				no_finger_f = true;
			}

			// If finger is detected ( we are in active heart rate mode )
			else if( red_sample != 0)
			{
				stop_f = false;

				if ( no_finger_f )
				{
					// TODO: We do nothing more in this program due to time constraints here.
				}

				char *fingerPrint = "\r\nblabla";
				if (HAL_UART_Transmit(&huart2, (uint8_t *) fingerPrint, strlen(fingerPrint), 1000) == HAL_ERROR)
					{
						Error_Handler();
					}

				char *pulseValue = "\r\nPulse value: ";
				uint8_t str_tmp[15] ="";

				// Output the value when the finger was detected
				if (HAL_UART_Transmit(&huart2, str_tmp, sprintf((char *) str_tmp, "%d\n\r", (int) red_sample), 1000) == HAL_ERROR)
				{
					Error_Handler();
				}


				bool iad = true;
				while (iad == true) {


					// Start timer
          if(HAL_TIM_Base_GetState(&htim7) == HAL_TIM_STATE_READY) {
            HAL_TIM_Base_Start_IT(&htim7);
          }




					// Get current time
					timer_val = tim_elapsed;
					//timer_val = __HAL_TIM_GET_COUNTER(&htim7);
//					timer_val = HAL_GetTick();

					// Get current time (microseconds)
					//				timer_val = __HAL_TIM_GET_COUNTER(&htim6);
					//				delta = __HAL_TIM_GET_COUNTER(&htim6) - lastBeat;					//Measure duration between two beats, ist zB beim ersten Mal 4 Sekunden

					delta = timer_val - lastBeat;					//Measure duration between two beats, ist zB beim ersten Mal 4 Sekunden

					uint8_t str_tmps[50] ="";

					// Output the value when the finger was detected
					if (HAL_UART_Transmit(&huart2, str_tmps, sprintf((char *) str_tmps, "%d %d\n\r", (int) delta, (int) timer_val), 1000) == HAL_ERROR)
					{
						Error_Handler();
					}
					HAL_Delay(500);



					//				delta = HAL_GetTick() - lastBeat;					//Measure duration between two beats, ist zB beim ersten Mal 4 Sekunden
					//				lastBeat = HAL_GetTick(); // Auch 4 Sekunden
					lastBeat = timer_val;

					beatsPerMinute = 60 / (delta / 1000.0);					//Calculating the BPM

					if (beatsPerMinute < 255 && beatsPerMinute > 20)
					{
						rates[rateSpot++] = (uint8_t)beatsPerMinute;
						rateSpot %= RATE_SIZE; //Wrap variable
						if(rates[3] != 0)
						{
							iad = false;

						}
					}

					//goto startAgain;

				}

				//Take average of readings
				beatAvg = 0;
				for (uint8_t x = 0; x < RATE_SIZE ; x++)
				{
					beatAvg += rates[x];
				}
				beatAvg /= RATE_SIZE;

				/*						// Output the value when the finger was detected
						if (HAL_UART_Transmit(&huart2, str_tmp, sprintf((char *) str_tmp, "%d\\n\r", (int) beatAvg), 1000) == HAL_ERROR)
						{
						Error_Handler();
						}*/

				//				char *pulseValue = "\r\nPulse value: ";
				//				uint8_t str_tmp[7] ="";

				// Print pulse value
				if (HAL_UART_Transmit(&huart2, (uint8_t *) pulseValue, strlen(pulseValue), 1000) == HAL_ERROR)
				{
					Error_Handler();
				}

				// Output the value when the finger was detected
				if (HAL_UART_Transmit(&huart2, str_tmp, sprintf((char *) str_tmp, "%d\\n\r", (int) beatAvg), 1000) == HAL_ERROR)
				{
					Error_Handler();
				}

				/*
				// Output the value when the finger was detected
				if (HAL_UART_Transmit(&huart2, str_tmp, sprintf((char *) str_tmp, "%d\\n\r", (int) red_sample), 1000) == HAL_ERROR)
				{
					Error_Handler();
				}
				 */



				// Start program command input again
				if (HAL_UART_Transmit(&huart2, (uint8_t *)welcome, strlen(welcome), 1000) == HAL_ERROR)
				{
					Error_Handler();
				}



				no_finger_f = false;

			}
		}



		/**
		 *
		 * "BUFFERSIZE" is my size (in our case 10) that I have to transfer.
		 * Once I have entered 10 characters, my callback steps in the function checkInput(hi2c1, huart2, ringBuffer, 10).
		 *
		 */

		// Nonblocking Function
//		if (HAL_UART_Receive_IT(&huart2, ringBuffer, BUFFERSIZE) == HAL_ERROR)
//		{
//			Error_Handler();
//		}
//
//		HAL_Delay(500);

		HAL_Delay(500);



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

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = 0;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
		Error_Handler();
	}
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
	hi2c1.Init.Timing = 0x00000E14;
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

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	if(huart == &huart2)
	{

		checkInput(hi2c1, huart2, ringBuffer, 10);
	}




}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  // Here the magic happens with our custom clock (which just is a counter)
  // Because we have 32MHZ and use a pre-scaler of 32000 that mans our clock speed is 1ms
  // However because I set the period to 9 the interrupt fires every 10ms and the counter therefore increased in 10ms steps
  if (htim->Instance == TIM7) {
    tim_elapsed += htim->Instance -> ARR + 1; // 9 + 1 = 10 ms to add to the counter
  }
}


/* USER CODE END 4 */

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

#include <h4lib.h>
#include <stdlib.h>

uint32_t red_val = 0;
uint32_t red_average = 0;

void checkInput(I2C_HandleTypeDef handleI2C, UART_HandleTypeDef handleUART, uint8_t ringBuffer[], uint8_t size) {

	char *receive = "\r\ndata: #a,";

	char *ackStm32 = "\r\nSTM32: ACK\r\n";
	char *nackStm32 = "\r\nSTM32: NACK\r\n";
	char *ackPc = "\r\nPC: ACK\\n\r\n";

			char *cmdt = NULL;
			cmdt = (char *) ringBuffer;
			char* comparisonCmdt = "#t,tempa\\n";

			// Check each character from my input with the allowed command (#t,tempa\n).
			int result = strcmp(comparisonCmdt, cmdt);

			// Check each element from my input with the allowed command.
			// If the input != 0, the command is forbidden and we return to our command prompt
			if(result != 0)
			{

				return;
			}


	if(ringBuffer[1] == 't') {


/*		char *cmdt = NULL;
		cmdt = (char *) ringBuffer;
		char* comparisonCmdt = "#t,tempa\\n";

		// Check each character from my input with the allowed command (#t,tempa\n).
		int result = strcmp(comparisonCmdt, cmdt);*/




/*		// Check each element from my input with the allowed command.
		// If the input != 0, the command is forbidden and we return to our command prompt
		if(result != 0)
		{
			// Print Nack for negative acknowledge
			if (HAL_UART_Transmit(&handleUART, (uint8_t *) nackStm32, strlen(nackStm32), 1000) == HAL_ERROR)
			{
				Error_Handler();
			}

			// Start terminal with welcome message again
			// TODO: Repeating code, maybe put it in a separate function, gives less lines of code
			if (HAL_UART_Transmit(&handleUART, (uint8_t *)welcome, strlen(welcome), 1000) == HAL_ERROR)
			{
				Error_Handler();
			}

			return;
		}*/


		uint8_t temperature[] = "\r\n**** Temperature values measurement ****\n\r";
		uint8_t initTempMsg[] = "===> Initialize Temperature Sensor MAX30101 \r\n";
		uint8_t initTempMsgFinish[] = "=====> Temperature Sensor MAX30101 initialized\n\r";

		// TODO: Repeating code, maybe put it in a separate function, gives less lines of code
		// Print Acknowledgment from Stm32
		if (HAL_UART_Transmit(&handleUART, (uint8_t *) ackStm32, strlen(ackStm32), 1000) == HAL_ERROR)
		{
			Error_Handler();
		}




		// Print temperature messages
		if(HAL_UART_Transmit(&handleUART, (uint8_t *)temperature, sizeof(temperature), 1000) == HAL_ERROR)
		{
			Error_Handler();
		}

		//TODO: Edit initTempMsg with the real Temperature Chip Name
		if(HAL_UART_Transmit(&handleUART, (uint8_t *)initTempMsg, sizeof(initTempMsg), 1000) == HAL_ERROR)
		{
			Error_Handler();
		}
		//TODO: Edit initTempMsgFinish with the real Temperature Chip Name

		if(HAL_UART_Transmit(&handleUART, (uint8_t *)initTempMsgFinish, sizeof(initTempMsgFinish), 1000) == HAL_ERROR)
		{
			Error_Handler();
		}

		// Print data:
		if (HAL_UART_Transmit(&handleUART, (uint8_t *)receive, strlen(receive), 1000) == HAL_ERROR)

		{
			Error_Handler();
		}


		// Get Temperature from chip
		hr4_get_chipTemp(handleI2C, handleUART);

		// Print Acknowledgment from PC
		if (HAL_UART_Transmit(&handleUART, (uint8_t *) ackPc, strlen(ackPc), 1000) == HAL_ERROR)
		{
			Error_Handler();
		}



	} else {

		// Print Nack for negative acknowledge
		if (HAL_UART_Transmit(&handleUART, (uint8_t *) nackStm32, strlen(nackStm32), 1000) == HAL_ERROR)
		{
			Error_Handler();
		}


	}
}


void hr4_test_partID(I2C_HandleTypeDef handleI2C, UART_HandleTypeDef handleUART) {

	/* The 0xFF register address of the Part-ID */
	static const uint8_t partID = 0xFF;
	uint8_t buf[11];
	/* We save the return value in this variable */
	HAL_StatusTypeDef ret;

	/* We set the first byte of the buffer to the location of the register */
	/* Tell heartrate4 that we want to read from the register */
	/* We select with our master the slave and start an event */

	buf[0] = partID;

	ret = HAL_I2C_Master_Transmit(&handleI2C, HEARTRATE4_ADDR << 1, buf, 1, HAL_MAX_DELAY);
	if (ret != HAL_OK) {
		strcpy((char*)buf, "Error Tx\r\n");
	} else {

		/* Read 1 bytes from the register */
		/* The slave sends the master after ACK the desired values from the register in buf[0] */
		ret = HAL_I2C_Master_Receive(&handleI2C, HEARTRATE4_ADDR << 1, buf, 1, HAL_MAX_DELAY);
		if (ret != HAL_OK) {
			strcpy((char*)buf, "Error Rx\r\n");
		} else {
			//			 HAL_UART_Transmit(&handleUART, buf, strlen((char*)buf), HAL_MAX_DELAY);
			HAL_UART_Transmit(&handleUART, &(buf[0]), strlen((char*)buf), HAL_MAX_DELAY);

			HAL_Delay(500);


		}
	}


}


void hr4_get_chipTemp(I2C_HandleTypeDef handleI2C, UART_HandleTypeDef handleUART) {


	static const uint8_t temp_integer = 0x1F;
	//	static const uint8_t temp_fraction = 0x20;
	static const uint8_t temp_EN = 0x21;


	uint8_t buf[BUFFERSIZE];

	buf[0] = temp_EN;
	buf[1] = 1; //R/W-Bit = 1 für lesen
	uint8_t str_tmp[21] ="";

	/**
	 *  We set HEARTRATE4_ADDR << 1 in I2C_Master_Transmit to start the communication with the click module,  and enable
	 *  TEMP_EN with the register address 0x21 and the second byte with 1 to enable it .
	 *  We select with our master the slave and start an event
	 */

	if (HAL_I2C_Master_Transmit(&handleI2C, HEARTRATE4_ADDR << 1, buf, 2, HAL_MAX_DELAY) == HAL_ERROR) {
		Error_Handler();

	}



	/**
	 * We set the first byte of the buffer to the location of the register (temp_integer).
	 *  Tell heartrate4 that we want to read from this register.
	 */

	buf[0] = temp_integer;

	if (HAL_I2C_Master_Transmit(&handleI2C, HEARTRATE4_ADDR << 1, buf, 1, HAL_MAX_DELAY) == HAL_ERROR) {
		Error_Handler();

	}


	/**
	 * Here we tell the temp sensor that we want to read two bytes from
	 * the register (temp_Integer (0x1F = 31) and the next byte temp_fraction (0x20 = 32).
	 */
	if (HAL_I2C_Master_Receive(&handleI2C, HEARTRATE4_ADDR << 1, buf, 2, HAL_MAX_DELAY) == HAL_ERROR) {
		Error_Handler();

	}




	float floatSum = buf[0] + 0.0625 * buf[1];

	// Send the Chip-Temperature to our Webserver, and save it in our Database
	wifi_click_send_test(floatSum);



	// Nonblocking Function
	// Print temperature
	if (HAL_UART_Transmit(&handleUART, str_tmp, sprintf((char *) str_tmp, "%d\\n\r", (int) floatSum), 1000) == HAL_ERROR)
	{
		Error_Handler();
	}




}



// HELPER FUNCTIONS
uint8_t hr4_read_reg ( uint8_t reg, I2C_HandleTypeDef hi2c1handle )
{

	uint16_t addr = 0x57;
	uint16_t devAddr = addr<<1;
	uint8_t regnr = reg;
	uint8_t dataReceived[2];
	uint8_t new_value;

	/**
	 * We request here the register xxx, from this we would like to read.
	 */
	if (HAL_I2C_Master_Transmit(&hi2c1handle, devAddr,&regnr,1,HAL_MAX_DELAY) == HAL_ERROR)
	{
		Error_Handler();
	}

	/**
	 * I want to read one byte from register xxx.
	 */
	if (HAL_I2C_Master_Receive(&hi2c1handle, devAddr, dataReceived, 1, HAL_MAX_DELAY) == HAL_ERROR)
	{
		Error_Handler();
	}

	new_value = dataReceived[0];

	return new_value;
}

/**
 * The function may not work properly, still needs to be tested.
 */
void hr4_read_reg_multi ( uint8_t reg, uint8_t *buffer, uint8_t count, I2C_HandleTypeDef hi2c1handle)
{
	uint16_t addr = 0x57;
	uint16_t devAddr = addr<<1;
	uint8_t* new_value = buffer;
	new_value[0] = reg;

	/**
	 * We request here the register xxx, from this we would like to read.
	 * I need here only the indication of the register.
	 */
	if (HAL_I2C_Master_Transmit(&hi2c1handle, devAddr,&new_value[0], 1,HAL_MAX_DELAY) == HAL_ERROR)
	{
		Error_Handler();
	}

	/**
	 * I want to read one byte from register xxx.
	 */
	if (HAL_I2C_Master_Receive(&hi2c1handle, devAddr, buffer, count, HAL_MAX_DELAY) == HAL_ERROR)
	{
		Error_Handler();
	}



}

void hr4_write_reg( uint8_t reg, uint8_t reg_val, I2C_HandleTypeDef hi2c1handle)
{

	uint16_t addr = 0x57;
	uint16_t devAddr = addr<<1;
	uint8_t dataSend[2];

	dataSend[0] = reg;
	dataSend[1] = reg_val;


	if (HAL_I2C_Master_Transmit(&hi2c1handle, devAddr, dataSend, 2, HAL_MAX_DELAY) == HAL_ERROR)
	{
		Error_Handler();
	}

}


void hr4_set_registers(I2C_HandleTypeDef hi2c1handle)
{
	// FIFO Configuration
	hr4_set_fifo_sample_averaging( 0x05, hi2c1handle); // 32 sample averaging
	hr4_fifo_rollover_enable( 1, hi2c1handle);         // Allow FIFO to wrap/roll over
	hr4_proximity_int_enable( 1, hi2c1handle);         // Enable Proximity Threshold Triggering
	hr4_set_mode( 0x02, hi2c1handle);                  // Entering HR mode, RED LED only!
	hr4_set_spo2_sample_rate( 0x05, hi2c1handle);      // 1000 sample rate

	// Enabling interrupt in case of new data arrival, and disable when near full
	hr4_new_fifo_data_int_enable( 1, hi2c1handle);
	hr4_fifo_full_int_enable( 0, hi2c1handle );

	// Setting LED Pulse Amplitudes
	hr4_set_red_led_pulse_amplitude( 0x1F, hi2c1handle );
	hr4_set_ir_led_pulse_amplitude( 0x1F, hi2c1handle );
	hr4_set_green_led_pulse_amplitude( 0x1F, hi2c1handle );
	hr4_set_proximity_led_pulse_amplitude( 0x0F, hi2c1handle );

	// Setting PROX THRESHOLD value to 31743 ( 0x01 is 1023)
	hr4_set_proximity_threshold( 0x1F, hi2c1handle );
}

int8_t hr4_set_fifo_sample_averaging( uint8_t sample_number, I2C_HandleTypeDef hi2c1handle)
{
	uint16_t addr = 0x57;
	uint16_t devAddr = addr<<1;
	uint8_t regnr = 0x08;
	uint8_t dataReceived[2];
	uint8_t dataSend[2];
	uint8_t new_value;
	HAL_StatusTypeDef ret;

	/**
	 * We request the register 0x08 here, we want to read from it.
	 */
	ret = HAL_I2C_Master_Transmit(&hi2c1handle, devAddr,&regnr,1,HAL_MAX_DELAY);



	/**
	 * I want to read one byte from register 0x08.
	 */
	ret = HAL_I2C_Master_Receive(&hi2c1handle, devAddr, dataReceived, 1, HAL_MAX_DELAY);


	new_value = dataReceived[0];

	/**
	 * Here we change from the returned byte only the last 3 bytes (xxx1 1010)
	 *  sample_number = 0x5 = 101 we write in new_value on bit 7-5.
	 */

	new_value |=   (sample_number) << 5;
	dataSend[0] = regnr;
	dataSend[1] = new_value;

	ret = HAL_I2C_Master_Transmit(&hi2c1handle, devAddr, dataSend, 2, HAL_MAX_DELAY);

	return 0;

}

void hr4_fifo_rollover_enable( bool enable, I2C_HandleTypeDef hi2c1handle)
{
	uint16_t addr = 0x57;
	uint16_t devAddr = addr<<1;
	uint8_t regnr = 0x08;
	uint8_t dataReceived[2];
	uint8_t dataSend[2];

	/**
	 * We request the register 0x08 here, we want to read from it.
	 */
	if (HAL_I2C_Master_Transmit(&hi2c1handle, devAddr,&regnr,1,HAL_MAX_DELAY) == HAL_ERROR)
	{
		Error_Handler();
	}

	/**
	 * I want to read one byte from register 0x08.
	 */
	if (HAL_I2C_Master_Receive(&hi2c1handle, devAddr, dataReceived, 1, HAL_MAX_DELAY) == HAL_ERROR)
	{
		Error_Handler();
	}

	/**
	 * Here the received byte is ORed with the number 1 which has been shifted 4 places to the left.
	 */
	dataReceived[0] |= (1) << 4;
	dataSend[0] = regnr;
	dataSend[1] = dataReceived[0];

	/**
	 * Here we send back the changes in the register, 2 bytes.
	 */
	if (HAL_I2C_Master_Transmit(&hi2c1handle, devAddr, dataSend, 2, HAL_MAX_DELAY) == HAL_ERROR)
	{
		Error_Handler();
	}


}

void hr4_proximity_int_enable( bool enable, I2C_HandleTypeDef hi2c1handle)
{
	uint16_t addr = 0x57;
	uint16_t devAddr = addr<<1;
	uint8_t regnr = 0x02;
	uint8_t dataReceived[2];
	uint8_t dataSend[2];


	/**
	 * We request the register 0x02 here, we want to read from it.
	 */
	if (HAL_I2C_Master_Transmit(&hi2c1handle, devAddr,&regnr,1,HAL_MAX_DELAY) == HAL_ERROR)
	{
		Error_Handler();
	}

	/**
	 * I want to read one byte from register 0x02.
	 */
	if (HAL_I2C_Master_Receive(&hi2c1handle, devAddr, dataReceived, 1, HAL_MAX_DELAY) == HAL_ERROR)
	{
		Error_Handler();
	}

	/**
	 * Here the received byte is ORed with the number 1 which has been shifted 4 places to the left.
	 */
	dataReceived[0] |= (1) << 4;
	dataSend[0] = regnr;
	dataSend[1] = dataReceived[0];


	/**
	 * Here we send back the changes in the register.
	 */
	if (HAL_I2C_Master_Transmit(&hi2c1handle, devAddr, dataSend, 2, HAL_MAX_DELAY) == HAL_ERROR)
	{
		Error_Handler();
	}

}

int8_t hr4_set_mode( uint8_t mode, I2C_HandleTypeDef hi2c1handle)
{
	uint16_t addr = 0x57;
	uint16_t devAddr = addr<<1;
	uint8_t regnr = 0x09;
	uint8_t dataSend[2] = {0};



	if ((mode > 7) || (mode == 0) || (mode == 1) || (mode == 4) || (mode == 5) || (mode == 6))
	{
		return -1;
	}

	dataSend[0] = regnr;
	dataSend[1] |= mode;


	/**
	 * Here we send back the changes in the register.
	 */
	if (HAL_I2C_Master_Transmit(&hi2c1handle, devAddr, dataSend, 2, HAL_MAX_DELAY) == HAL_ERROR)
	{
		Error_Handler();
	}


	return 0;
}

int8_t hr4_set_spo2_sample_rate( uint8_t rate, I2C_HandleTypeDef hi2c1handle)
{
	uint16_t addr = 0x57;
	uint16_t devAddr = addr<<1;
	uint8_t regnr = 0x0A;
	uint8_t dataReceived[2];
	uint8_t dataSend[2];


	if (rate > 7)
	{
		return -1;
	}

	/**
	 * We request the register 0x0A here, we want to read from it.
	 */
	if (HAL_I2C_Master_Transmit(&hi2c1handle, devAddr,&regnr,1,HAL_MAX_DELAY) == HAL_ERROR)
	{
		Error_Handler();
	}

	/**
	 * I want to read one byte from register 0x0A.
	 */
	if (HAL_I2C_Master_Receive(&hi2c1handle, devAddr, dataReceived, 1, HAL_MAX_DELAY) == HAL_ERROR)
	{
		Error_Handler();
	}

	/**
	 * Here the received byte is ORed with the rate which has been shifted 2 places to the left.
	 */
	dataReceived[0] |= rate << 2;
	dataSend[0] = regnr;
	dataSend[1] = dataReceived[0];

	/**
	 * Here we send back the changes in the register.
	 */
	if (HAL_I2C_Master_Transmit(&hi2c1handle, devAddr, dataSend, 2, HAL_MAX_DELAY) == HAL_ERROR)
	{
		Error_Handler();
	}


	return 0;
}

void hr4_new_fifo_data_int_enable( bool enable, I2C_HandleTypeDef hi2c1handle)
{
	uint16_t addr = 0x57;
	uint16_t devAddr = addr<<1;
	uint8_t regnr = 0x02;
	uint8_t dataReceived[2];
	uint8_t dataSend[2];

	/**
	 * We request the register 0x02 here, we want to read from it.
	 */
	if (HAL_I2C_Master_Transmit(&hi2c1handle, devAddr,&regnr,1,HAL_MAX_DELAY) == HAL_ERROR)
	{
		Error_Handler();
	}

	/**
	 * I want to read one byte from register 0x02.
	 */
	if (HAL_I2C_Master_Receive(&hi2c1handle, devAddr, dataReceived, 1, HAL_MAX_DELAY) == HAL_ERROR)
	{
		Error_Handler();
	}

	if (enable)
	{

		/**
		 *  Here the received byte is ORed with the number 1 which has been shifted 6 places to the left.
		 */
		dataReceived[0] |= 1 << 6;
		dataSend[0] = regnr;
		dataSend[1] = dataReceived[0];

	} else
	{
		dataReceived[0] &= ~(1 << 6);
		dataSend[0] = regnr;
		dataSend[1] = dataReceived[0];
	}

	/**
	 * Here we send back the changes in the register.
	 */
	if (HAL_I2C_Master_Transmit(&hi2c1handle, devAddr, dataSend, 2, HAL_MAX_DELAY) == HAL_ERROR)
	{
		Error_Handler();
	}

}

void hr4_fifo_full_int_enable( bool enable, I2C_HandleTypeDef hi2c1handle)
{
	uint16_t addr = 0x57;
	uint16_t devAddr = addr<<1;
	uint8_t regnr = 0x02;
	uint8_t dataReceived[2];
	uint8_t dataSend[2];


	/**
	 * We request the register 0x02 here, we want to read from it.
	 */
	if (HAL_I2C_Master_Transmit(&hi2c1handle, devAddr,&regnr,1,HAL_MAX_DELAY) == HAL_ERROR)
	{
		Error_Handler();
	}

	/**
	 * I want to read one byte from register 0x02.
	 */
	if (HAL_I2C_Master_Receive(&hi2c1handle, devAddr, dataReceived, 1, HAL_MAX_DELAY) == HAL_ERROR)
	{
		Error_Handler();
	}


	if (enable)
	{
		dataReceived[0] |= (1 << 7);
		dataSend[0] = regnr;
		dataSend[1] = dataReceived[0];
	}
	else
	{
		dataReceived[0] &= ~(1 << 7);
		dataSend[0] = regnr;
		dataSend[1] = dataReceived[0];

	}

	/**
	 * Here we send back the changes in the register.
	 */
	if (HAL_I2C_Master_Transmit(&hi2c1handle, devAddr, dataSend, 2, HAL_MAX_DELAY) == HAL_ERROR)
	{
		Error_Handler();
	}


}

void hr4_set_red_led_pulse_amplitude( uint8_t amplitude, I2C_HandleTypeDef handleI2C )
{
	hr4_write_reg( 0x0C, amplitude, handleI2C);
}

void hr4_set_ir_led_pulse_amplitude( uint8_t amplitude, I2C_HandleTypeDef handleI2C )
{
	hr4_write_reg( 0x0D, amplitude, handleI2C);
}

void hr4_set_green_led_pulse_amplitude( uint8_t amplitude, I2C_HandleTypeDef handleI2C )
{
	hr4_write_reg( 0x0E, amplitude, handleI2C);
}

void hr4_set_proximity_led_pulse_amplitude( uint8_t amplitude, I2C_HandleTypeDef handleI2C )
{
	hr4_write_reg( 0x10, amplitude, handleI2C);
}

void hr4_set_proximity_threshold( uint8_t threshold, I2C_HandleTypeDef handleI2C )
{
	hr4_write_reg( 0x30, threshold, handleI2C);
}

uint8_t hr4_is_new_fifo_data_ready(I2C_HandleTypeDef handleI2C)
{

	uint8_t regnr = 0x00;



	uint8_t temp = hr4_read_reg ( regnr, handleI2C );
	/**
	 * We shift all bits in 0x00 6 places to the right.
	 *	That means only A_FULL and PPG_RDY are left.
	 * A_FULL is assumed to be 0 anyway, even if it is full there is still data.
	 * If one of the two bits (A_FULL or PPG_RDY) is set to 1, data is available.
	 */
	temp >>= 6;

	return temp;
}

uint32_t hr4_read_red(I2C_HandleTypeDef handleI2C)
{
	uint8_t i = 0;
	uint8_t sampleNum = 0;
	uint8_t wrPtr = 0;
	uint8_t rdPtr = 0;
	uint8_t temp_red[ 4 ];
	uint8_t temp[ 3 ] = { 0 };

	red_average = 0;

	wrPtr = hr4_get_write_pointer(handleI2C);
	rdPtr = hr4_get_read_pointer(handleI2C);

	sampleNum = abs( 16 + wrPtr - rdPtr ) % 16;

	if ( sampleNum >= 1 )
	{
		for ( i = 0; i < sampleNum; ++i )
		{
			hr4_read_reg_multi ( 0x07, temp, 3, handleI2C );

			temp_red[ 3 ] = 0;
			temp_red[ 2 ] = temp[ 0 ] & 0x03;
			temp_red[ 1 ] = temp[ 1 ];
			temp_red[ 0 ] = temp[ 2 ];

			// Convert array to uint32
			memcpy( &red_val, temp_red, 4 );

			red_average += red_val;
		}
		red_average /= sampleNum;
	}
	return red_average;
}

uint8_t hr4_get_write_pointer( I2C_HandleTypeDef handleI2C )
{
	return hr4_read_reg( 0x04, handleI2C );
}

uint8_t hr4_get_read_pointer( I2C_HandleTypeDef handleI2C )
{
	return hr4_read_reg( 0x06, handleI2C );
}

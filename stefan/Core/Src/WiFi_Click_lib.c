/**
 ******************************************************************************
 * @file           : WiFi_Click_lib.c
 * @brief          : This file initializes our WiFi with a hotspot and sends our POST requests
 * 					 via TCP to the web server which receives them via a PHP script.
 * @author		   : Stefan Bittgen
 * @date		   : 03.07.2022
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <string.h>
#include <stdio.h>
#include "cmsis_os.h"

// prepared strings with AT commands to be sent
char AT_TEST[] = "AT\r\n";
char AT_MODESEL[] = "AT+CWMODE=1\r\n";
char AT_SETIPSTATIC[] = "AT+CIPSTA=\"192.168.97.228\"\r\n";
char AT_WIFICONNECT[] = "AT+CWJAP=\"AndroidAP6156\",\"Balou1407%\"\r\n";

char AT_GETIP[] = "AT+CIFSR\r\n";

char RX_Buffer[500];

/**
@brief Initialiaze WiFi
@retval none
*/
void wifi_click_init(void) {
	HAL_UART_Transmit(&huart1, (uint8_t*)AT_TEST, strlen(AT_TEST), HAL_MAX_DELAY);
	HAL_UART_Receive(&huart1,(uint8_t*)RX_Buffer,500,5000);
	HAL_UART_Transmit(&huart2, (uint8_t*)RX_Buffer, strlen(RX_Buffer), HAL_MAX_DELAY);
	HAL_Delay(2000);

	HAL_UART_Transmit(&huart1, (uint8_t*)AT_MODESEL, strlen(AT_MODESEL), HAL_MAX_DELAY);
	HAL_UART_Receive(&huart1,(uint8_t*)RX_Buffer,500,5000);
	HAL_UART_Transmit(&huart2, (uint8_t*)RX_Buffer, strlen(RX_Buffer), HAL_MAX_DELAY);
	HAL_Delay(2000);

	HAL_UART_Transmit(&huart1, (uint8_t*)AT_SETIPSTATIC, strlen(AT_SETIPSTATIC), HAL_MAX_DELAY);
	HAL_UART_Receive(&huart1,(uint8_t*)RX_Buffer,500,5000);
	HAL_UART_Transmit(&huart2, (uint8_t*)RX_Buffer, strlen(RX_Buffer), HAL_MAX_DELAY);
	HAL_Delay(2000);

	HAL_UART_Transmit(&huart1, (uint8_t*)AT_WIFICONNECT, strlen(AT_WIFICONNECT), HAL_MAX_DELAY);
	HAL_UART_Receive(&huart1,(uint8_t*)RX_Buffer,500,5000);
	HAL_UART_Transmit(&huart2, (uint8_t*)RX_Buffer, strlen(RX_Buffer), HAL_MAX_DELAY);
	HAL_Delay(5000);

	memset(RX_Buffer,0,sizeof(RX_Buffer));

	HAL_UART_Transmit(&huart1, (uint8_t*)AT_GETIP, strlen(AT_GETIP), HAL_MAX_DELAY);
	HAL_UART_Receive(&huart1,(uint8_t*)RX_Buffer,500,5000);
	HAL_UART_Transmit(&huart2, (uint8_t*)RX_Buffer, strlen(RX_Buffer), HAL_MAX_DELAY);
}

/**
@brief We send here the POST-Request to our webserver
@param chipTemp: Chip Temperature from the module
@param heartRate: Pulse from the module
@retval none
*/
void wifi_click_send_test(int chipTemp, int heartRate) {

	char AT_CIPSTART[]="AT+CIPSTART=\"TCP\",\"192.168.97.221\",80\r\n";
	char AT_CIPCLOSE[]="AT+CIPCLOSE\r\n"; //close TCP connection string

     //HTTP-Header Example
/*	 "POST /post-esp-data.php HTTP/1.1\r\n"
	 "Host: 192.168.149.221\r\n"
	 "Content-Type: application/x-www-form-urlencoded\r\n"
	 "Content-Length: 23\r\n\r\n"
	 Attention: The content length must be correct, otherwise no successful HTTP post request can be sent

     //HTTP-Body
	 "temp=38&heart_rate=96\r\n";
*/

	char buf[155] = {0};


	char hrBuf[10] = {'0'};
	char tempBuf[10] = {'0'};
	char httpBody1[23] =  "temp=";
	char httpBody2[15] =  "&heart_rate=";

	snprintf(hrBuf, 10, "%d", heartRate);
	snprintf(tempBuf, 10, "%d", chipTemp);
	strcat(httpBody1, tempBuf);
	strcat(httpBody2, hrBuf);
	strcat(httpBody1, httpBody2);
	int lengthBody = strlen(httpBody1)+2;

		snprintf(buf, 180, "POST /post-esp-data.php HTTP/1.1\r\n"
	 	  	  	     "Host: 192.168.97.221\r\n"
	 	  	  	     "Content-Type: application/x-www-form-urlencoded\r\n"
	                 "Content-Length: %d\r\n\r\n"
					 "%s\r\n", lengthBody, httpBody1);



	char AT_CIPSEND_MSG[20];

	// get length
	int lenghtOfData = strlen(buf);


	// prepare string with the length of data to be expected by WiFi-Click
	sprintf(AT_CIPSEND_MSG, "AT+CIPSEND=%d\r\n", lenghtOfData);

	// sending string variables via UART -> UART1 = wi-fi; UART2 = serial terminal;
	//establish TCP connection
	HAL_UART_Transmit(&huart1, (uint8_t*)AT_CIPSTART, strlen(AT_CIPSTART), HAL_MAX_DELAY);
	osDelay(1000);

	// let esp32 know how much data to expect
	HAL_UART_Transmit(&huart1, (uint8_t*)AT_CIPSEND_MSG, strlen(AT_CIPSEND_MSG), HAL_MAX_DELAY);
	osDelay(1000);

	// send data via wifi
	HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), HAL_MAX_DELAY);

	osDelay(5000);

	//close TCP connection
	HAL_UART_Transmit(&huart1, (uint8_t*)AT_CIPCLOSE, strlen(AT_CIPCLOSE), HAL_MAX_DELAY);
	osDelay(1000);
}

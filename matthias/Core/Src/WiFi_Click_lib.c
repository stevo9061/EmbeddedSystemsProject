/**
 ******************************************************************************
 * @file           : WiFi_Click_lib.c
 * @brief          : This file contains very basic functions for initializing
 *                   the WiFi-Click as well as sending a test message via TCP.
 * @author		   : Patrick Schmitt
 * @date		   : 15.05.2022
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <string.h>
#include <stdio.h>
#include "cmsis_os.h"

//prepared strings with AT commands to be sent
char AT_TEST[] = "AT\r\n";
char AT_MODESEL[] = "AT+CWMODE=1\r\n";
  char AT_SETIPSTATIC[] = "AT+CIPSTA=\"192.168.0.53\"\r\n";
//char AT_WIFICONNECT[] = "AT+CWJAP=\"Magenta-6HQ256\",\"Password\"\r\n"; //replace SSID with the network name and PASSWORD with wifi password
char AT_WIFICONNECT[] = "AT+CWJAP=\"AndroidAP6156\",\"Balou1407%\"\r\n";

char AT_GETIP[] = "AT+CIFSR\r\n";

char RX_Buffer[500];

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
	HAL_Delay(20000);

	memset(RX_Buffer,0,sizeof(RX_Buffer));

	HAL_UART_Transmit(&huart1, (uint8_t*)AT_GETIP, strlen(AT_GETIP), HAL_MAX_DELAY);
	HAL_UART_Receive(&huart1,(uint8_t*)RX_Buffer,500,5000);
	HAL_UART_Transmit(&huart2, (uint8_t*)RX_Buffer, strlen(RX_Buffer), HAL_MAX_DELAY);
}

void wifi_click_send_test(float chipTemp) {

	char AT_CIPSTART[]="AT+CIPSTART=\"TCP\",\"192.168.0.218\",80\r\n";
	char AT_CIPCLOSE[]="AT+CIPCLOSE\r\n"; //close TCP connection string

	char buf[155] = {0};

	int heartR = (int) chipTemp;
	int temp = 96;  //TODO: Replace with live value


	char hrBuf[10] = {'0'};
	char tempBuf[10] = {'0'};
	char httpBody1[23] =  "temp=";
	char httpBody2[15] =  "&heart_rate=";

	snprintf(hrBuf, 10, "%d", heartR);
	snprintf(tempBuf, 10, "%d", temp);
	strcat(httpBody1, hrBuf);
	strcat(httpBody2, tempBuf);
	strcat(httpBody1, httpBody2);
	int lengthBody = strlen(httpBody1)+2;

	char *get_request = "GET /fetch-latest-data.php HTTP/1.1\r\nHost: 192.168.0.218\r\n\r\n";

//		snprintf(buf, 180, "POST /post-esp-data.php HTTP/1.1\r\n"
//	 	  	  	     "Host: 192.168.0.218\r\n"
//	 	  	  	     "Content-Type: application/x-www-form-urlencoded\r\n"
//	                 "Content-Length: %d\r\n\r\n"
//					 "%s\r\n", lengthBody, httpBody1);

	char AT_CIPSEND_MSG[20];

	int lenghtOfData = strlen(get_request); // buf

	//prepare string with the length of data to be expected by WiFi-Click
	sprintf(AT_CIPSEND_MSG, "AT+CIPSEND=%d\r\n", lenghtOfData);

	/* sending string variables via UART -> UART1 = wi-fi; UART2 = serial terminal;*/
	//establish TCP connection
	HAL_UART_Transmit(&huart1, (uint8_t*)AT_CIPSTART, strlen(AT_CIPSTART), HAL_MAX_DELAY);
	osDelay(1000);
  HAL_UART_Receive(&huart1,(uint8_t*)RX_Buffer,500,5000);
  osDelay(5000);
  HAL_UART_Transmit(&huart2, (uint8_t*)RX_Buffer, strlen(RX_Buffer), HAL_MAX_DELAY);
  osDelay(20000);

	//let esp32 know how much data to expect
	HAL_UART_Transmit(&huart1, (uint8_t*)AT_CIPSEND_MSG, strlen(AT_CIPSEND_MSG), HAL_MAX_DELAY);
	osDelay(1000);
  HAL_UART_Receive(&huart1,(uint8_t*)RX_Buffer,500,5000);
  osDelay(5000);
  HAL_UART_Transmit(&huart2, (uint8_t*)RX_Buffer, strlen(RX_Buffer), HAL_MAX_DELAY);
  osDelay(20000);

	// send data via wifi
	HAL_UART_Transmit(&huart1, (uint8_t*)get_request, strlen(get_request), HAL_MAX_DELAY);
	osDelay(10000);
  HAL_UART_Receive(&huart1,(uint8_t*)RX_Buffer,500,5000);
  osDelay(5000);
  HAL_UART_Transmit(&huart2, (uint8_t*)RX_Buffer, strlen(RX_Buffer), HAL_MAX_DELAY);
  osDelay(20000);

	//close TCP connection
	HAL_UART_Transmit(&huart1, (uint8_t*)AT_CIPCLOSE, strlen(AT_CIPCLOSE), HAL_MAX_DELAY);
	osDelay(1000);
  HAL_UART_Receive(&huart1,(uint8_t*)RX_Buffer,500,5000);
  osDelay(5000);
  HAL_UART_Transmit(&huart2, (uint8_t*)RX_Buffer, strlen(RX_Buffer), HAL_MAX_DELAY);
  osDelay(20000);
}

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
  char AT_SETIPSTATIC[] = "AT+CIPSTA=\"192.168.149.222\"\r\n";
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
	HAL_Delay(5000);

	memset(RX_Buffer,0,sizeof(RX_Buffer));

	HAL_UART_Transmit(&huart1, (uint8_t*)AT_GETIP, strlen(AT_GETIP), HAL_MAX_DELAY);
	HAL_UART_Receive(&huart1,(uint8_t*)RX_Buffer,500,5000);
	HAL_UART_Transmit(&huart2, (uint8_t*)RX_Buffer, strlen(RX_Buffer), HAL_MAX_DELAY);
}

void wifi_click_send_test(void) {
  printf("starte wifi_click_send_test...\n");
//	char AT_CIPSTART[]="AT+CIPSTART=\"TCP\",\"192.168.1.69\",6000\r\n";
	char AT_CIPSTART[]="AT+CIPSTART=\"TCP\",\"192.168.149.221\",80\r\n";
	char AT_CIPCLOSE[]="AT+CIPCLOSE\r\n"; //close TCP connection string
//	char TEST_MSG[]="A big brown fox jumps over...\r\n";  //prepare string to be send - just a test message
	char TEST_MSG[] = "POST /post-esp-data.php HTTP/1.1\r\n"
						      "Host: 192.168.149.221\r\n"
						      "Content-Type: application/x-www-form-urlencoded\r\n"
						      "Content-Length: 79\r\n\r\n"
						      "api_key=tPmAT5Ab3j7F9&sensor=BME280&location=Office&value1=24.75&value2=49.54\r\n";
	char AT_CIPSEND_MSG[20];

	//get length
	int lenghtOfData = strlen(TEST_MSG);

	//prepare string with the length of data to be expected by WiFi-Click
	sprintf(AT_CIPSEND_MSG, "AT+CIPSEND=%d\r\n", lenghtOfData);

	/* sending string variables via UART -> UART1 = wi-fi; UART2 = serial terminal;*/
	//establish TCP connection
	HAL_UART_Transmit(&huart1, (uint8_t*)AT_CIPSTART, strlen(AT_CIPSTART), HAL_MAX_DELAY);
	osDelay(1000);

	//let esp32 know how much data to expect
	HAL_UART_Transmit(&huart1, (uint8_t*)AT_CIPSEND_MSG, strlen(AT_CIPSEND_MSG), HAL_MAX_DELAY);
	osDelay(1000);

	// send data via wifi
	HAL_UART_Transmit(&huart1, (uint8_t*)TEST_MSG, strlen(TEST_MSG), HAL_MAX_DELAY);
//	osDelay(1000);
	osDelay(5000);

	//close TCP connection
	HAL_UART_Transmit(&huart1, (uint8_t*)AT_CIPCLOSE, strlen(AT_CIPCLOSE), HAL_MAX_DELAY);
	printf("finish wifi_click_send_test...\n");
	osDelay(1000);
}

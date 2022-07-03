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
#include "string.h"
#include "stdio.h"
#include "cmsis_os.h"

// TODO:
//char *WIFI_STATIC_API = ...
//char *WIFI_SSID = ...
//char *WIFI_PASSWORD = ...

void wifi_click_init(void) {
	uart_transmit_with_echo(huart1, huart2, "AT\r\n");
	uart_transmit_with_echo(huart1, huart2, "AT+CWMODE=1\r\n");
	uart_transmit_with_echo(huart1, huart2, "AT+CIPSTA=\"192.168.97.226\"\r\n"); // 192.168.0.53
	uart_transmit_with_echo(huart1, huart2, "AT+CWJAP=\"AndroidAP6156\",\"Balou1407%\"\r\n");
	uart_transmit_with_echo(huart1, huart2, "AT+CIFSR\r\n");
}

char * wifi_click_fetch_data() {

  char *get_request = "GET /fetch-latest-data.php HTTP/1.1\r\nHost: 192.168.97.221\r\n\r\n"; // .0.218

	char AT_CIPSTART[]="AT+CIPSTART=\"TCP\",\"192.168.97.221\",80\r\n"; // ,7200 for tcp keep alive, but didn't work for me..
	char AT_CIPCLOSE[]="AT+CIPCLOSE\r\n"; //close TCP connection string

  // establish TCP connection (I couldn't make keeplive work. so sad :(
  uart_transmit_with_echo(huart1, huart2, AT_CIPSTART);

	// prepare string with the length of data to be expected by WiFi-Click
	char AT_CIPSEND_MSG[20];
	sprintf(AT_CIPSEND_MSG, "AT+CIPSEND=%d\r\n", strlen(get_request));
	// let esp32 know how much data to expect
  uart_transmit_with_echo(huart1, huart2, AT_CIPSEND_MSG);
	// send data via wifi
  char buf[1024] = {0};
  char *data = uart_transmit_with_echo(huart1, huart2, get_request);
  strcpy(buf, data);
  char *ret = buf;
	//close TCP connection
  uart_transmit_with_echo(huart1, huart2, AT_CIPCLOSE);
  return ret;
}

/* sending string variables via UART -> UART1 = wi-fi; UART2 = serial terminal;*/
char * uart_transmit_with_echo(UART_HandleTypeDef huart_tx, UART_HandleTypeDef huart_echo, char *tx_data) {
  char RX_Buffer[1024] = {0};
  HAL_UART_Transmit(&huart_tx, (uint8_t*)tx_data, strlen(tx_data), HAL_MAX_DELAY);
  HAL_UART_Receive(&huart_tx,(uint8_t*)RX_Buffer,1024,5000);
  HAL_UART_Transmit(&huart2, (uint8_t*)RX_Buffer, strlen(RX_Buffer), HAL_MAX_DELAY);
  char *ret = RX_Buffer;
  return ret;
}

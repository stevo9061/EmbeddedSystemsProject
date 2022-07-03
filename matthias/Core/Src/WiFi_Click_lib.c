/**
  ******************************************************************************
  * @file           : WiFi_Click_lib.c
  * @author         : Matthias Kurz
  * @brief          : Functions used to initialize and control the WiFi click
  *                   board, so we are able to send and receive message via the
  *                   TCP protocoll (which can be used for HTTP requests).
  ******************************************************************************
  */

#include "main.h"
#include "string.h"
#include "stdio.h"
#include "cmsis_os.h"

/**
  * @brief  Initializes the WiFi module.
  * @note   Sets the stations mode of the WiFi module to "client", sets a static
  * IP address and afterwards connects the module to a given WiFi network specificed by
  * SSID and password.
  * @retval None
  */
void wifi_click_init(void) {
	uart_transmit_with_echo(huart1, huart2, "AT\r\n");
	uart_transmit_with_echo(huart1, huart2, "AT+CWMODE=1\r\n");
	uart_transmit_with_echo(huart1, huart2, "AT+CIPSTA=\"192.168.97.226\"\r\n");
	uart_transmit_with_echo(huart1, huart2, "AT+CWJAP=\"AndroidAP6156\",\"Balou1407%\"\r\n");
	uart_transmit_with_echo(huart1, huart2, "AT+CIFSR\r\n");
}

/**
  * @brief  Fetches data with a HTTP GET request (over TCP of course) through the WiFi module.
  * @note   Establishes a TCP connection to a server, sends a http GET request, saves the response to return it
  * and at the and closes the TCP connection.
  * @retval The response of the http GET request.
  */
char * wifi_click_fetch_data() {

  // This is the HTTP GET request to send
  char *get_request = "GET /fetch-latest-data.php HTTP/1.1\r\nHost: 192.168.97.221\r\n\r\n";

	char AT_CIPSTART[]="AT+CIPSTART=\"TCP\",\"192.168.97.221\",80\r\n"; // Add ",7200" for TCP keep alive, but didn't work for me...
	char AT_CIPCLOSE[]="AT+CIPCLOSE\r\n"; // The TCP connection "close" string

  // Establish a TCP connection (I couldn't make keeplive work. so sad :(
  uart_transmit_with_echo(huart1, huart2, AT_CIPSTART);

	// Let's prepare a string with the length of the data to be expected by the WiFi click
	char AT_CIPSEND_MSG[20];
	sprintf(AT_CIPSEND_MSG, "AT+CIPSEND=%d\r\n", strlen(get_request));
	// Now let esp32 know how much data to expect
  uart_transmit_with_echo(huart1, huart2, AT_CIPSEND_MSG);
	// Send the data (=GET request) via the WiFi click
  char buf[1024] = {0};
  char *data = uart_transmit_with_echo(huart1, huart2, get_request);
  strcpy(buf, data);
  char *ret = buf;
	// Finally close the TCP connection
  uart_transmit_with_echo(huart1, huart2, AT_CIPCLOSE);
  // Return the response
  return ret;
}

/**
  * @brief  Sends string variables via one UART and echo the responses on another uart (usually serial terminal)
  * @note   Establishes a TCP connection to a server, sends a http GET request, saves the response to return it
  * and at the and closes the TCP connection.
  * @param  huart_tx : UART to send the data to (should be the UART used by the WiFI click board)
  * @param  huart_echo : UART on where to echo the response (should probably be the serial terminal)
  * @retval The response of the huart_tx.
  */
char * uart_transmit_with_echo(UART_HandleTypeDef huart_tx, UART_HandleTypeDef huart_echo, char *tx_data) {
  char RX_Buffer[1024] = {0};
  HAL_UART_Transmit(&huart_tx, (uint8_t*)tx_data, strlen(tx_data), HAL_MAX_DELAY);
  HAL_UART_Receive(&huart_tx,(uint8_t*)RX_Buffer,1024,5000);
  HAL_UART_Transmit(&huart2, (uint8_t*)RX_Buffer, strlen(RX_Buffer), HAL_MAX_DELAY);
  char *ret = RX_Buffer;
  return ret;
}

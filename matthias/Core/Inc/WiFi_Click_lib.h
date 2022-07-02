/*
 * WiFi_Click_lib.h
 *
 *  Created on: May 16, 2022
 *      Author: patrick
 */

#ifndef INC_WIFI_CLICK_LIB_H_
#define INC_WIFI_CLICK_LIB_H_

void wifi_click_init();
char * wifi_click_fetch_data();

//HAL_StatusTypeDef uart_transmit(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size, uint32_t Timeout);

char * uart_transmit_with_echo(UART_HandleTypeDef huart_tx, UART_HandleTypeDef huart_echo, char *tx_data);



#endif /* INC_WIFI_CLICK_LIB_H_ */

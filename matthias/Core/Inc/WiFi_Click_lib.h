/**
  ******************************************************************************
  * @file           : matrix_funktion.h
  * @author         : Matthias Kurz
  * @brief          : Header file with various function definitions used for the
  *                   ESP32 wifi click board
  ******************************************************************************
  */

void wifi_click_init();
char * wifi_click_fetch_data();
char * uart_transmit_with_echo(UART_HandleTypeDef huart_tx, UART_HandleTypeDef huart_echo, char *tx_data);

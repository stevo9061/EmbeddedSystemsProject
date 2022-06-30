/*
 * h4lib.h
 *
 *  Created on: 26 Jan 2022
 *      Author: sbitt
 */


#ifndef H4LIB_H
#define H4LIB_H

// Use 8-bit device address for I2C address from the heartrate 4 sensor, the address is out from the manual max30101.
#define HEARTRATE4_ADDR 0x57
//static const uint8_t HEARTRATE4_ADDR = 0x57 << 1;

#define BUFFERSIZE 10


#include <string.h>
#include <stdio.h>
#include "main.h"
#include <stdbool.h>
#include <stdlib.h>

#endif

void checkInput(I2C_HandleTypeDef handleI2C, UART_HandleTypeDef handleUART, uint8_t ringBuffer[], uint8_t size);
void hr4_test_partID(I2C_HandleTypeDef handleI2C, UART_HandleTypeDef handleUART); //Is no longer used in the program, was created for test purposes only.
void hr4_get_chipTemp(I2C_HandleTypeDef handleI2C, UART_HandleTypeDef handleUART);

uint8_t hr4_read_reg ( uint8_t reg, I2C_HandleTypeDef handleI2C);
void hr4_read_reg_multi ( uint8_t reg, uint8_t *buffer, uint8_t count, I2C_HandleTypeDef handleI2C);
void hr4_write_reg( uint8_t reg, uint8_t reg_val, I2C_HandleTypeDef handleI2C);
void hr4_set_registers(I2C_HandleTypeDef handleI2C);
int8_t hr4_set_fifo_sample_averaging( uint8_t sample_number, I2C_HandleTypeDef handleI2C);
void hr4_fifo_rollover_enable( bool enable, I2C_HandleTypeDef handleI2C);
void hr4_proximity_int_enable( bool enable, I2C_HandleTypeDef handleI2C);
int8_t hr4_set_mode( uint8_t mode, I2C_HandleTypeDef handleI2C);
int8_t hr4_set_spo2_sample_rate( uint8_t rate, I2C_HandleTypeDef handleI2C);
void hr4_new_fifo_data_int_enable( bool enable, I2C_HandleTypeDef handleI2C);
void hr4_fifo_full_int_enable( bool enable, I2C_HandleTypeDef handleI2C);
void hr4_set_red_led_pulse_amplitude( uint8_t amplitude, I2C_HandleTypeDef handleI2C );
void hr4_set_ir_led_pulse_amplitude( uint8_t amplitude, I2C_HandleTypeDef handleI2C );
void hr4_set_green_led_pulse_amplitude( uint8_t amplitude, I2C_HandleTypeDef handleI2C );
void hr4_set_proximity_led_pulse_amplitude( uint8_t amplitude, I2C_HandleTypeDef handleI2C );
void hr4_set_proximity_threshold( uint8_t threshold, I2C_HandleTypeDef handleI2C );
uint8_t hr4_is_new_fifo_data_ready(I2C_HandleTypeDef handleI2C);
uint32_t hr4_read_red(I2C_HandleTypeDef handleI2C);
uint8_t hr4_get_write_pointer( I2C_HandleTypeDef handleI2C );
uint8_t hr4_get_read_pointer( I2C_HandleTypeDef handleI2C );




















/*
 * matrix_funktion.h
 *
 *  Created on: Jan 26, 2022
 *      Author: fatoumeh
 */

#ifndef INC_MATRIX_FUNKTION_H_
#define INC_MATRIX_FUNKTION_H_

#include "main.h"
#include "stdbool.h"



void write_byte(uint8_t byte);
void write_register (uint8_t address, uint8_t data);
void init_8x8leds(void);
void brightness_control (uint8_t brightness_intensity);

#endif /* INC_MATRIX_FUNKTION_H_ */

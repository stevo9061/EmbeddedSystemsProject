/**
  ******************************************************************************
  * @file           : matrix_funktion.h
  * @author         : Matthias Kurz
  * @brief          : Header file with various includes and function definitions
  *                   used for the 8x8 click board
  ******************************************************************************
  */

#include "main.h"
#include "stdbool.h"

void write_byte(uint8_t byte);
void write_register (uint8_t address, uint8_t data);
void init_8x8leds(void);
void brightness_control (uint8_t brightness_intensity);

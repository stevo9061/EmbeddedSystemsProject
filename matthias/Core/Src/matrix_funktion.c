/**
  ******************************************************************************
  * @file           : main.c
  * @author         : Matthias Kurz
  * @brief          : Functions used to initialize and control the 8x8 click
  *                   board.
  ******************************************************************************
  */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "matrix_funktion.h"
#include "main.h"

typedef uint8_t byte;
SPI_HandleTypeDef hspi3;

/**
  * @brief  Used to set bytes in the 8x8 click board registers
  * @note   Using a variable to write MSB, data shifted to the left in the shiftregister
  * clk is only working active when cs is low (byte << 1: transmit every data bit! D0-D15)
  * @param  byte : byte two write
  * @retval None
  */
void write_byte(uint8_t byte)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 1); 					    // sets clk pin high
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, byte&0x80);			// writes MSB to the data pin
  byte = byte<<1;  											                // shift the data to the left
  HAL_SPI_Transmit(&hspi3, &byte, 1, HAL_MAX_DELAY);		// transmits my data
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 0);  					  // pulls clk low
}

/**
  * @brief  Sends adress and data to the 8x8 click module
  * @note   CS needs to be set low at start of the function, pulled back high after
  * operation finishes, CS needs to be pulled
  * @param  address: address to write to
  * @param  data: data to write
  * @retval None
  */
void write_register (uint8_t address, uint8_t data)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 0);  // sets CS pin low
  write_byte(address);										  // writes address
  write_byte(data);											    // writes data
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 1);	// pulls CS back high
}

/**
  * @brief  Initialize the 8x8 click module (power, scan limit, brightness, decode, test mode)
  * @note   Sets up the 8x8 click board with predefined functionalities
  * @retval None
  */
void init_8x8leds(void)
{
	 write_register (0x0c,		0x00);							//  0 = initial power down
	 write_register (0x0c, 		0x01);							//  1 = activate 8x8
	 write_register (0x0b, 		0x07);							//  scan limit register sets how many digits are displayed
	 write_register (0x0a, 		0x07);							//  chooses brightness intensity (0x00 - 0xf)
	 write_register (0x09, 		0x00);							//  no BCD encoding of digits, we paint what we want ;)
	 write_register (0x0f, 		0x01);							//  tests display: all leds full (useful for debugging)
	 write_register (0x0f, 		0x00);							//  disable testing display
}

/**
  * @brief  Sets the brightness of the 8x8 LEDs thru the bits in according register
  * @note   Helper function to controle the brightness of the 8x8 click board
  * @param  brightness_intensity: The brightness to set, from 0 (low) to 15 (high)
  * @retval None
  */
void brightness_control (uint8_t brightness_intensity)
{
  switch(brightness_intensity)
  {
    case 0:
      write_register(0x0a,     0x00);
      break;
    case 1:
      write_register(0x0a,     0x01);
      break;
    case 2:
      write_register(0x0a,     0x02);
      break;
    case 3:
      write_register(0x0a,     0x03);
      break;
    case 4:
      write_register(0x0a,     0x04);
      break;
    case 5:
      write_register(0x0a,     0x05);
      break;
    case 6:
      write_register(0x0a,     0x06);
      break;
    case 7:
      write_register(0x0a,     0x07);
      break;
    case 8:
      write_register(0x0a,     0x08);
      break;
    case 9:
      write_register(0x0a,     0x09);
      break;
    case 10:
      write_register(0x0a,     0x0a);
      break;
    case 11:
      write_register(0x0a,     0x0B);
      break;
    case 12:
      write_register(0x0a,     0x0C);
      break;
    case 13:
      write_register(0x0a,     0x0D);
      break;
    case 14:
      write_register(0x0a,     0x0E);
      break;
    case 15:
      write_register(0x0a,     0x0F);
      break;
    default:
      break;
  }
}

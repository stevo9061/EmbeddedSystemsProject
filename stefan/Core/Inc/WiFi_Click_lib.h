/*
 * WiFi_Click_lib.h
 *
 *  Created on: July 03, 2022
 *      Author: Stefan Bittgen
 */

#ifndef INC_WIFI_CLICK_LIB_H_
#define INC_WIFI_CLICK_LIB_H_

/**
@brief Initialiaze WiFi
@retval none
*/
void wifi_click_init();


/**
@brief We send here the POST-Request to our webserver
@param chipTemp: Chip Temperature from the module
@param heartRate: Pulse from the module
@retval none
*/
void wifi_click_send_test(int chipTemp, int heartRate);

#endif /* INC_WIFI_CLICK_LIB_H_ */

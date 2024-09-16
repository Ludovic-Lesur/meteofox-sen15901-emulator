/*
 * gpio_mapping.h
 *
 *  Created on: 08 sep. 2024
 *      Author: Ludo
 */

#ifndef __GPIO_MAPPING_H__
#define __GPIO_MAPPING_H__

#include "gpio.h"

/*** MAPPING global variables ***/

// Wind speed emulation.
extern const GPIO_pin_t 	GPIO_WIND_SPEED;
// Wind direction emulation.
extern const GPIO_pin_t 	GPIO_WIND_DIRECTION_N;
extern const GPIO_pin_t 	GPIO_WIND_DIRECTION_NE;
extern const GPIO_pin_t 	GPIO_WIND_DIRECTION_E;
extern const GPIO_pin_t 	GPIO_WIND_DIRECTION_SE;
extern const GPIO_pin_t 	GPIO_WIND_DIRECTION_S;
extern const GPIO_pin_t 	GPIO_WIND_DIRECTION_SW;
extern const GPIO_pin_t 	GPIO_WIND_DIRECTION_W;
extern const GPIO_pin_t 	GPIO_WIND_DIRECTION_NW;
// Rain gauge emulation.
extern const GPIO_pin_t 	GPIO_RAINFALL;
// LED.
extern const GPIO_pin_t		GPIO_LED;

#endif /* __GPIO_MAPPING_H__ */

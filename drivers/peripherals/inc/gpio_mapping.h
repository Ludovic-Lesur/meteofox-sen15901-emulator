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
extern const GPIO_pin_t 	GPIO_WIND_DIRECTION_MUX_INHIBIT;
extern const GPIO_pin_t 	GPIO_WIND_DIRECTION_MUX_A;
extern const GPIO_pin_t 	GPIO_WIND_DIRECTION_MUX_B;
extern const GPIO_pin_t 	GPIO_WIND_DIRECTION_MUX_C;
extern const GPIO_pin_t 	GPIO_WIND_DIRECTION_MUX_D;
// Rain gauge emulation.
extern const GPIO_pin_t 	GPIO_RAINFALL;
// LED.
extern const GPIO_pin_t		GPIO_LED;

#endif /* __GPIO_MAPPING_H__ */

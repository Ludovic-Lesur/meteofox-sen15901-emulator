/*
 * gpio_mapping.c
 *
 *  Created on: 08 sep. 2024
 *      Author: Ludo
 */

#include "gpio_mapping.h"

#include "gpio.h"
#include "gpio_reg.h"

/*** MAPPING global variables ***/

// Wind speed emulation.
const GPIO_pin_t GPIO_WIND_SPEED =					(GPIO_pin_t) {GPIOB, 1, 4, 4};
// Wind direction emulation.
const GPIO_pin_t GPIO_WIND_DIRECTION_MUX_INHIBIT =	(GPIO_pin_t) {GPIOA, 0, 5, 0};
const GPIO_pin_t GPIO_WIND_DIRECTION_MUX_A =		(GPIO_pin_t) {GPIOA, 0, 6, 0};
const GPIO_pin_t GPIO_WIND_DIRECTION_MUX_B =		(GPIO_pin_t) {GPIOA, 0, 7, 0};
const GPIO_pin_t GPIO_WIND_DIRECTION_MUX_C =		(GPIO_pin_t) {GPIOB, 1, 0, 0};
const GPIO_pin_t GPIO_WIND_DIRECTION_MUX_D =		(GPIO_pin_t) {GPIOB, 1, 1, 0};
// Rain gauge emulation.
const GPIO_pin_t GPIO_RAINFALL =					(GPIO_pin_t) {GPIOB, 1, 6, 5};
// Debug LED.
const GPIO_pin_t GPIO_LED = 						(GPIO_pin_t) {GPIOB, 1, 3, 0};

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
const GPIO_pin_t GPIO_WIND_DIRECTION_N =			(GPIO_pin_t) {GPIOA, 0, 0, 0};
const GPIO_pin_t GPIO_WIND_DIRECTION_NE =			(GPIO_pin_t) {GPIOA, 0, 1, 0};
const GPIO_pin_t GPIO_WIND_DIRECTION_E =			(GPIO_pin_t) {GPIOA, 0, 9, 0};
const GPIO_pin_t GPIO_WIND_DIRECTION_SE =			(GPIO_pin_t) {GPIOA, 0, 3, 0};
const GPIO_pin_t GPIO_WIND_DIRECTION_S =			(GPIO_pin_t) {GPIOA, 0, 4, 0};
const GPIO_pin_t GPIO_WIND_DIRECTION_SW =			(GPIO_pin_t) {GPIOA, 0, 5, 0};
const GPIO_pin_t GPIO_WIND_DIRECTION_W =			(GPIO_pin_t) {GPIOA, 0, 6, 0};
const GPIO_pin_t GPIO_WIND_DIRECTION_NW =			(GPIO_pin_t) {GPIOA, 0, 7, 0};
// Rain gauge emulation.
const GPIO_pin_t GPIO_RAINFALL =					(GPIO_pin_t) {GPIOB, 1, 6, 5};
// Debug LED.
const GPIO_pin_t GPIO_LED = 						(GPIO_pin_t) {GPIOB, 1, 3, 0};

/*
 * gpio_mapping.h
 *
 *  Created on: 08 sep. 2024
 *      Author: Ludo
 */

#ifndef __GPIO_MAPPING_H__
#define __GPIO_MAPPING_H__

#include "gpio.h"
#include "usart.h"

/*** GPIO MAPPING global variables ***/

// TCXO power control.
extern const GPIO_pin_t GPIO_TCXO_POWER_ENABLE;
// Wind speed emulation.
extern const GPIO_pin_t GPIO_WIND_SPEED;
// Wind direction emulation.
extern const GPIO_pin_t GPIO_WIND_DIRECTION_N;
extern const GPIO_pin_t GPIO_WIND_DIRECTION_NE;
extern const GPIO_pin_t GPIO_WIND_DIRECTION_E;
extern const GPIO_pin_t GPIO_WIND_DIRECTION_SE;
extern const GPIO_pin_t GPIO_WIND_DIRECTION_S;
extern const GPIO_pin_t GPIO_WIND_DIRECTION_SW;
extern const GPIO_pin_t GPIO_WIND_DIRECTION_W;
extern const GPIO_pin_t GPIO_WIND_DIRECTION_NW;
// Rain gauge emulation.
extern const GPIO_pin_t GPIO_RAINFALL;
// DUT synchronization.
extern const GPIO_pin_t GPIO_DUT_SYNCHRO;
// Log.
extern const USART_gpio_t GPIO_LOG_USART;
// LEDs.
extern const GPIO_pin_t GPIO_LED_SIMULATION;
extern const GPIO_pin_t GPIO_LED_SYNCHRO;

#endif /* __GPIO_MAPPING_H__ */

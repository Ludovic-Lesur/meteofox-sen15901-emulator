/*
 * gpio_mapping.c
 *
 *  Created on: 08 sep. 2024
 *      Author: Ludo
 */

#include "gpio_mapping.h"

#include "gpio.h"
#include "gpio_registers.h"

/*** GPIO MAPPING local global variables ***/

static const GPIO_pin_t GPIO_USART2_TX = { GPIOA, 0, 2, 4 };
static const GPIO_pin_t GPIO_USART2_RX = { GPIOA, 0, 15, 4 };

/*** GPIO MAPPING global variables ***/

// TCXO power control.
const GPIO_pin_t GPIO_TCXO_POWER_ENABLE = { GPIOA, 0, 1, 0 };
// Wind speed emulation.
const GPIO_pin_t GPIO_WIND_SPEED = { GPIOB, 1, 4, 4 };
// Wind direction emulation.
const GPIO_pin_t GPIO_WIND_DIRECTION_N = { GPIOA, 0, 8, 0 };
const GPIO_pin_t GPIO_WIND_DIRECTION_NE = { GPIOA, 0, 3, 0 };
const GPIO_pin_t GPIO_WIND_DIRECTION_E = { GPIOA, 0, 4, 0 };
const GPIO_pin_t GPIO_WIND_DIRECTION_SE = { GPIOA, 0, 5, 0 };
const GPIO_pin_t GPIO_WIND_DIRECTION_S = { GPIOA, 0, 6, 0 };
const GPIO_pin_t GPIO_WIND_DIRECTION_SW = { GPIOA, 0, 7, 0 };
const GPIO_pin_t GPIO_WIND_DIRECTION_W = { GPIOB, 1, 0, 0 };
const GPIO_pin_t GPIO_WIND_DIRECTION_NW = { GPIOB, 1, 1, 0 };
// Rain gauge emulation.
const GPIO_pin_t GPIO_RAINFALL = { GPIOB, 1, 6, 5 };
// DUT synchronization.
const GPIO_pin_t GPIO_DUT_SYNCHRO = { GPIOB, 1, 7, 0 };
// Log.
const USART_gpio_t GPIO_LOG_USART = { &GPIO_USART2_TX, &GPIO_USART2_RX };
// LEDs.
const GPIO_pin_t GPIO_LED_SIMULATION = { GPIOB, 1, 3, 0 };
const GPIO_pin_t GPIO_LED_SYNCHRO = { GPIOA, 0, 12, 0 };

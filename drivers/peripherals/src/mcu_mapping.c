/*
 * mcu_mapping.c
 *
 *  Created on: 08 sep. 2024
 *      Author: Ludo
 */

#include "mcu_mapping.h"

#include "gpio.h"
#include "gpio_registers.h"
#include "tim.h"
#include "usart.h"

/*** MCU MAPPING local global variables ***/

// Wind speed emulation.
static const GPIO_pin_t GPIO_WIND_SPEED = { GPIOB, 1, 4, 4 };
// Rain gauge emulation.
static const GPIO_pin_t GPIO_RAINFALL = { GPIOB, 1, 6, 5 };
// Timer channels.
static const TIM_channel_gpio_t TIM_CHANNEL_GPIO_WIND_SPEED = { TIM_CHANNEL_WIND_SPEED, &GPIO_WIND_SPEED, TIM_POLARITY_ACTIVE_HIGH };
static const TIM_channel_gpio_t TIM_CHANNEL_GPIO_RAINFALL = { TIM_CHANNEL_RAINFALL, &GPIO_RAINFALL, TIM_POLARITY_ACTIVE_HIGH };
// Timer pins list.
static const TIM_channel_gpio_t* const TIM_CHANNEL_GPIO_LIST_WIND_SPEED[TIM_CHANNEL_INDEX_WIND_SPEED_LAST] = { &TIM_CHANNEL_GPIO_WIND_SPEED };
static const TIM_channel_gpio_t* const TIM_CHANNEL_GPIO_LIST_RAINFALL[TIM_CHANNEL_INDEX_RAINFALL_LAST] = { &TIM_CHANNEL_GPIO_RAINFALL };
// USART2.
static const GPIO_pin_t GPIO_USART2_TX = { GPIOA, 0, 9, 4 };
static const GPIO_pin_t GPIO_USART2_RX = { GPIOA, 0, 10, 4 };

/*** MCU MAPPING global variables ***/

// TCXO power control.
const GPIO_pin_t GPIO_TCXO_POWER_ENABLE = { GPIOA, 0, 2, 0 };
// Wind speed emulation.
const TIM_gpio_t TIM_GPIO_WIND_SPEED = { (const TIM_channel_gpio_t**) &TIM_CHANNEL_GPIO_LIST_WIND_SPEED, TIM_CHANNEL_INDEX_WIND_SPEED_LAST };
// Wind direction emulation.
const GPIO_pin_t GPIO_WIND_DIRECTION_N =  { GPIOA, 0, 3, 0 };
const GPIO_pin_t GPIO_WIND_DIRECTION_NE = { GPIOA, 0, 4, 0 };
const GPIO_pin_t GPIO_WIND_DIRECTION_E =  { GPIOA, 0, 5, 0 };
const GPIO_pin_t GPIO_WIND_DIRECTION_SE = { GPIOA, 0, 6, 0 };
const GPIO_pin_t GPIO_WIND_DIRECTION_S =  { GPIOA, 0, 7, 0 };
const GPIO_pin_t GPIO_WIND_DIRECTION_SW = { GPIOB, 1, 0, 0 };
const GPIO_pin_t GPIO_WIND_DIRECTION_W =  { GPIOB, 1, 1, 0 };
const GPIO_pin_t GPIO_WIND_DIRECTION_NW = { GPIOB, 1, 2, 0 };
// Rain gauge emulation.
const TIM_gpio_t TIM_GPIO_RAINFALL = { (const TIM_channel_gpio_t**) &TIM_CHANNEL_GPIO_LIST_RAINFALL, TIM_CHANNEL_INDEX_RAINFALL_LAST };
// DUT synchronization.
const GPIO_pin_t GPIO_DUT_SYNCHRO = { GPIOB, 1, 7, 0 };
// Log.
const USART_gpio_t USART_GPIO_LOG = { &GPIO_USART2_TX, &GPIO_USART2_RX };
// LEDs.
const GPIO_pin_t GPIO_LED_RUN = { GPIOB, 1, 3, 0 };
const GPIO_pin_t GPIO_LED_SYNCHRO = { GPIOA, 0, 15, 0 };
const GPIO_pin_t GPIO_LED_FAULT = { GPIOA, 0, 12, 0 };

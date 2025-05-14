/*
 * mcu_mapping.h
 *
 *  Created on: 08 sep. 2024
 *      Author: Ludo
 */

#ifndef __MCU_MAPPING_H__
#define __MCU_MAPPING_H__

#include "gpio.h"
#include "sen15901_emulator_flags.h"
#include "tim.h"
#include "usart.h"

/*** MCU MAPPING macros ***/

#define TIM_INSTANCE_SIMULATION     TIM_INSTANCE_TIM2

#define TIM_INSTANCE_WIND           TIM_INSTANCE_TIM22
#define TIM_CHANNEL_WIND_SPEED      TIM_CHANNEL_1
#define TIM_CHANNEL_WIND_DIRECTION  TIM_CHANNEL_2

#define TIM_INSTANCE_RAINFALL       TIM_INSTANCE_TIM21
#define TIM_CHANNEL_RAINFALL        TIM_CHANNEL_1

#define USART_INSTANCE_LOG          USART_INSTANCE_USART2

/*** MCU MAPPING structures ***/

/*!******************************************************************
 * \enum TIM_channel_index_wind_t
 * \brief TIM wind channels index.
 *******************************************************************/
typedef enum {
    TIM_CHANNEL_INDEX_WIND_SPEED = 0,
#ifdef SEN15901_MODE_ULTIMETER
    TIM_CHANNEL_INDEX_WIND_DIRECTION,
#endif
    TIM_CHANNEL_INDEX_WIND_LAST
} TIM_channel_index_wind_t;

/*!******************************************************************
 * \enum TIM_channel_index_rainfall_t
 * \brief TIM rainfall channels index.
 *******************************************************************/
typedef enum {
    TIM_CHANNEL_INDEX_RAINFALL = 0,
    TIM_CHANNEL_INDEX_RAINFALL_LAST
} TIM_channel_index_rainfall_t;

/*** MCU MAPPING global variables ***/

// TCXO power control.
extern const GPIO_pin_t GPIO_TCXO_POWER_ENABLE;
// Wind speed emulation.
extern const TIM_gpio_t TIM_GPIO_WIND;
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
extern const TIM_gpio_t TIM_GPIO_RAINFALL;
// DUT synchronization.
extern const GPIO_pin_t GPIO_DUT_SYNCHRO;
// Log.
extern const GPIO_pin_t GPIO_USB_DETECT;
extern const USART_gpio_t USART_GPIO_LOG;
// LEDs.
extern const GPIO_pin_t GPIO_LED_RUN;
extern const GPIO_pin_t GPIO_LED_SYNCHRO;
extern const GPIO_pin_t GPIO_LED_FAULT;

#endif /* __MCU_MAPPING_H__ */

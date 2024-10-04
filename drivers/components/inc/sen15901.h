/*
 * sen15901.h
 *
 *  Created on: 15 sep. 2024
 *      Author: Ludo
 */

#ifndef __SEN15901_H__
#define __SEN15901_H__

#include "tim.h"
#include "types.h"

/*** SEN15901 macros ***/

#define SEN15901_WIND_DIRECTION_RESISTOR_NUMBER     8
#define SEN15901_WIND_DIRECTION_NUMBER              (SEN15901_WIND_DIRECTION_RESISTOR_NUMBER << 1)

/*** SEN15901 structures ***/

/*!******************************************************************
 * \enum SEN15901_status_t
 * \brief SEN15901 emulator driver error codes.
 *******************************************************************/
typedef enum {
    // Driver errors.
    SEN15901_SUCCESS = 0,
    SEN15901_ERROR_WIND_DIRECTION,
    // Low level driver errors.
    SEN15901_ERROR_BASE_TIM_WIND_SPEED = 0x0100,
    SEN15901_ERROR_BASE_TIM_RAINFALL = (SEN15901_ERROR_BASE_TIM_WIND_SPEED + TIM_ERROR_BASE_LAST),
    // Last base value.
    SEN15901_ERROR_BASE_LAST = (SEN15901_ERROR_BASE_TIM_RAINFALL + TIM_ERROR_BASE_LAST)
} SEN15901_status_t;

/*** SEN15901 functions ***/

/*!******************************************************************
 * \fn SEN15901_status_t SEN15901_init(void)
 * \brief Init SEN15901 emulator driver.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
SEN15901_status_t SEN15901_init(void);

/*!******************************************************************
 * \fn SEN15901_status_t SEN15901_de_init(void)
 * \brief Release SEN15901 emulator driver.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
SEN15901_status_t SEN15901_de_init(void);

/*!******************************************************************
 * \fn SEN15901_status_t SEN15901_set_wind_speed(uint32_t wind_speed_kmh)
 * \brief Set SEN15901 test waveform to simulate a wind speed.
 * \param[in]  	wind_speed_kmh: Wind speed to simulate in km/h.
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
SEN15901_status_t SEN15901_set_wind_speed(uint32_t wind_speed_kmh);

/*!******************************************************************
 * \fn SEN15901_status_t SEN15901_set_wind_direction(uint32_t wind_direction_degrees)
 * \brief Set SEN15901 test waveform to simulate a wind direction.
 * \param[in]  	wind_direction_degrees: Wind direction to simulate in degrees.
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
SEN15901_status_t SEN15901_set_wind_direction(uint32_t wind_direction_degrees);

/*!******************************************************************
 * \fn void SEN15901_reset_rainfall_mm(void)
 * \brief Reset SEN15901 rainfall count.
 * \param[in]  	none
 * \param[out] 	none
 * \retval		none
 *******************************************************************/
void SEN15901_reset_rainfall_mm(void);

/*!******************************************************************
 * \fn SEN15901_status_t SEN15901_add_rainfall_mm(uint32_t rainfall_mm)
 * \brief Set SEN15901 test waveform to simulate a rainfall.
 * \param[in]  	rainfall_mm: Rainfall to add in mm.
 * \param[out] 	none
 * \retval		Function execution status.
 *******************************************************************/
SEN15901_status_t SEN15901_add_rainfall_mm(uint32_t rainfall_mm);

/*******************************************************************/
#define SEN15901_exit_error(base) { ERROR_check_exit(sen15901_status, SEN15901_SUCCESS, base) }

/*******************************************************************/
#define SEN15901_stack_error(base) { ERROR_check_stack(sen15901_status, SEN15901_SUCCESS, base) }

/*******************************************************************/
#define SEN15901_stack_exit_error(base, code) { ERROR_check_stack_exit(sen15901_status, SEN15901_SUCCESS, base, code) }

#endif /* __SEN15901_H__ */

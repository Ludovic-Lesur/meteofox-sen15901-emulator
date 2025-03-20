/*
 * simulation.h
 *
 *  Created on: 18 sep. 2024
 *      Author: Ludo
 */

#ifndef __SIMULATION_H__
#define __SIMULATION_H__

#include "error.h"
#include "sen15901.h"
#include "tim.h"
#include "types.h"
#include "usart.h"

/*** SIMULATION structures ***/

/*!******************************************************************
 * \enum SIMULATION_status_t_status_t
 * \brief Simulation driver error codes.
 *******************************************************************/
typedef enum {
    // Driver errors.
    SIMULATION_SUCCESS = 0,
    // Low level driver errors.
    SIMULATION_ERROR_BASE_WAVEFORM_TIMER = ERROR_BASE_STEP,
    SIMULATION_ERROR_BASE_LOG_USART = (SIMULATION_ERROR_BASE_WAVEFORM_TIMER + TIM_ERROR_BASE_LAST),
    SIMULATION_ERROR_BASE_SEN15901 = (SIMULATION_ERROR_BASE_LOG_USART + USART_ERROR_BASE_LAST),
    // Last base value.
    SIMULATION_ERROR_BASE_LAST = (SIMULATION_ERROR_BASE_SEN15901 + SEN15901_ERROR_BASE_LAST)
} SIMULATION_status_t;

/*** SIMULATION functions ***/

/*!******************************************************************
 * \fn SIMULATION_status_t SIMULATION_init(void)
 * \brief Init simulation driver.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
SIMULATION_status_t SIMULATION_init(void);

/*!******************************************************************
 * \fn SIMULATION_status_t SIMULATION_de_init(void)
 * \brief Release simulation driver.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
SIMULATION_status_t SIMULATION_de_init(void);

/*!******************************************************************
 * \fn SIMULATION_status_t SIMULATION_init(void)
 * \brief Start SEN15901 simulation.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
SIMULATION_status_t SIMULATION_start(void);

/*!******************************************************************
 * \fn SIMULATION_status_t SIMULATION_stop(void)
 * \brief Stop SEN15901 simulation.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
SIMULATION_status_t SIMULATION_stop(void);

/*!******************************************************************
 * \fn SIMULATION_status_t SIMULATION_process(void)
 * \brief Process SEN15901 simulation.
 * \param[in]   none
 * \param[out]  none
 * \retval      Function execution status.
 *******************************************************************/
SIMULATION_status_t SIMULATION_process(void);

/*******************************************************************/
#define SIMULATION_exit_error(base) { ERROR_check_exit(simulation_status, SIMULATION_SUCCESS, base) }

/*******************************************************************/
#define SIMULATION_stack_error(base) { ERROR_check_stack(simulation_status, SIMULATION_SUCCESS, base) }

/*******************************************************************/
#define SIMULATION_stack_exit_error(base, code) { ERROR_check_stack_exit(simulation_status, SIMULATION_SUCCESS, base, code) }

#endif /* __SIMULATION_H__ */

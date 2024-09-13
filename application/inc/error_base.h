/*
 * error_base.h
 *
 *  Created on: 12 mar. 2022
 *      Author: Ludo
 */

#ifndef __ERROR_BASE_H__
#define __ERROR_BASE_H__

// Peripherals.
#include "flash.h"
#include "iwdg.h"
#include "lptim.h"
#include "nvm.h"
#include "rcc.h"
#include "rtc.h"
#include "tim.h"
// Utils.
#include "math.h"
#include "parser.h"
#include "string.h"

/*** ERROR structures ***/

/*!******************************************************************
 * \enum ERROR_base_t
 * \brief Board error bases.
 *******************************************************************/
typedef enum {
	SUCCESS = 0,
	// Peripherals.
	ERROR_BASE_FLASH = 0x0100,
	ERROR_BASE_IWDG = (ERROR_BASE_FLASH + FLASH_ERROR_BASE_LAST),
	ERROR_BASE_LPTIM = (ERROR_BASE_IWDG + IWDG_ERROR_BASE_LAST),
	ERROR_BASE_RCC = (ERROR_BASE_LPTIM + LPTIM_ERROR_BASE_LAST),
	ERROR_BASE_RTC = (ERROR_BASE_RCC + RCC_ERROR_BASE_LAST),
	ERROR_BASE_TIM_WIND_SPEED = (ERROR_BASE_RTC + RTC_ERROR_BASE_LAST),
	ERROR_BASE_TIM_RAIN_GAUGE = (ERROR_BASE_TIM_WIND_SPEED + TIM_ERROR_BASE_LAST),
	// Utils.
	ERROR_BASE_MATH = (ERROR_BASE_TIM_RAIN_GAUGE + TIM_ERROR_BASE_LAST),
	ERROR_BASE_PARSER = (ERROR_BASE_MATH + MATH_ERROR_BASE_LAST),
	ERROR_BASE_STRING = (ERROR_BASE_PARSER + PARSER_ERROR_BASE_LAST),
	// Last base value.
	ERROR_BASE_LAST = (ERROR_BASE_STRING + STRING_ERROR_BASE_LAST)
} ERROR_base_t;

#endif /* __ERROR_BASE_H__ */

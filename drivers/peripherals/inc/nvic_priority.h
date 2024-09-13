/*
 * nvic_priority.h
 *
 *  Created on: 08 sep. 2024
 *      Author: Ludo
 */

#ifndef __NVIC_PRIORITY_H__
#define __NVIC_PRIORITY_H__

/*!******************************************************************
 * \enum NVIC_priority_list_t
 * \brief NVIC interrupt priorities list.
 *******************************************************************/
typedef enum {
	// Common.
	NVIC_PRIORITY_CLOCK = 0,
	NVIC_PRIORITY_CLOCK_CALIBRATION = 1,
	NVIC_PRIORITY_DELAY = 2,
	NVIC_PRIORITY_RTC = 3,
} NVIC_priority_list_t;

#endif /* __NVIC_PRIORITY_H__ */

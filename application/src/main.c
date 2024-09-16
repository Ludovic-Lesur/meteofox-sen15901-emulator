/*
 * main.c
 *
 *  Created on: 08 sep. 2024
 *      Author: Ludo
 */

// Peripherals.
#include "error.h"
#include "exti.h"
#include "gpio.h"
#include "gpio_mapping.h"
#include "iwdg.h"
#include "lptim.h"
#include "nvic_priority.h"
#include "pwr.h"
#include "rcc.h"
#include "rtc.h"
// Components.
#include "sen15901.h"
// Applicative.
#include "error_base.h"
#include "mode.h"

/*** MAIN local structures ***/

/*******************************************************************/
typedef struct {
	volatile uint8_t rtc_flag;
} SEN15901_EMULATOR_context_t;

/*** MAIN local global variables ***/

static SEN15901_EMULATOR_context_t sen15901_emulator_ctx;

/*** MAIN local functions ***/

/*******************************************************************/
void _SEN15901_EMULATOR_tick_second_callback(void) {
	// Set flag.
	sen15901_emulator_ctx.rtc_flag = 1;
}

/*******************************************************************/
void _SEN15901_EMULATOR_init_context(void) {
	// Init context.
	sen15901_emulator_ctx.rtc_flag = 0;
}

/*******************************************************************/
void _SEN15901_EMULATOR_init_hw(void) {
	// Local variables.
	RCC_status_t rcc_status = RCC_SUCCESS;
	RTC_status_t rtc_status = RTC_SUCCESS;
	SEN15901_status_t sen15901_status = SEN15901_SUCCESS;
#ifndef DEBUG
	IWDG_status_t iwdg_status = IWDG_SUCCESS;
#endif
	// Init error stack
	ERROR_stack_init();
	// Init memory.
	NVIC_init();
	// Init power module and clock tree.
	PWR_init();
	rcc_status = RCC_init(NVIC_PRIORITY_CLOCK);
	RCC_stack_error(ERROR_BASE_RCC);
	// Init GPIOs.
	GPIO_init();
	EXTI_init();
#ifndef DEBUG
	// Start independent watchdog.
	iwdg_status = IWDG_init();
	IWDG_stack_error(ERROR_BASE_IWDG);
#endif
	// High speed oscillator.
	rcc_status = RCC_switch_to_hsi();
	RCC_stack_error(ERROR_BASE_RCC);
	// Calibrate internal clocks.
	rcc_status = RCC_calibrate_internal_clocks(NVIC_PRIORITY_CLOCK_CALIBRATION);
	RCC_stack_error(ERROR_BASE_RCC);
	// Init RTC.
	rtc_status = RTC_init(&_SEN15901_EMULATOR_tick_second_callback, NVIC_PRIORITY_RTC);
	RTC_stack_error(ERROR_BASE_RTC);
	// Init delay timer.
	LPTIM_init(NVIC_PRIORITY_DELAY);
	// Init LED pin.
	GPIO_configure(&GPIO_LED, GPIO_MODE_OUTPUT, GPIO_TYPE_PUSH_PULL, GPIO_SPEED_LOW, GPIO_PULL_NONE);
	// Init SEN15901 emulator.
	sen15901_status = SEN15901_init();
	SEN15901_stack_error(ERROR_BASE_SEN15901);
}

/*** MAIN function ***/

/*******************************************************************/
int main(void) {
	// Init board.
	_SEN15901_EMULATOR_init_context();
	_SEN15901_EMULATOR_init_hw();
	// Static simulation.
	SEN15901_set_wind_speed(50);
	SEN15901_set_wind_direction(180);
	SEN15901_add_rainfall_mm(5);
	// Main loop.
	while (1) {
		// Enter sleep mode.
		IWDG_reload();
		PWR_enter_sleep_mode();
		IWDG_reload();
		// Check flag.
		if (sen15901_emulator_ctx.rtc_flag != 0) {
			// Clear flag.
			sen15901_emulator_ctx.rtc_flag = 0;
			GPIO_toggle(&GPIO_LED);
		}
	}
	return 0;
}

/*
 * main.c
 *
 *  Created on: 08 sep. 2024
 *      Author: Ludo
 */

#include "error.h"
#include "error_base.h"
#include "exti.h"
#include "gpio.h"
#include "gpio_mapping.h"
#include "iwdg.h"
#include "lptim.h"
#include "mode.h"
#include "nvic_priority.h"
#include "pwr.h"
#include "rcc.h"
#include "rtc.h"

#include "rcc_reg.h"

/*** MAIN local structures ***/

/*******************************************************************/
typedef struct {
	volatile uint8_t rtc_flag;
	volatile uint8_t timer_flag;
	uint32_t timer_count;
} SEN15901_EMULATOR_context_t;

/*** MAIN local global variables ***/

static SEN15901_EMULATOR_context_t sen15901_emulator_ctx;

static const GPIO_pin_t GPIO_DEBUG = (GPIO_pin_t) {GPIOB, 1, 0, 0};
static const GPIO_pin_t GPIO_MCO = (GPIO_pin_t) {GPIOA, 0, 8, 0};

/*** MAIN local functions ***/

/*******************************************************************/
void _SEN15901_EMULATOR_tick_second_callback(void) {
	// Set flag.
	sen15901_emulator_ctx.rtc_flag = 1;
}

void _SEN15901_timer_standard_callback(void) {
	// Set flag.
	sen15901_emulator_ctx.timer_flag = 1;
}

/*******************************************************************/
void _SEN15901_EMULATOR_init_context(void) {
	// Init context.
	sen15901_emulator_ctx.rtc_flag = 0;
	sen15901_emulator_ctx.timer_flag = 0;
	sen15901_emulator_ctx.timer_count = 0;
}

/*******************************************************************/
void _SEN15901_EMULATOR_init_hw(void) {
	// Local variables.
	RCC_status_t rcc_status = RCC_SUCCESS;
	RTC_status_t rtc_status = RTC_SUCCESS;
	TIM_status_t tim_status = TIM_SUCCESS;
#ifndef DEBUG
	IWDG_status_t iwdg_status = IWDG_SUCCESS;
#endif
	TIM_gpio_t tim_gpio;
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
	// MCO check;
	RCC_set_mco(RCC_CLOCK_LSE, RCC_MCO_PRESCALER_16, &GPIO_MCO);
	// Init RTC.
	rtc_status = RTC_init(&_SEN15901_EMULATOR_tick_second_callback, NVIC_PRIORITY_RTC);
	RTC_stack_error(ERROR_BASE_RTC);
	// Init delay timer.
	LPTIM_init(NVIC_PRIORITY_DELAY);
	// Init LED pin.
	GPIO_configure(&GPIO_LED, GPIO_MODE_OUTPUT, GPIO_TYPE_PUSH_PULL, GPIO_SPEED_LOW, GPIO_PULL_NONE);
	GPIO_configure(&GPIO_DEBUG, GPIO_MODE_OUTPUT, GPIO_TYPE_PUSH_PULL, GPIO_SPEED_LOW, GPIO_PULL_NONE);
	// Init wind speed emulator timer.
	tim_gpio.channel = TIM_CHANNEL_1;
	tim_gpio.gpio = &GPIO_WIND_SPEED;
	tim_gpio.polarity = TIM_POLARITY_ACTIVE_HIGH;
	tim_status = TIM_PWM_init(TIM_INSTANCE_TIM22, &tim_gpio, 1);
	TIM_stack_error(ERROR_BASE_TIM_WIND_SPEED);
	// Init rain gauge emulator timer.
	tim_gpio.channel = TIM_CHANNEL_1;
	tim_gpio.gpio = &GPIO_RAINFALL;
	tim_gpio.polarity = TIM_POLARITY_ACTIVE_LOW;
	tim_status = TIM_OPM_init(TIM_INSTANCE_TIM21, &tim_gpio, 1);
	TIM_stack_error(ERROR_BASE_TIM_RAIN_GAUGE);
	// Init standard timer.
	tim_status = TIM_STD_init(TIM_INSTANCE_TIM2, 0);
	TIM_stack_error(ERROR_BASE_TIM_RAIN_GAUGE);
}

/*** MAIN function ***/

/*******************************************************************/
int main(void) {
	// Init board.
	_SEN15901_EMULATOR_init_context();
	_SEN15901_EMULATOR_init_hw();
	// Local variables.
	TIM_status_t tim_status = TIM_SUCCESS;
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
			sen15901_emulator_ctx.timer_count = 0;
			GPIO_toggle(&GPIO_LED);
			// Update test waveforms.
			tim_status = TIM_PWM_set_waveform(TIM_INSTANCE_TIM22, TIM_CHANNEL_1, 100, 50);
			TIM_stack_error(ERROR_BASE_TIM_WIND_SPEED);
			tim_status = TIM_OPM_make_pulse(TIM_INSTANCE_TIM21, TIM_CHANNEL_1, 40000000, 10000000);
			TIM_stack_error(ERROR_BASE_TIM_RAIN_GAUGE);
			tim_status = TIM_STD_stop(TIM_INSTANCE_TIM2);
			TIM_stack_error(ERROR_BASE_TIM_WIND_SPEED);
			tim_status = TIM_STD_start(TIM_INSTANCE_TIM2, 10000000, &_SEN15901_timer_standard_callback);
			TIM_stack_error(ERROR_BASE_TIM_WIND_SPEED);
		}
		if (sen15901_emulator_ctx.timer_flag != 0) {
			// Clear flag.
			sen15901_emulator_ctx.timer_flag = 0;
			sen15901_emulator_ctx.timer_count++;
			GPIO_toggle(&GPIO_DEBUG);
			// Update test waveforms.
			if (sen15901_emulator_ctx.timer_count == 5) {
				tim_status = TIM_STD_stop(TIM_INSTANCE_TIM2);
				TIM_stack_error(ERROR_BASE_TIM_WIND_SPEED);
				tim_status = TIM_STD_start(TIM_INSTANCE_TIM2, 30000000, &_SEN15901_timer_standard_callback);
				TIM_stack_error(ERROR_BASE_TIM_WIND_SPEED);
			}
		}
	}
	return 0;
}

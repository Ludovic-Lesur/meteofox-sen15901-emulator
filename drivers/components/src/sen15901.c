/*
 * sen15901.c
 *
 *  Created on: 15 sep. 2024
 *      Author: Ludo
 */

#include "sen15901.h"

#include "error.h"
#include "error_base.h"
#include "gpio.h"
#include "gpio_mapping.h"
#include "tim.h"
#include "types.h"

/*** SEN15901 local macros ***/

#define SEN15901_WIND_SPEED_TIMER_INSTANCE				TIM_INSTANCE_TIM22
#define SEN15901_WIND_SPEED_TIMER_CHANNEL				TIM_CHANNEL_1

#define SEN15901_WIND_SPEED_1HZ_TO_MH					2400

#define SEN15901_WIND_DIRECTION_RESISTOR_NUMBER			8
#define SEN15901_WIND_DIRECTION_RESISTOR_RANGE_DEGREES	34

#define SEN15901_RAINFALL_TIMER_INSTANCE				TIM_INSTANCE_TIM21
#define SEN15901_RAINFALL_TIMER_CHANNEL					TIM_CHANNEL_1
#define SEN15901_RAINFALL_PULSE_DURATION_MS				100

#define SEN15901_RAINFALL_EDGE_TO_UM					279

#define SEN15901_FULL_CIRCLE_DEGREES					360

/*** SEN15901 local structures ***/

/*******************************************************************/
typedef struct {
	const GPIO_pin_t* gpio;
	const uint32_t angle;
	uint32_t angle_min;
	uint32_t angle_max;
} SEN15901_wind_direction_resistor_t;

/*** SEN15901 local global variables ***/

static const TIM_gpio_t TIM_GPIO_WIND_SPEED = {
	.channel = SEN15901_WIND_SPEED_TIMER_CHANNEL,
	.gpio = &GPIO_WIND_SPEED,
	.polarity = TIM_POLARITY_ACTIVE_HIGH
};
static const TIM_gpio_t TIM_GPIO_RAINFALL = {
	.channel = SEN15901_RAINFALL_TIMER_CHANNEL,
	.gpio = &GPIO_RAINFALL,
	.polarity = TIM_POLARITY_ACTIVE_HIGH
};
static SEN15901_wind_direction_resistor_t SEN159001_WIND_DIRECTION_RESISTOR[SEN15901_WIND_DIRECTION_RESISTOR_NUMBER] = {
	{&GPIO_WIND_DIRECTION_N,  0,   0, 0},
	{&GPIO_WIND_DIRECTION_NE, 45,  0, 0},
	{&GPIO_WIND_DIRECTION_E,  90,  0, 0},
	{&GPIO_WIND_DIRECTION_SE, 135, 0, 0},
	{&GPIO_WIND_DIRECTION_S,  180, 0, 0},
	{&GPIO_WIND_DIRECTION_SW, 225, 0, 0},
	{&GPIO_WIND_DIRECTION_W,  270, 0, 0},
	{&GPIO_WIND_DIRECTION_NW, 315, 0, 0},
};

/*** SEN15901 functions ***/

/*** SEN15901 functions ***/

/*******************************************************************/
SEN15901_status_t SEN15901_init(void) {
	// Local variables.
	SEN15901_status_t status = SEN15901_SUCCESS;
	TIM_status_t tim_status = TIM_SUCCESS;
	int32_t tmp_s32 = 0;
	uint8_t idx = 0;
	// Init wind vane resistors.
	for (idx=0 ; idx<SEN15901_WIND_DIRECTION_RESISTOR_NUMBER ; idx++) {
		GPIO_write(SEN159001_WIND_DIRECTION_RESISTOR[idx].gpio,  1);
		GPIO_configure(SEN159001_WIND_DIRECTION_RESISTOR[idx].gpio, GPIO_MODE_OUTPUT, GPIO_TYPE_OPEN_DRAIN, GPIO_SPEED_LOW, GPIO_PULL_NONE);
		// Compute minimum angle.
		tmp_s32 = SEN159001_WIND_DIRECTION_RESISTOR[idx].angle - SEN15901_WIND_DIRECTION_RESISTOR_RANGE_DEGREES;
		SEN159001_WIND_DIRECTION_RESISTOR[idx].angle_min = (tmp_s32 < 0) ? ((uint32_t) (tmp_s32 + SEN15901_FULL_CIRCLE_DEGREES)) : ((uint32_t) tmp_s32);
		// Compute maximum angle.
		tmp_s32 = SEN159001_WIND_DIRECTION_RESISTOR[idx].angle + SEN15901_WIND_DIRECTION_RESISTOR_RANGE_DEGREES;
		SEN159001_WIND_DIRECTION_RESISTOR[idx].angle_max = (tmp_s32 > SEN15901_FULL_CIRCLE_DEGREES) ? ((uint32_t) (tmp_s32 - SEN15901_FULL_CIRCLE_DEGREES)) : ((uint32_t) tmp_s32);
	}
	// Init PWM timer for wind speed.
	tim_status = TIM_PWM_init(SEN15901_WIND_SPEED_TIMER_INSTANCE, (TIM_gpio_t*) &TIM_GPIO_WIND_SPEED, 1);
	TIM_exit_error(SEN15901_ERROR_BASE_TIM_WIND_SPEED);
	// Init OPM timer for rainfall.
	tim_status = TIM_OPM_init(SEN15901_RAINFALL_TIMER_INSTANCE, (TIM_gpio_t*) &TIM_GPIO_RAINFALL, 1);
	TIM_exit_error(SEN15901_ERROR_BASE_TIM_RAINFALL);
errors:
	return status;
}

/*******************************************************************/
SEN15901_status_t SEN15901_de_init(void) {
	// Local variables.
	SEN15901_status_t status = SEN15901_SUCCESS;
	TIM_status_t tim_status = TIM_SUCCESS;
	// Release PWM timer for wind speed.
	tim_status = TIM_PWM_de_init(SEN15901_WIND_SPEED_TIMER_INSTANCE, (TIM_gpio_t*) &TIM_GPIO_WIND_SPEED, 1);
	TIM_exit_error(SEN15901_ERROR_BASE_TIM_WIND_SPEED);
	// Release OPM timer for rainfall.
	tim_status = TIM_OPM_de_init(SEN15901_RAINFALL_TIMER_INSTANCE, (TIM_gpio_t*) &TIM_GPIO_RAINFALL, 1);
	TIM_exit_error(SEN15901_ERROR_BASE_TIM_RAINFALL);
errors:
	return status;
}

/*******************************************************************/
SEN15901_status_t SEN15901_set_wind_speed(uint32_t wind_speed_kmh) {
	// Local variables.
	SEN15901_status_t status = SEN15901_SUCCESS;
	TIM_status_t tim_status = TIM_SUCCESS;
	uint32_t pwm_frequency_hz = 0;
	uint8_t pwm_duty_cycle_percent = 50;
	// Convert speed to PWM frequency.
	pwm_frequency_hz = (1000 * wind_speed_kmh) / (SEN15901_WIND_SPEED_1HZ_TO_MH);
	// Check frequency.
	if (pwm_frequency_hz == 0) {
		// Disable output signal.
		pwm_frequency_hz = 1;
		pwm_duty_cycle_percent = 0;
	}
	tim_status = TIM_PWM_set_waveform(SEN15901_WIND_SPEED_TIMER_INSTANCE, SEN15901_WIND_SPEED_TIMER_CHANNEL, pwm_frequency_hz, pwm_duty_cycle_percent);
	TIM_exit_error(SEN15901_ERROR_BASE_TIM_RAINFALL);
errors:
	return status;
}

/*******************************************************************/
SEN15901_status_t SEN15901_set_wind_direction(uint32_t wind_direction_degrees) {
	// Local variables.
	SEN15901_status_t status = SEN15901_SUCCESS;
	uint8_t idx = 0;
	uint32_t angle_min = 0;
	uint32_t angle_max = 0;
	uint8_t state = 0;
	// Check parameter.
	if (wind_direction_degrees >= SEN15901_FULL_CIRCLE_DEGREES) {
		status = SEN15901_ERROR_WIND_DIRECTION;
		goto errors;
	}
	// Activate required resistors.
	for (idx=0 ; idx<SEN15901_WIND_DIRECTION_RESISTOR_NUMBER ; idx++) {
		// Update variables.
		angle_min = SEN159001_WIND_DIRECTION_RESISTOR[idx].angle_min;
		angle_max = SEN159001_WIND_DIRECTION_RESISTOR[idx].angle_max;
		// Check formula to apply.
		if (angle_min < angle_max) {
			// And condition.
			state = (wind_direction_degrees > angle_min) && (wind_direction_degrees < angle_max) ? 0 : 1;
		}
		else {
			// Or condition.
			state = (wind_direction_degrees > angle_min) || (wind_direction_degrees < angle_max) ? 0 : 1;
		}
		// Write pin.
		GPIO_write(SEN159001_WIND_DIRECTION_RESISTOR[idx].gpio, state);
	}
errors:
	return status;
}

/*******************************************************************/
SEN15901_status_t SEN15901_add_rainfall_mm(uint32_t rainfall_mm) {
	// Local variables.
	SEN15901_status_t status = SEN15901_SUCCESS;
	TIM_status_t tim_status = TIM_SUCCESS;
	uint32_t pulse_duration_ns = (SEN15901_RAINFALL_PULSE_DURATION_MS * 1000000);
	uint32_t number_of_pulses = (((rainfall_mm * 1000) / (SEN15901_RAINFALL_EDGE_TO_UM)) + 1);
	uint8_t pulse_is_done = 0;
	uint32_t idx = 0;
	// Simulate rain gauge.
	for (idx=0 ; idx<number_of_pulses ; idx++) {
		// Make pulse.
		tim_status = TIM_OPM_make_pulse(SEN15901_RAINFALL_TIMER_INSTANCE, SEN15901_RAINFALL_TIMER_CHANNEL, pulse_duration_ns, pulse_duration_ns);
		TIM_exit_error(SEN15901_ERROR_BASE_TIM_RAINFALL);
		// Wait for pulse to be completed.
		do {
			TIM_OPM_get_pulse_status(SEN15901_RAINFALL_TIMER_INSTANCE, SEN15901_RAINFALL_TIMER_CHANNEL, &pulse_is_done);
		}
		while (pulse_is_done == 0);
	}
errors:
	return status;
}

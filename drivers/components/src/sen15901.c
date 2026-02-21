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
#include "mcu_mapping.h"
#include "sen15901_emulator_flags.h"
#include "tim.h"
#include "types.h"

/*** SEN15901 local macros ***/

#ifdef SEN15901_MODE_ULTIMETER
#define SEN15901_WIND_SPEED_1HZ_TO_MH                   5400
#else
#define SEN15901_WIND_SPEED_1HZ_TO_MH                   2400
#endif

#define SEN15901_PWM_FREQUENCY_MIN_MHZ                  (1000000 / SEN15901_WIND_SPEED_1HZ_TO_MH)

#define SEN15901_WIND_DIRECTION_RESISTOR_NUMBER         8
#define SEN15901_WIND_DIRECTION_RESISTOR_RANGE_DEGREES  34

#define SEN15901_RAINFALL_PULSE_DURATION_MS             200

/*** SEN15901 local structures ***/

#ifndef SEN15901_MODE_ULTIMETER
/*******************************************************************/
typedef struct {
    const GPIO_pin_t* const gpio;
    const uint32_t angle;
    uint32_t angle_min;
    uint32_t angle_max;
} SEN15901_wind_direction_resistor_t;
#endif

#ifdef SEN15901_MODE_ULTIMETER
/*******************************************************************/
typedef struct {
    uint32_t speed_pwm_frequency_mhz;
    uint8_t speed_pwm_duty_cycle;
} SEN15901_context_t;
#endif

/*** SEN15901 local global variables ***/

#ifndef SEN15901_MODE_ULTIMETER
static SEN15901_wind_direction_resistor_t SEN159001_WIND_DIRECTION_RESISTOR[SEN15901_WIND_DIRECTION_RESISTOR_NUMBER] = {
    { &GPIO_WIND_DIRECTION_N, 0, 0, 0 },
    { &GPIO_WIND_DIRECTION_NE, 45, 0, 0 },
    { &GPIO_WIND_DIRECTION_E, 90, 0, 0 },
    { &GPIO_WIND_DIRECTION_SE, 135, 0, 0 },
    { &GPIO_WIND_DIRECTION_S, 180, 0, 0 },
    { &GPIO_WIND_DIRECTION_SW, 225, 0, 0 },
    { &GPIO_WIND_DIRECTION_W, 270, 0, 0 },
    { &GPIO_WIND_DIRECTION_NW, 315, 0, 0 },
};
#endif

#ifdef SEN15901_MODE_ULTIMETER
static SEN15901_context_t sen15901_ctx = {
    .speed_pwm_frequency_mhz = SEN15901_PWM_FREQUENCY_MIN_MHZ,
    .speed_pwm_duty_cycle = 0,
};
#endif

/*** SEN15901 functions ***/

/*******************************************************************/
SEN15901_status_t SEN15901_init(void) {
    // Local variables.
    SEN15901_status_t status = SEN15901_SUCCESS;
    TIM_status_t tim_status = TIM_SUCCESS;
#ifndef SEN15901_MODE_ULTIMETER
    int32_t tmp_s32 = 0;
    uint8_t idx = 0;
#endif
#ifdef SEN15901_MODE_ULTIMETER
    // Init context.
    sen15901_ctx.speed_pwm_frequency_mhz = SEN15901_PWM_FREQUENCY_MIN_MHZ;
    sen15901_ctx.speed_pwm_duty_cycle = 0;
#endif
#ifndef SEN15901_MODE_ULTIMETER
    // Init wind vane resistors.
    for (idx = 0; idx < SEN15901_WIND_DIRECTION_RESISTOR_NUMBER; idx++) {
        GPIO_configure(SEN159001_WIND_DIRECTION_RESISTOR[idx].gpio, GPIO_MODE_OUTPUT, GPIO_TYPE_PUSH_PULL, GPIO_SPEED_LOW, GPIO_PULL_NONE);
        // Compute minimum angle.
        tmp_s32 = SEN159001_WIND_DIRECTION_RESISTOR[idx].angle - SEN15901_WIND_DIRECTION_RESISTOR_RANGE_DEGREES;
        SEN159001_WIND_DIRECTION_RESISTOR[idx].angle_min = (tmp_s32 < 0) ? ((uint32_t) (tmp_s32 + MATH_2_PI_DEGREES)) : ((uint32_t) tmp_s32);
        // Compute maximum angle.
        tmp_s32 = SEN159001_WIND_DIRECTION_RESISTOR[idx].angle + SEN15901_WIND_DIRECTION_RESISTOR_RANGE_DEGREES;
        SEN159001_WIND_DIRECTION_RESISTOR[idx].angle_max = (tmp_s32 > MATH_2_PI_DEGREES) ? ((uint32_t) (tmp_s32 - MATH_2_PI_DEGREES)) : ((uint32_t) tmp_s32);
    }
#endif
    // Init PWM timer for wind speed.
    tim_status = TIM_PWM_init(TIM_INSTANCE_WIND, (TIM_gpio_t*) &TIM_GPIO_WIND);
    TIM_exit_error(SEN15901_ERROR_BASE_TIM_WIND);
    // Init OPM timer for rainfall.
    tim_status = TIM_OPM_init(TIM_INSTANCE_RAINFALL, (TIM_gpio_t*) &TIM_GPIO_RAINFALL);
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
    tim_status = TIM_PWM_de_init(TIM_INSTANCE_WIND, (TIM_gpio_t*) &TIM_GPIO_WIND);
    TIM_stack_error(ERROR_BASE_SEN15901 + SEN15901_ERROR_BASE_TIM_WIND);
    // Release OPM timer for rainfall.
    tim_status = TIM_OPM_de_init(TIM_INSTANCE_RAINFALL, (TIM_gpio_t*) &TIM_GPIO_RAINFALL);
    TIM_stack_error(ERROR_BASE_SEN15901 + SEN15901_ERROR_BASE_TIM_RAINFALL);
    return status;
}

/*******************************************************************/
SEN15901_status_t SEN15901_set_wind_speed(uint32_t wind_speed_kmh) {
    // Local variables.
    SEN15901_status_t status = SEN15901_SUCCESS;
    TIM_status_t tim_status = TIM_SUCCESS;
    uint32_t pwm_frequency_mhz = 0;
    uint8_t pwm_duty_cycle_percent = 50;
    // Convert speed to PWM frequency.
    pwm_frequency_mhz = (MATH_POWER_10[6] * wind_speed_kmh) / (SEN15901_WIND_SPEED_1HZ_TO_MH);
    // Check frequency.
    if (pwm_frequency_mhz == 0) {
        // Disable output signal.
        pwm_frequency_mhz = SEN15901_PWM_FREQUENCY_MIN_MHZ;
        pwm_duty_cycle_percent = 0;
    }
#ifdef SEN15901_MODE_ULTIMETER
    sen15901_ctx.speed_pwm_frequency_mhz = pwm_frequency_mhz;
    sen15901_ctx.speed_pwm_duty_cycle = pwm_duty_cycle_percent;
#endif
    tim_status = TIM_PWM_set_waveform(TIM_INSTANCE_WIND, TIM_CHANNEL_WIND_SPEED, pwm_frequency_mhz, pwm_duty_cycle_percent);
    TIM_exit_error(SEN15901_ERROR_BASE_TIM_WIND);
errors:
    return status;
}

/*******************************************************************/
SEN15901_status_t SEN15901_set_wind_direction(uint32_t wind_direction_degrees) {
    // Local variables.
    SEN15901_status_t status = SEN15901_SUCCESS;
#ifdef SEN15901_MODE_ULTIMETER
    TIM_status_t tim_status = TIM_SUCCESS;
    uint8_t wind_direction_percent = 0;
    uint8_t pwm_duty_cycle_percent = 0;
#else
    uint8_t idx = 0;
    uint32_t angle_min = 0;
    uint32_t angle_max = 0;
    uint8_t state = 0;
#endif
    // Check parameter.
    if (wind_direction_degrees >= MATH_2_PI_DEGREES) {
        status = SEN15901_ERROR_WIND_DIRECTION;
        goto errors;
    }
#ifdef SEN15901_MODE_ULTIMETER
    // Convert degrees to percent.
    wind_direction_percent = ((wind_direction_degrees * MATH_PERCENT_MAX) / (MATH_2_PI_DEGREES));
    // Compute direction duty cycle.
    if (sen15901_ctx.speed_pwm_duty_cycle > 0) {
        pwm_duty_cycle_percent = ((sen15901_ctx.speed_pwm_duty_cycle + MATH_PERCENT_MAX - wind_direction_percent) % MATH_PERCENT_MAX);
        // Avoid 0 case.
        if (pwm_duty_cycle_percent == 0) {
            pwm_duty_cycle_percent = 1;
        }
    }
    // Set duty cycle.
    tim_status = TIM_PWM_set_waveform(TIM_INSTANCE_WIND, TIM_CHANNEL_WIND_DIRECTION, sen15901_ctx.speed_pwm_frequency_mhz, pwm_duty_cycle_percent);
    TIM_exit_error(SEN15901_ERROR_BASE_TIM_WIND);
#else
    // Activate required resistors.
    for (idx = 0; idx < SEN15901_WIND_DIRECTION_RESISTOR_NUMBER; idx++) {
        // Update variables.
        angle_min = SEN159001_WIND_DIRECTION_RESISTOR[idx].angle_min;
        angle_max = SEN159001_WIND_DIRECTION_RESISTOR[idx].angle_max;
        // Check formula to apply.
        if (angle_min < angle_max) {
            // And condition.
            state = (wind_direction_degrees > angle_min) && (wind_direction_degrees < angle_max) ? 1 : 0;
        }
        else {
            // Or condition.
            state = (wind_direction_degrees > angle_min) || (wind_direction_degrees < angle_max) ? 1 : 0;
        }
        // Write pin.
        GPIO_write(SEN159001_WIND_DIRECTION_RESISTOR[idx].gpio, state);
    }
#endif
errors:
    return status;
}

/*******************************************************************/
SEN15901_status_t SEN15901_make_rainfall_interrupt(void) {
    // Local variables.
    SEN15901_status_t status = SEN15901_SUCCESS;
    TIM_status_t tim_status = TIM_SUCCESS;
    uint32_t pulse_duration_ns = (SEN15901_RAINFALL_PULSE_DURATION_MS * MATH_POWER_10[6]);
    // Make pulse.
    tim_status = TIM_OPM_make_pulse(TIM_INSTANCE_RAINFALL, (0b1 << TIM_CHANNEL_RAINFALL), pulse_duration_ns, pulse_duration_ns, 0);
    TIM_exit_error(SEN15901_ERROR_BASE_TIM_RAINFALL);
errors:
    return status;
}

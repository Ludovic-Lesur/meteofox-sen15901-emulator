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
#include "usart.h"
// Middleware.
#include "simulation.h"
// Applicative.
#include "error_base.h"
#include "sen15901_emulator_flags.h"

/*** MAIN local functions ***/

/*******************************************************************/
static void _SEN15901_EMULATOR_init_hw(void) {
    // Local variables.
    RCC_status_t rcc_status = RCC_SUCCESS;
    RTC_status_t rtc_status = RTC_SUCCESS;
    SIMULATION_status_t simulation_status = SIMULATION_SUCCESS;
#ifndef SEN15901_EMULATOR_MODE_DEBUG
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
#ifndef SEN15901_EMULATOR_MODE_DEBUG
    // Start independent watchdog.
    iwdg_status = IWDG_init();
    IWDG_stack_error(ERROR_BASE_IWDG);
#endif
    // Init TCXO control pin.
    GPIO_configure(&GPIO_TCXO_POWER_ENABLE, GPIO_MODE_OUTPUT, GPIO_TYPE_PUSH_PULL, GPIO_SPEED_LOW, GPIO_PULL_NONE);
    GPIO_write(&GPIO_TCXO_POWER_ENABLE, 1);
    // High speed oscillator.
    rcc_status = RCC_switch_to_hse(RCC_HSE_MODE_BYPASS);
    RCC_stack_error(ERROR_BASE_RCC);
    // Calibrate internal clocks.
    rcc_status = RCC_calibrate_internal_clocks(NVIC_PRIORITY_CLOCK_CALIBRATION);
    RCC_stack_error(ERROR_BASE_RCC);
    // Init RTC.
    rtc_status = RTC_init(NULL, NVIC_PRIORITY_RTC);
    RTC_stack_error(ERROR_BASE_RTC);
    // Init delay timer.
    LPTIM_init(NVIC_PRIORITY_DELAY);
    // Init simulation.
    simulation_status = SIMULATION_init();
    SIMULATION_stack_error(ERROR_BASE_SIMULATION);
}

/*** MAIN function ***/

/*******************************************************************/
int main(void) {
    // Local variables.
    SIMULATION_status_t simulation_status = SIMULATION_SUCCESS;
    // Init board.
    _SEN15901_EMULATOR_init_hw();
    // Start simulation.
    simulation_status = SIMULATION_start();
    SIMULATION_stack_error(ERROR_BASE_SIMULATION);
    // Main loop.
    while (1) {
        // Enter sleep mode.
        IWDG_reload();
        PWR_enter_sleep_mode();
        IWDG_reload();
        // Run simulation.
        simulation_status = SIMULATION_process();
        SIMULATION_stack_error(ERROR_BASE_SIMULATION);
    }
    return 0;
}

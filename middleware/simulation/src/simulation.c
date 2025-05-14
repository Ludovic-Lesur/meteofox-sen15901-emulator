/*
 * simulation.c
 *
 *  Created on: 18 sep. 2024
 *      Author: Ludo
 */

#include "simulation.h"

#include "error.h"
#include "error_base.h"
#include "exti.h"
#include "nvic_priority.h"
#include "gpio.h"
#include "mcu_mapping.h"
#include "rtc.h"
#include "sen15901.h"
#include "sen15901_emulator_flags.h"
#include "terminal.h"
#include "tim.h"
#include "types.h"
#include "version.h"

/*** SIMULATION local macros ***/

#define SIMULATION_WIND_SPEED_KMH_MAX           120

#define SIMULATION_RAINFALL_MM_MAX              100
#define SIMULATION_RAINFALL_TIMESTAMP_MS        1800000

#ifdef SEN15901_MODE_ULTIMETER
#define SIMULATION_WAVEFORM_TIMER_PERIOD_MS     6001
#else
#define SIMULATION_WAVEFORM_TIMER_PERIOD_MS     3001
#endif

#define SIMULATION_DUT_SYNCHRO_IRQ_FILTER_MS    60000

#define SIMULATION_LOG_BAUD_RATE                9600
#define SIMULATION_LOG_LINE_END                 "\r\n"

#define SIMULATION_FAULT_TIME_THRESHOLD_MS      3900000

/*** SIMULATION local structures ***/

/*******************************************************************/
typedef union {
    struct {
        unsigned wind_speed_down :1;
        unsigned timer :1;
        unsigned synchro :1;
        unsigned first_synchro :1;
        unsigned synchro_irq_enable :1;
    } __attribute__((scalar_storage_order("big-endian")))__attribute__((packed));
    uint8_t all;
} SIMULATION_flags_t;

/*******************************************************************/
typedef struct {
    // State machine.
    volatile SIMULATION_flags_t flags;
    volatile uint32_t time_ms;
    // Amplitudes.
    uint32_t wind_speed_peak_kmh;
    uint32_t wind_direction_table_index;
    uint32_t rainfall_peak_mm;
    // Values within period.
    uint32_t wind_speed_kmh;
    uint32_t rainfall_mm;
} SIMULATION_context_t;

/*** SIMULATION local global variables ***/

static const uint32_t SIMULATION_WIND_DIRECTION_TABLE[SEN15901_WIND_DIRECTION_NUMBER] = { 0, 22, 45, 67, 90, 112, 135, 157, 180, 202, 225, 247, 270, 292, 315, 337 };

static SIMULATION_context_t simulation_ctx = {
    .flags.all = 0,
    .time_ms = 0,
    .wind_speed_peak_kmh = 0,
    .wind_direction_table_index = (SEN15901_WIND_DIRECTION_NUMBER - 1),
    .rainfall_peak_mm = 0,
    .wind_speed_kmh = 0,
    .rainfall_mm = 0
};

/*** SIMULATION local functions ***/

/*******************************************************************/
static void _SIMULATION_dut_synchro_callback(void) {
    // Set flags.
    simulation_ctx.flags.first_synchro = 1;
    simulation_ctx.flags.synchro = simulation_ctx.flags.synchro_irq_enable;
    // Disable interrupt for debouncing.
    simulation_ctx.flags.synchro_irq_enable = 0;
}

/*******************************************************************/
static void _SIMULATION_timer_callback(void) {
    // Set flags.
    simulation_ctx.flags.timer = 1;
    simulation_ctx.time_ms += SIMULATION_WAVEFORM_TIMER_PERIOD_MS;
}

/*******************************************************************/
static void _SIMULATION_print_sw_version(void) {
    // Local variables.
    TERMINAL_status_t terminal_status = TERMINAL_SUCCESS;
    // Print string.
    terminal_status = TERMINAL_flush_tx_buffer(0);
    TERMINAL_stack_error(ERROR_BASE_TERMINAL);
    terminal_status = TERMINAL_tx_buffer_add_string(0, "Version=sw");
    TERMINAL_stack_error(ERROR_BASE_TERMINAL);
    terminal_status = TERMINAL_tx_buffer_add_integer(0, GIT_MAJOR_VERSION, STRING_FORMAT_DECIMAL, 0);
    TERMINAL_stack_error(ERROR_BASE_TERMINAL);
    terminal_status = TERMINAL_tx_buffer_add_string(0, ".");
    TERMINAL_stack_error(ERROR_BASE_TERMINAL);
    terminal_status = TERMINAL_tx_buffer_add_integer(0, GIT_MINOR_VERSION, STRING_FORMAT_DECIMAL, 0);
    TERMINAL_stack_error(ERROR_BASE_TERMINAL);
    terminal_status = TERMINAL_tx_buffer_add_string(0, ".");
    TERMINAL_stack_error(ERROR_BASE_TERMINAL);
    terminal_status = TERMINAL_tx_buffer_add_integer(0, GIT_COMMIT_INDEX, STRING_FORMAT_DECIMAL, 0);
    TERMINAL_stack_error(ERROR_BASE_TERMINAL);
    if (GIT_DIRTY_FLAG != 0) {
        terminal_status = TERMINAL_tx_buffer_add_string(0, ".dev");
        TERMINAL_stack_error(ERROR_BASE_TERMINAL);
    }
    terminal_status = TERMINAL_tx_buffer_add_string(0, SIMULATION_LOG_LINE_END);
    TERMINAL_stack_error(ERROR_BASE_TERMINAL);
    terminal_status = TERMINAL_send_tx_buffer(0);
    TERMINAL_stack_error(ERROR_BASE_TERMINAL);
}

/*******************************************************************/
static void _SIMULATION_print_string(char_t* str) {
    // Local variables.
    TERMINAL_status_t terminal_status = TERMINAL_SUCCESS;
    // Print string.
    terminal_status = TERMINAL_flush_tx_buffer(0);
    TERMINAL_stack_error(ERROR_BASE_TERMINAL);
    if (str != NULL) {
        terminal_status = TERMINAL_tx_buffer_add_string(0, str);
        TERMINAL_stack_error(ERROR_BASE_TERMINAL);
    }
    terminal_status = TERMINAL_tx_buffer_add_string(0, SIMULATION_LOG_LINE_END);
    TERMINAL_stack_error(ERROR_BASE_TERMINAL);
    terminal_status = TERMINAL_send_tx_buffer(0);
    TERMINAL_stack_error(ERROR_BASE_TERMINAL);
    return;
}

/*******************************************************************/
static void _SIMULATION_print_value(char_t* name, int32_t value, char_t* unit) {
    // Local variables.
    TERMINAL_status_t terminal_status = TERMINAL_SUCCESS;
    // Print value.
    terminal_status = TERMINAL_flush_tx_buffer(0);
    TERMINAL_stack_error(ERROR_BASE_TERMINAL);
    if (name != NULL) {
        terminal_status = TERMINAL_tx_buffer_add_string(0, name);
        TERMINAL_stack_error(ERROR_BASE_TERMINAL);
    }
    terminal_status = TERMINAL_tx_buffer_add_integer(0, value, STRING_FORMAT_DECIMAL, 0);
    TERMINAL_stack_error(ERROR_BASE_TERMINAL);
    if (unit != NULL) {
        terminal_status = TERMINAL_tx_buffer_add_string(0, unit);
        TERMINAL_stack_error(ERROR_BASE_TERMINAL);
    }
    terminal_status = TERMINAL_tx_buffer_add_string(0, SIMULATION_LOG_LINE_END);
    TERMINAL_stack_error(ERROR_BASE_TERMINAL);
    terminal_status = TERMINAL_send_tx_buffer(0);
    TERMINAL_stack_error(ERROR_BASE_TERMINAL);
    return;
}

/*** SIMULATION functions ***/

/*******************************************************************/
SIMULATION_status_t SIMULATION_init(void) {
    // Local variables.
    SIMULATION_status_t status = SIMULATION_SUCCESS;
    SEN15901_status_t sen15901_status = SEN15901_SUCCESS;
    TIM_status_t tim_status = TIM_SUCCESS;
    // Reset context.
    simulation_ctx.flags.all = 0;
    simulation_ctx.time_ms = 0;
    simulation_ctx.wind_speed_peak_kmh = 0;
    simulation_ctx.wind_direction_table_index = (SEN15901_WIND_DIRECTION_NUMBER - 1);
    simulation_ctx.rainfall_peak_mm = 0;
    simulation_ctx.wind_speed_kmh = 0;
    simulation_ctx.rainfall_mm = 0;
    // Init status LEDs.
    GPIO_configure(&GPIO_LED_RUN, GPIO_MODE_OUTPUT, GPIO_TYPE_PUSH_PULL, GPIO_SPEED_LOW, GPIO_PULL_NONE);
    GPIO_configure(&GPIO_LED_SYNCHRO, GPIO_MODE_OUTPUT, GPIO_TYPE_PUSH_PULL, GPIO_SPEED_LOW, GPIO_PULL_NONE);
    GPIO_configure(&GPIO_LED_FAULT, GPIO_MODE_OUTPUT, GPIO_TYPE_PUSH_PULL, GPIO_SPEED_LOW, GPIO_PULL_NONE);
    // Init synchronization signal.
    EXTI_configure_gpio(&GPIO_DUT_SYNCHRO, GPIO_PULL_NONE, EXTI_TRIGGER_RISING_EDGE, &_SIMULATION_dut_synchro_callback, NVIC_PRIORITY_DUT_SYNCHRONIZATION);
    // Init waveform timer.
    tim_status = TIM_STD_init(TIM_INSTANCE_SIMULATION, NVIC_PRIORITY_SIMULATION_WAVEFORM_TIMER);
    TIM_exit_error(SIMULATION_ERROR_BASE_WAVEFORM_TIMER);
    // Init SEN15901 emulator.
    sen15901_status = SEN15901_init();
    SEN15901_exit_error(SIMULATION_ERROR_BASE_SEN15901);
    // Init USB detect pin.
    GPIO_configure(&GPIO_USB_DETECT, GPIO_MODE_INPUT, GPIO_TYPE_PUSH_PULL, GPIO_SPEED_LOW, GPIO_PULL_NONE);
errors:
    return status;
}

/*******************************************************************/
SIMULATION_status_t SIMULATION_de_init(void) {
    // Local variables.
    SIMULATION_status_t status = SIMULATION_SUCCESS;
    SEN15901_status_t sen15901_status = SEN15901_SUCCESS;
    // Release synchronization signal.
    EXTI_release_gpio(&GPIO_DUT_SYNCHRO, GPIO_MODE_ANALOG);
    // Init SEN15901 emulator.
    sen15901_status = SEN15901_de_init();
    SEN15901_stack_error(ERROR_BASE_SIMULATION + SIMULATION_ERROR_BASE_SEN15901);
    // Release USB detect pin.
    GPIO_configure(&GPIO_USB_DETECT, GPIO_MODE_ANALOG, GPIO_TYPE_OPEN_DRAIN, GPIO_SPEED_LOW, GPIO_PULL_NONE);
    return status;
}

/*******************************************************************/
SIMULATION_status_t SIMULATION_start(void) {
    // Local variables.
    SIMULATION_status_t status = SIMULATION_SUCCESS;
    TIM_status_t tim_status = TIM_SUCCESS;
    // Enable synchronization interrupt.
    simulation_ctx.flags.synchro_irq_enable = 1;
    EXTI_enable_gpio_interrupt(&GPIO_DUT_SYNCHRO);
    // Reset time.
    simulation_ctx.time_ms = 0;
    // Start timer.
    tim_status = TIM_STD_start(TIM_INSTANCE_SIMULATION, SIMULATION_WAVEFORM_TIMER_PERIOD_MS, TIM_UNIT_MS, &_SIMULATION_timer_callback);
    TIM_exit_error(SIMULATION_ERROR_BASE_WAVEFORM_TIMER);
errors:
    return status;
}

/*******************************************************************/
SIMULATION_status_t SIMULATION_stop(void) {
    // Local variables.
    SIMULATION_status_t status = SIMULATION_SUCCESS;
    TIM_status_t tim_status = TIM_SUCCESS;
    // Enable synchronization interrupt.
    EXTI_disable_gpio_interrupt(&GPIO_DUT_SYNCHRO);
    simulation_ctx.flags.synchro_irq_enable = 0;
    // Stop timer.
    tim_status = TIM_STD_stop(TIM_INSTANCE_SIMULATION);
    TIM_exit_error(SIMULATION_ERROR_BASE_WAVEFORM_TIMER);
errors:
    return status;
}

/*******************************************************************/
SIMULATION_status_t SIMULATION_process(void) {
    // Local variables.
    SIMULATION_status_t status = SIMULATION_SUCCESS;
    SEN15901_status_t sen15901_status = SEN15901_SUCCESS;
    TERMINAL_status_t terminal_status = TERMINAL_SUCCESS;
    uint8_t synchro_event = 0;
    uint8_t log_enable = GPIO_read(&GPIO_USB_DETECT);
    // Check fault condition.
    GPIO_write(&GPIO_LED_FAULT, ((simulation_ctx.time_ms > SIMULATION_FAULT_TIME_THRESHOLD_MS) ? 1 : 0));
    // Do not start before first DUT synchronization.
    if (simulation_ctx.flags.first_synchro == 0) goto errors;
    // Check synchronization flag.
    if (simulation_ctx.flags.synchro != 0) {
        // Clear flag.
        simulation_ctx.flags.synchro = 0;
        synchro_event = 1;
        // Reset current values.
        simulation_ctx.time_ms = 0;
        simulation_ctx.wind_speed_kmh = 0;
        simulation_ctx.flags.wind_speed_down = 0;
        simulation_ctx.rainfall_mm = 0;
        SEN15901_reset_rainfall_mm();
        // Increment amplitudes.
        simulation_ctx.wind_speed_peak_kmh = (simulation_ctx.wind_speed_peak_kmh + 1) % (SIMULATION_WIND_SPEED_KMH_MAX + 1);
        simulation_ctx.wind_direction_table_index = (simulation_ctx.wind_direction_table_index + 1) % SEN15901_WIND_DIRECTION_NUMBER;
        simulation_ctx.rainfall_peak_mm = (simulation_ctx.rainfall_peak_mm + 1) % (SIMULATION_RAINFALL_MM_MAX + 1);
        // Turn LED on.
        GPIO_write(&GPIO_LED_SYNCHRO, 1);
    }
    // Manage synchronization interrupt.
    if (simulation_ctx.time_ms > SIMULATION_DUT_SYNCHRO_IRQ_FILTER_MS) {
        GPIO_write(&GPIO_LED_SYNCHRO, 0);
        simulation_ctx.flags.synchro_irq_enable = 1;
    }
    // Check timer flag.
    if (simulation_ctx.flags.timer != 0) {
        // Clear flag.
        simulation_ctx.flags.timer = 0;
        // Blink LED.
        GPIO_toggle(&GPIO_LED_RUN);
        // Wind speed.
        if (simulation_ctx.wind_speed_peak_kmh > 0) {
            if (simulation_ctx.wind_speed_kmh >= simulation_ctx.wind_speed_peak_kmh) {
                // Clamp speed and start ramp down.
                simulation_ctx.wind_speed_kmh = (simulation_ctx.wind_speed_peak_kmh - 1);
                simulation_ctx.flags.wind_speed_down = 1;
            }
            else {
                if (simulation_ctx.wind_speed_kmh == 0) {
                    // Clamp speed and start ramp up.
                    simulation_ctx.wind_speed_kmh = 1;
                    simulation_ctx.flags.wind_speed_down = 0;
                }
                else {
                    if (simulation_ctx.flags.wind_speed_down == 0) {
                        simulation_ctx.wind_speed_kmh++;
                    }
                    else {
                        simulation_ctx.wind_speed_kmh--;
                    }
                }
            }
        }
        else {
            simulation_ctx.wind_speed_kmh = 0;
        }
        sen15901_status = SEN15901_set_wind_speed(simulation_ctx.wind_speed_kmh);
        SEN15901_exit_error(SIMULATION_ERROR_BASE_SEN15901);
        // Wind direction.
        sen15901_status = SEN15901_set_wind_direction(SIMULATION_WIND_DIRECTION_TABLE[simulation_ctx.wind_direction_table_index]);
        SEN15901_exit_error(SIMULATION_ERROR_BASE_SEN15901);
        // Rainfall.
        if ((simulation_ctx.time_ms >= SIMULATION_RAINFALL_TIMESTAMP_MS) && (simulation_ctx.rainfall_mm < simulation_ctx.rainfall_peak_mm)) {
            // Add rain.
            sen15901_status = SEN15901_add_rainfall_mm(1);
            SEN15901_exit_error(SIMULATION_ERROR_BASE_SEN15901);
            // Update counter.
            simulation_ctx.rainfall_mm++;
        }
        if (log_enable != 0) {
            // Open terminal.
            terminal_status = TERMINAL_open(0, SIMULATION_LOG_BAUD_RATE, NULL);
            TERMINAL_stack_error(ERROR_BASE_TERMINAL);
            // Print current simulation values.
            _SIMULATION_print_sw_version();
            if (synchro_event != 0) {
                _SIMULATION_print_string("DUT_synchro");
            }
            _SIMULATION_print_value("Wind_speed=", (int32_t) simulation_ctx.wind_speed_kmh, "km/h");
            _SIMULATION_print_value("Wind_speed_peak=", (int32_t) simulation_ctx.wind_speed_peak_kmh, "km/h");
            _SIMULATION_print_value("Wind_direction=", (int32_t) SIMULATION_WIND_DIRECTION_TABLE[simulation_ctx.wind_direction_table_index], "d");
            _SIMULATION_print_value("Rainfall=", (int32_t) simulation_ctx.rainfall_mm, "mm");
            _SIMULATION_print_value("Rainfall_peak=", (int32_t) simulation_ctx.rainfall_peak_mm, "mm");
            _SIMULATION_print_string(NULL);
            // Close terminal.
            terminal_status = TERMINAL_close(0);
            TERMINAL_stack_error(ERROR_BASE_TERMINAL);
        }
    }
errors:
    return status;
}

/*
 * PC814.c
 *
 * PC814 Zero-Crossing Detection Optocoupler Library Implementation
 * Supports Timer Input Capture for AC line zero-crossing detection
 *
 * Author: Ehsan Zehni
 * Created: 2025
 * 
 * Description: Complete implementation of PC814 library with
 *              pull-up/pull-down configuration and zero-crossing detection
 */

#include "PC814.h"
#include <string.h>
#include <math.h>

/* Default values */
#define PC814_DEFAULT_FREQ 50           /* Default line frequency (Hz) */
#define PC814_DEFAULT_TOLERANCE 5.0f    /* Default frequency tolerance (%) */
#define PC814_PERIOD_50HZ_US 10000      /* Period for 50Hz in microseconds */
#define PC814_PERIOD_60HZ_US 8333       /* Period for 60Hz in microseconds */

/* Validate frequency */
static bool validate_frequency(uint32_t freq, uint32_t expected, float tolerance)
{
    if (freq == 0 || expected == 0) {
        return false;
    }
    
    float diff = (float)abs((int32_t)freq - (int32_t)expected);
    float percent = (diff / (float)expected) * 100.0f;
    
    return percent <= tolerance;
}

/* Initialize PC814 handle */
pc814_status_t pc814_init(pc814_handle_t *handle, pc814_port_t *port, 
                          pc814_pull_t pull_config, pc814_edge_t edge_type)
{
    if (handle == NULL || port == NULL) {
        return PC814_ERROR;
    }
    
    memset(handle, 0, sizeof(pc814_handle_t));
    handle->port = port;
    handle->pull_config = pull_config;
    handle->edge_type = edge_type;
    handle->expected_frequency = PC814_DEFAULT_FREQ;
    handle->frequency_tolerance = PC814_DEFAULT_TOLERANCE;
    handle->initialized = false;
    handle->data.valid = false;
    handle->callback = NULL;
    handle->period_sum = 0;
    handle->period_count = 0;
    memset(&handle->statistics, 0, sizeof(pc814_statistics_t));
    
    /* Configure GPIO pull-up/pull-down */
    if (pull_config == PC814_PULL_UP) {
        if (port->gpio_set_pull_up != NULL) {
            port->gpio_set_pull_up();
        }
    } else {
        if (port->gpio_set_pull_down != NULL) {
            port->gpio_set_pull_down();
        }
    }
    
    handle->initialized = true;
    return PC814_OK;
}

/* Process Timer Input Capture */
pc814_status_t pc814_process_capture(pc814_handle_t *handle)
{
    if (handle == NULL || !handle->initialized || handle->port == NULL) {
        return PC814_NOT_INITIALIZED;
    }
    
    if (handle->port->timer_get_capture_value == NULL ||
        handle->port->timer_get_frequency == NULL) {
        return PC814_ERROR;
    }
    
    uint32_t current_capture = handle->port->timer_get_capture_value();
    uint32_t timer_freq = handle->port->timer_get_frequency();
    
    if (current_capture == 0 || timer_freq == 0) {
        return PC814_ERROR;
    }
    
    /* Get current time */
    uint32_t current_time = 0;
    if (handle->port->get_time_us != NULL) {
        current_time = handle->port->get_time_us();
    }
    
    /* Calculate period if we have previous capture */
    if (handle->last_capture_value != 0) {
        uint32_t period_ticks;
        
        if (current_capture > handle->last_capture_value) {
            period_ticks = current_capture - handle->last_capture_value;
        } else {
            /* Handle timer overflow */
            period_ticks = (0xFFFFFFFF - handle->last_capture_value) + current_capture;
        }
        
        /* Convert ticks to microseconds */
        uint32_t period_us = (period_ticks * 1000000UL) / timer_freq;
        
        /* Calculate frequency */
        uint32_t freq_hz = 1000000UL / period_us;
        
        /* Validate frequency */
        bool freq_valid = validate_frequency(freq_hz, handle->expected_frequency, 
                                            handle->frequency_tolerance);
        
        /* Update data */
        handle->data.period_us = period_us;
        handle->data.frequency_hz = freq_hz;
        handle->data.timestamp_us = current_time;
        handle->data.count++;
        handle->data.valid = freq_valid;
        
        /* Update statistics */
        if (freq_valid) {
            handle->statistics.total_zc_count++;
            handle->statistics.valid_zc_count++;
            
            /* Update min/max period */
            if (handle->statistics.min_period_us == 0 || period_us < handle->statistics.min_period_us) {
                handle->statistics.min_period_us = period_us;
            }
            if (period_us > handle->statistics.max_period_us) {
                handle->statistics.max_period_us = period_us;
            }
            
            /* Update min/max frequency */
            float freq_float = (float)freq_hz;
            if (handle->statistics.min_frequency_hz == 0.0f || freq_float < handle->statistics.min_frequency_hz) {
                handle->statistics.min_frequency_hz = freq_float;
            }
            if (freq_float > handle->statistics.max_frequency_hz) {
                handle->statistics.max_frequency_hz = freq_float;
            }
            
            /* Update average period */
            handle->period_sum += period_us;
            handle->period_count++;
            if (handle->period_count > 0) {
                handle->statistics.avg_period_us = handle->period_sum / handle->period_count;
                handle->statistics.avg_frequency_hz = 1000000.0f / (float)handle->statistics.avg_period_us;
            }
        } else {
            handle->statistics.total_zc_count++;
            handle->statistics.invalid_zc_count++;
        }
        
        /* Call callback if set */
        if (handle->callback != NULL && freq_valid) {
            handle->callback(handle, &handle->data);
        }
    }
    
    handle->last_capture_value = current_capture;
    handle->last_capture_time = current_time;
    
    return PC814_OK;
}

/* Read zero-crossing data */
pc814_status_t pc814_read_data(pc814_handle_t *handle, pc814_data_t *data)
{
    if (handle == NULL || data == NULL || !handle->initialized) {
        return PC814_ERROR;
    }
    
    memcpy(data, &handle->data, sizeof(pc814_data_t));
    return PC814_OK;
}

/* Get line frequency */
uint32_t pc814_get_frequency(pc814_handle_t *handle)
{
    if (handle == NULL || !handle->initialized || !handle->data.valid) {
        return 0;
    }
    return handle->data.frequency_hz;
}

/* Get period between zero-crossings */
uint32_t pc814_get_period_us(pc814_handle_t *handle)
{
    if (handle == NULL || !handle->initialized || !handle->data.valid) {
        return 0;
    }
    return handle->data.period_us;
}

/* Get zero-crossing count */
uint32_t pc814_get_count(pc814_handle_t *handle)
{
    if (handle == NULL || !handle->initialized) {
        return 0;
    }
    return handle->data.count;
}

/* Get time since last zero-crossing */
uint32_t pc814_get_time_since_zc(pc814_handle_t *handle)
{
    if (handle == NULL || !handle->initialized || handle->port == NULL) {
        return 0;
    }
    
    if (handle->port->get_time_us == NULL) {
        return 0;
    }
    
    uint32_t current_time = handle->port->get_time_us();
    if (current_time < handle->data.timestamp_us) {
        /* Handle time overflow */
        return 0;
    }
    
    return current_time - handle->data.timestamp_us;
}

/* Set expected line frequency */
void pc814_set_expected_frequency(pc814_handle_t *handle, uint32_t freq)
{
    if (handle != NULL && (freq == 50 || freq == 60)) {
        handle->expected_frequency = freq;
    }
}

/* Set frequency tolerance */
void pc814_set_frequency_tolerance(pc814_handle_t *handle, float tolerance)
{
    if (handle != NULL && tolerance > 0.0f && tolerance <= 50.0f) {
        handle->frequency_tolerance = tolerance;
    }
}

/* Check if data is valid */
bool pc814_is_data_valid(pc814_handle_t *handle)
{
    if (handle == NULL || !handle->initialized) {
        return false;
    }
    return handle->data.valid;
}

/* Reset handle */
void pc814_reset(pc814_handle_t *handle)
{
    if (handle == NULL) {
        return;
    }
    
    handle->last_capture_value = 0;
    handle->last_capture_time = 0;
    handle->data.count = 0;
    handle->data.valid = false;
    
    if (handle->port != NULL && handle->port->timer_reset_capture != NULL) {
        handle->port->timer_reset_capture();
    }
}

/* Set callback */
void pc814_set_callback(pc814_handle_t *handle, pc814_zc_callback_t callback)
{
    if (handle != NULL) {
        handle->callback = callback;
    }
}

/* Start zero-crossing detection */
pc814_status_t pc814_start(pc814_handle_t *handle)
{
    if (handle == NULL || !handle->initialized || handle->port == NULL) {
        return PC814_ERROR;
    }
    
    if (handle->port->timer_start_capture != NULL) {
        handle->port->timer_start_capture();
    }
    
    return PC814_OK;
}

/* Stop zero-crossing detection */
void pc814_stop(pc814_handle_t *handle)
{
    if (handle == NULL || handle->port == NULL) {
        return;
    }
    
    if (handle->port->timer_stop_capture != NULL) {
        handle->port->timer_stop_capture();
    }
}

/* Calculate phase angle from time offset */
float pc814_calc_phase_angle(uint32_t time_offset_us, uint32_t line_freq)
{
    if (line_freq == 0) {
        return 0.0f;
    }
    
    /* Calculate period in microseconds */
    uint32_t period_us = 1000000UL / line_freq;
    
    if (period_us == 0) {
        return 0.0f;
    }
    
    /* Calculate phase angle: (time_offset / period) * 360 */
    float phase = ((float)time_offset_us / (float)period_us) * 360.0f;
    
    /* Normalize to 0-360 range */
    while (phase >= 360.0f) {
        phase -= 360.0f;
    }
    while (phase < 0.0f) {
        phase += 360.0f;
    }
    
    return phase;
}

/* Calculate time offset for desired phase angle */
uint32_t pc814_calc_time_for_phase(float phase_deg, uint32_t line_freq)
{
    if (line_freq == 0) {
        return 0;
    }
    
    /* Normalize phase to 0-360 */
    while (phase_deg >= 360.0f) {
        phase_deg -= 360.0f;
    }
    while (phase_deg < 0.0f) {
        phase_deg += 360.0f;
    }
    
    /* Calculate period in microseconds */
    uint32_t period_us = 1000000UL / line_freq;
    
    /* Calculate time offset: (phase / 360) * period */
    uint32_t time_offset = (uint32_t)((phase_deg / 360.0f) * (float)period_us);
    
    return time_offset;
}

/* Get statistics */
pc814_status_t pc814_get_statistics(pc814_handle_t *handle, pc814_statistics_t *stats)
{
    if (handle == NULL || stats == NULL || !handle->initialized) {
        return PC814_ERROR;
    }
    
    memcpy(stats, &handle->statistics, sizeof(pc814_statistics_t));
    return PC814_OK;
}

/* Reset statistics */
void pc814_reset_statistics(pc814_handle_t *handle)
{
    if (handle == NULL) {
        return;
    }
    
    memset(&handle->statistics, 0, sizeof(pc814_statistics_t));
    handle->period_sum = 0;
    handle->period_count = 0;
}

/* Wait for next zero-crossing */
pc814_status_t pc814_wait_for_zc(pc814_handle_t *handle, uint32_t timeout_ms)
{
    if (handle == NULL || !handle->initialized) {
        return PC814_ERROR;
    }
    
    uint32_t last_count = handle->data.count;
    uint32_t start_time = 0;
    
    if (handle->port != NULL && handle->port->get_time_us != NULL) {
        start_time = handle->port->get_time_us() / 1000;  /* Convert to ms */
    }
    
    while (handle->data.count == last_count) {
        if (timeout_ms > 0) {
            uint32_t current_time = 0;
            if (handle->port != NULL && handle->port->get_time_us != NULL) {
                current_time = handle->port->get_time_us() / 1000;
            }
            
            if ((current_time - start_time) >= timeout_ms) {
                return PC814_ERROR;  /* Timeout */
            }
        }
        
        if (handle->port != NULL && handle->port->delay_ms != NULL) {
            handle->port->delay_ms(1);
        }
    }
    
    return PC814_OK;
}

/* Check if new zero-crossing occurred */
bool pc814_is_new_zc(pc814_handle_t *handle, uint32_t last_count)
{
    if (handle == NULL || !handle->initialized) {
        return false;
    }
    
    return handle->data.count > last_count;
}

/* Get half period */
uint32_t pc814_get_half_period_us(pc814_handle_t *handle)
{
    if (handle == NULL || !handle->initialized || !handle->data.valid) {
        return 0;
    }
    
    return handle->data.period_us / 2;
}

/* Get quarter period */
uint32_t pc814_get_quarter_period_us(pc814_handle_t *handle)
{
    if (handle == NULL || !handle->initialized || !handle->data.valid) {
        return 0;
    }
    
    return handle->data.period_us / 4;
}


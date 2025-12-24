/*
 * PC814.h
 *
 * PC814 Zero-Crossing Detection Optocoupler Library
 * Supports Timer Input Capture for AC line zero-crossing detection
 *
 * Author: Ehsan Zehni
 * Created: 2025
 * 
 * Description: Complete library for PC814 optocoupler zero-crossing detection
 *              with Timer Input Capture support and pull-up/pull-down configuration
 */

#ifndef PC814_H
#define PC814_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* Return codes */
typedef enum {
    PC814_OK = 0,
    PC814_ERROR = -1,
    PC814_NOT_INITIALIZED = -2,
    PC814_INVALID_PARAM = -3
} pc814_status_t;

/* Pull configuration */
typedef enum {
    PC814_PULL_UP = 0,      /* Pull-up configuration */
    PC814_PULL_DOWN = 1     /* Pull-down configuration */
} pc814_pull_t;

/* Zero-crossing edge type */
typedef enum {
    PC814_EDGE_RISING = 0,  /* Rising edge (low to high) */
    PC814_EDGE_FALLING = 1  /* Falling edge (high to low) */
} pc814_edge_t;

/* Zero-crossing data structure */
typedef struct {
    uint32_t period_us;         /* Period between zero-crossings in microseconds */
    uint32_t frequency_hz;      /* Line frequency in Hz (50 or 60) */
    uint32_t timestamp_us;      /* Timestamp of last zero-crossing */
    uint32_t count;             /* Total zero-crossing count */
    bool valid;                 /* Data validity flag */
} pc814_data_t;

/* Statistics structure */
typedef struct {
    uint32_t total_zc_count;    /* Total zero-crossing count */
    uint32_t valid_zc_count;    /* Valid zero-crossing count */
    uint32_t invalid_zc_count;  /* Invalid zero-crossing count */
    uint32_t min_period_us;     /* Minimum period in microseconds */
    uint32_t max_period_us;     /* Maximum period in microseconds */
    uint32_t avg_period_us;     /* Average period in microseconds */
    float min_frequency_hz;      /* Minimum frequency in Hz */
    float max_frequency_hz;     /* Maximum frequency in Hz */
    float avg_frequency_hz;     /* Average frequency in Hz */
} pc814_statistics_t;

/* Port functions structure - user must implement */
typedef struct {
    /* Timer input capture functions */
    uint32_t (*timer_get_capture_value)(void);
    uint32_t (*timer_get_frequency)(void);
    void (*timer_reset_capture)(void);
    void (*timer_start_capture)(void);
    void (*timer_stop_capture)(void);
    
    /* GPIO functions for pull-up/pull-down */
    void (*gpio_set_pull_up)(void);
    void (*gpio_set_pull_down)(void);
    
    /* System time functions */
    uint32_t (*get_time_us)(void);  /* Get system time in microseconds */
    
    /* Delay function */
    void (*delay_us)(uint32_t us);
    void (*delay_ms)(uint32_t ms);
} pc814_port_t;

/* PC814 handle structure */
typedef struct {
    pc814_port_t *port;
    pc814_pull_t pull_config;
    pc814_edge_t edge_type;
    pc814_data_t data;
    uint32_t last_capture_time;
    uint32_t last_capture_value;
    bool initialized;
    uint32_t expected_frequency;  /* Expected line frequency (50 or 60 Hz) */
    float frequency_tolerance;    /* Frequency tolerance for validation (%) */
    pc814_zc_callback_t callback; /* Zero-crossing callback function */
    pc814_statistics_t statistics; /* Statistics data */
    uint32_t period_sum;          /* Sum of periods for average calculation */
    uint32_t period_count;        /* Count of periods for average */
} pc814_handle_t;

/* Callback function types */
typedef void (*pc814_zc_callback_t)(pc814_handle_t *handle, pc814_data_t *data);

/**
 * Initialize PC814 handle
 * @param handle Pointer to handle structure
 * @param port Pointer to port functions structure
 * @param pull_config Pull-up or pull-down configuration
 * @param edge_type Rising or falling edge detection
 * @return PC814_OK on success
 */
pc814_status_t pc814_init(pc814_handle_t *handle, pc814_port_t *port, 
                          pc814_pull_t pull_config, pc814_edge_t edge_type);

/**
 * Process Timer Input Capture (call from HAL_TIM_IC_CaptureCallback)
 * @param handle Pointer to handle structure
 * @return PC814_OK when zero-crossing detected
 */
pc814_status_t pc814_process_capture(pc814_handle_t *handle);

/**
 * Read zero-crossing data
 * @param handle Pointer to handle structure
 * @param data Pointer to data structure to fill
 * @return PC814_OK on success
 */
pc814_status_t pc814_read_data(pc814_handle_t *handle, pc814_data_t *data);

/**
 * Get line frequency
 * @param handle Pointer to handle structure
 * @return Frequency in Hz (50 or 60), 0 on error
 */
uint32_t pc814_get_frequency(pc814_handle_t *handle);

/**
 * Get period between zero-crossings
 * @param handle Pointer to handle structure
 * @return Period in microseconds, 0 on error
 */
uint32_t pc814_get_period_us(pc814_handle_t *handle);

/**
 * Get zero-crossing count
 * @param handle Pointer to handle structure
 * @return Count of zero-crossings detected
 */
uint32_t pc814_get_count(pc814_handle_t *handle);

/**
 * Calculate time since last zero-crossing
 * @param handle Pointer to handle structure
 * @return Time in microseconds since last zero-crossing, 0 on error
 */
uint32_t pc814_get_time_since_zc(pc814_handle_t *handle);

/**
 * Set expected line frequency
 * @param handle Pointer to handle structure
 * @param freq Expected frequency (50 or 60 Hz)
 */
void pc814_set_expected_frequency(pc814_handle_t *handle, uint32_t freq);

/**
 * Set frequency tolerance for validation
 * @param handle Pointer to handle structure
 * @param tolerance Tolerance in percent (e.g., 5.0 for 5%)
 */
void pc814_set_frequency_tolerance(pc814_handle_t *handle, float tolerance);

/**
 * Check if zero-crossing data is valid
 * @param handle Pointer to handle structure
 * @return true if data is valid
 */
bool pc814_is_data_valid(pc814_handle_t *handle);

/**
 * Reset handle and statistics
 * @param handle Pointer to handle structure
 */
void pc814_reset(pc814_handle_t *handle);

/**
 * Set zero-crossing callback
 * @param handle Pointer to handle structure
 * @param callback Callback function pointer
 */
void pc814_set_callback(pc814_handle_t *handle, pc814_zc_callback_t callback);

/**
 * Start zero-crossing detection
 * @param handle Pointer to handle structure
 * @return PC814_OK on success
 */
pc814_status_t pc814_start(pc814_handle_t *handle);

/**
 * Stop zero-crossing detection
 * @param handle Pointer to handle structure
 */
void pc814_stop(pc814_handle_t *handle);

/**
 * Calculate phase angle from time offset
 * @param time_offset_us Time offset from zero-crossing in microseconds
 * @param line_freq Line frequency in Hz
 * @return Phase angle in degrees (0-360)
 */
float pc814_calc_phase_angle(uint32_t time_offset_us, uint32_t line_freq);

/**
 * Calculate time offset for desired phase angle
 * @param phase_deg Desired phase angle in degrees
 * @param line_freq Line frequency in Hz
 * @return Time offset in microseconds
 */
uint32_t pc814_calc_time_for_phase(float phase_deg, uint32_t line_freq);

/**
 * Get statistics
 * @param handle Pointer to handle structure
 * @param stats Pointer to statistics structure
 * @return PC814_OK on success
 */
pc814_status_t pc814_get_statistics(pc814_handle_t *handle, pc814_statistics_t *stats);

/**
 * Reset statistics
 * @param handle Pointer to handle structure
 */
void pc814_reset_statistics(pc814_handle_t *handle);

/**
 * Wait for next zero-crossing (blocking)
 * @param handle Pointer to handle structure
 * @param timeout_ms Timeout in milliseconds (0 for infinite)
 * @return PC814_OK on success, PC814_ERROR on timeout
 */
pc814_status_t pc814_wait_for_zc(pc814_handle_t *handle, uint32_t timeout_ms);

/**
 * Check if zero-crossing occurred since last check
 * @param handle Pointer to handle structure
 * @param last_count Last known count
 * @return true if new zero-crossing occurred
 */
bool pc814_is_new_zc(pc814_handle_t *handle, uint32_t last_count);

/**
 * Get half period (for 180 degree calculations)
 * @param handle Pointer to handle structure
 * @return Half period in microseconds, 0 on error
 */
uint32_t pc814_get_half_period_us(pc814_handle_t *handle);

/**
 * Get quarter period (for 90 degree calculations)
 * @param handle Pointer to handle structure
 * @return Quarter period in microseconds, 0 on error
 */
uint32_t pc814_get_quarter_period_us(pc814_handle_t *handle);

#ifdef __cplusplus
}
#endif

#endif /* PC814_H */


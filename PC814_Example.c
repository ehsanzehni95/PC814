/*
 * PC814_Example.c
 *
 * Complete usage example for PC814 zero-crossing detection library
 * This file demonstrates how to use PC814 with Timer Input Capture
 *
 * Author: Ehsan Zehni
 * Created: 2025
 */

#include "PC814.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>

/* External handles - adjust for your hardware */
extern TIM_HandleTypeDef htim2;    /* Timer for Input Capture */
extern GPIO_TypeDef *PC814_GPIO_Port;
extern uint16_t PC814_GPIO_Pin;

/* Timer variables for zero-crossing detection */
static volatile uint32_t timer_capture_value = 0;
static volatile uint32_t timer_capture_last = 0;
static volatile uint32_t timer_period = 0;
static volatile bool timer_capture_ready = false;
static volatile uint32_t timer_frequency = 1000000;  /* 1MHz timer clock */

/* System time variable (use HAL_GetTick or similar) */
static uint32_t system_time_us = 0;

/* ========== Port Functions Implementation ========== */

/* Get timer capture value */
static uint32_t timer_get_capture_value(void)
{
    /* Return the period between two captures */
    if (timer_capture_ready) {
        timer_capture_ready = false;
        return timer_period;
    }
    return timer_capture_value;  /* Fallback to single capture */
}

/* Get timer frequency */
static uint32_t timer_get_frequency(void)
{
    /* Return timer clock frequency in Hz */
    /* Adjust based on your timer configuration */
    return timer_frequency;
}

/* Reset timer capture */
static void timer_reset_capture(void)
{
    /* Reset timer capture registers and variables */
    __HAL_TIM_SET_COUNTER(&htim2, 0);
    timer_capture_value = 0;
    timer_capture_last = 0;
    timer_period = 0;
    timer_capture_ready = false;
}

/* Start timer capture */
static void timer_start_capture(void)
{
    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
}

/* Stop timer capture */
static void timer_stop_capture(void)
{
    HAL_TIM_IC_Stop_IT(&htim2, TIM_CHANNEL_1);
}

/* Set GPIO pull-up */
static void gpio_set_pull_up(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = PC814_GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(PC814_GPIO_Port, &GPIO_InitStruct);
}

/* Set GPIO pull-down */
static void gpio_set_pull_down(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = PC814_GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(PC814_GPIO_Port, &GPIO_InitStruct);
}

/* Get system time in microseconds */
static uint32_t get_time_us(void)
{
    /* Use HAL_GetTick() and convert to microseconds */
    /* Adjust based on your system tick frequency */
    return HAL_GetTick() * 1000;  /* If HAL_GetTick returns milliseconds */
}

/* Delay in microseconds */
static void delay_us(uint32_t us)
{
    /* Use DWT or timer-based delay */
    /* For now, approximate using HAL_Delay */
    if (us >= 1000) {
        HAL_Delay(us / 1000);
    }
}

/* Delay in milliseconds */
static void delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}

/* Port functions structure */
static pc814_port_t pc814_port = {
    .timer_get_capture_value = timer_get_capture_value,
    .timer_get_frequency = timer_get_frequency,
    .timer_reset_capture = timer_reset_capture,
    .timer_start_capture = timer_start_capture,
    .timer_stop_capture = timer_stop_capture,
    .gpio_set_pull_up = gpio_set_pull_up,
    .gpio_set_pull_down = gpio_set_pull_down,
    .get_time_us = get_time_us,
    .delay_us = delay_us,
    .delay_ms = delay_ms
};

/* PC814 handle */
static pc814_handle_t pc814_handle;

/* ========== Timer Input Capture Callback ========== */
/* 
 * This callback must be called from HAL_TIM_IC_CaptureCallback
 * 
 * In stm32f4xx_it.c or main.c:
 * 
 * void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
 * {
 *     if (htim->Instance == TIM2) {
 *         PC814_TIM_IC_CaptureCallback(htim);
 *     }
 * }
 */
void PC814_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == htim2.Instance) {
        /* Read current capture value */
        uint32_t current_capture = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
        
        if (timer_capture_last != 0) {
            /* Calculate period between two captures */
            if (current_capture > timer_capture_last) {
                timer_period = current_capture - timer_capture_last;
            } else {
                /* Handle timer overflow */
                timer_period = (0xFFFFFFFF - timer_capture_last) + current_capture;
            }
            timer_capture_ready = true;
            
            /* Process zero-crossing */
            pc814_process_capture(&pc814_handle);
        }
        
        timer_capture_last = current_capture;
        timer_capture_value = current_capture;
        
        /* Reset counter for next capture */
        __HAL_TIM_SET_COUNTER(htim, 0);
        
        /* Restart Input Capture */
        HAL_TIM_IC_Start_IT(htim, TIM_CHANNEL_1);
    }
}

/* ========== Example Functions ========== */

/**
 * Initialize PC814 with pull-up configuration
 */
void PC814_Example_Init_PullUp(void)
{
    pc814_status_t status;
    
    /* Initialize PC814 library with pull-up */
    status = pc814_init(&pc814_handle, &pc814_port, 
                       PC814_PULL_UP, PC814_EDGE_RISING);
    
    if (status != PC814_OK) {
        printf("PC814 init failed!\r\n");
        return;
    }
    
    /* Set expected frequency (50Hz for 220V AC) */
    pc814_set_expected_frequency(&pc814_handle, 50);
    pc814_set_frequency_tolerance(&pc814_handle, 5.0f);
    
    /* Start zero-crossing detection */
    pc814_start(&pc814_handle);
    
    printf("PC814 initialized with pull-up configuration\r\n");
}

/**
 * Initialize PC814 with pull-down configuration
 */
void PC814_Example_Init_PullDown(void)
{
    pc814_status_t status;
    
    /* Initialize PC814 library with pull-down */
    status = pc814_init(&pc814_handle, &pc814_port, 
                       PC814_PULL_DOWN, PC814_EDGE_FALLING);
    
    if (status != PC814_OK) {
        printf("PC814 init failed!\r\n");
        return;
    }
    
    /* Set expected frequency (50Hz for 220V AC) */
    pc814_set_expected_frequency(&pc814_handle, 50);
    pc814_set_frequency_tolerance(&pc814_handle, 5.0f);
    
    /* Start zero-crossing detection */
    pc814_start(&pc814_handle);
    
    printf("PC814 initialized with pull-down configuration\r\n");
}

/**
 * Read zero-crossing data
 */
void PC814_Example_ReadData(void)
{
    pc814_data_t data;
    pc814_status_t status;
    
    status = pc814_read_data(&pc814_handle, &data);
    
    if (status == PC814_OK && data.valid) {
        printf("=== PC814 Zero-Crossing Data ===\r\n");
        printf("Frequency: %lu Hz\r\n", data.frequency_hz);
        printf("Period: %lu us\r\n", data.period_us);
        printf("Count: %lu\r\n", data.count);
        printf("Timestamp: %lu us\r\n", data.timestamp_us);
        printf("================================\r\n");
    } else {
        printf("Data not ready or invalid\r\n");
    }
}

/**
 * Monitor zero-crossing continuously
 */
void PC814_Example_Monitor(void)
{
    pc814_data_t data;
    uint32_t time_since_zc;
    
    if (pc814_read_data(&pc814_handle, &data) == PC814_OK && data.valid) {
        /* Get time since last zero-crossing */
        time_since_zc = pc814_get_time_since_zc(&pc814_handle);
        
        printf("Line Frequency: %lu Hz\r\n", data.frequency_hz);
        printf("Period: %lu us\r\n", data.period_us);
        printf("Time since last ZC: %lu us\r\n", time_since_zc);
        printf("Total ZC count: %lu\r\n", data.count);
        
        /* Calculate phase angle */
        float phase = pc814_calc_phase_angle(time_since_zc, data.frequency_hz);
        printf("Current phase angle: %.2f degrees\r\n", phase);
    }
}

/**
 * Calculate timing for phase control
 */
void PC814_Example_PhaseControl(void)
{
    pc814_data_t data;
    
    if (pc814_read_data(&pc814_handle, &data) == PC814_OK && data.valid) {
        /* Calculate time offset for 90 degrees phase shift */
        uint32_t time_90deg = pc814_calc_time_for_phase(90.0f, data.frequency_hz);
        printf("Time for 90 degrees: %lu us\r\n", time_90deg);
        
        /* Calculate time offset for 180 degrees phase shift */
        uint32_t time_180deg = pc814_calc_time_for_phase(180.0f, data.frequency_hz);
        printf("Time for 180 degrees: %lu us\r\n", time_180deg);
    }
}

/**
 * Example: Wait for next zero-crossing
 */
void PC814_Example_WaitForZeroCrossing(void)
{
    uint32_t last_count = pc814_get_count(&pc814_handle);
    
    printf("Waiting for zero-crossing...\r\n");
    
    /* Wait until count increases */
    while (pc814_get_count(&pc814_handle) == last_count) {
        HAL_Delay(1);
    }
    
    printf("Zero-crossing detected!\r\n");
    PC814_Example_ReadData();
}

/**
 * Zero-crossing callback function
 */
void PC814_ZeroCrossingCallback(pc814_handle_t *handle, pc814_data_t *data)
{
    /* This function is called automatically on each zero-crossing */
    printf("ZC Callback: Frequency=%lu Hz, Count=%lu\r\n", 
           data->frequency_hz, data->count);
    
    /* Example: Trigger TRIAC or SSR at specific phase */
    /* uint32_t time_90deg = pc814_calc_time_for_phase(90.0f, data->frequency_hz); */
    /* Schedule TRIAC trigger after time_90deg microseconds */
}

/**
 * Example: Using callback
 */
void PC814_Example_WithCallback(void)
{
    /* Set callback function */
    pc814_set_callback(&pc814_handle, PC814_ZeroCrossingCallback);
    
    /* Callback will be called automatically on each zero-crossing */
    /* No need to poll - just process in callback */
}

/**
 * Example: Get statistics
 */
void PC814_Example_GetStatistics(void)
{
    pc814_statistics_t stats;
    
    if (pc814_get_statistics(&pc814_handle, &stats) == PC814_OK) {
        printf("=== PC814 Statistics ===\r\n");
        printf("Total ZC Count: %lu\r\n", stats.total_zc_count);
        printf("Valid ZC Count: %lu\r\n", stats.valid_zc_count);
        printf("Invalid ZC Count: %lu\r\n", stats.invalid_zc_count);
        printf("Min Period: %lu us\r\n", stats.min_period_us);
        printf("Max Period: %lu us\r\n", stats.max_period_us);
        printf("Avg Period: %lu us\r\n", stats.avg_period_us);
        printf("Min Frequency: %.2f Hz\r\n", stats.min_frequency_hz);
        printf("Max Frequency: %.2f Hz\r\n", stats.max_frequency_hz);
        printf("Avg Frequency: %.2f Hz\r\n", stats.avg_frequency_hz);
        printf("=======================\r\n");
    }
}

/**
 * Example: Quick phase calculations
 */
void PC814_Example_QuickPhase(void)
{
    /* Get half period (for 180 degrees) */
    uint32_t half_period = pc814_get_half_period_us(&pc814_handle);
    printf("Half period (180°): %lu us\r\n", half_period);
    
    /* Get quarter period (for 90 degrees) */
    uint32_t quarter_period = pc814_get_quarter_period_us(&pc814_handle);
    printf("Quarter period (90°): %lu us\r\n", quarter_period);
}

/* ========== Main Usage Example ========== */
/*
void main(void)
{
    // Initialize system
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_TIM2_Init();   // Timer for Input Capture
    
    // Initialize PC814 with pull-up (or pull-down)
    PC814_Example_Init_PullUp();
    // or
    // PC814_Example_Init_PullDown();
    
    // Main loop
    while (1) {
        // Read and display zero-crossing data
        PC814_Example_ReadData();
        
        // Monitor continuously
        PC814_Example_Monitor();
        
        // Phase control example
        PC814_Example_PhaseControl();
        
        HAL_Delay(1000);  // Wait 1 second
    }
}
*/


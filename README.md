# PC814 Zero-Crossing Detection Optocoupler Library

**Author:** Ehsan Zehni  
**Version:** 1.0  
**Date:** 2025

Complete embedded library for PC814 zero-crossing detection optocoupler with Timer Input Capture support, configurable pull-up/pull-down GPIO settings, line frequency measurement, phase angle calculations, statistics, and callback functions. Includes comprehensive three-phase system support with automatic phase sequence detection (ABC/ACB) and swap recommendations. Production-ready for AC 220V single-phase and three-phase zero-crossing detection applications.

**Short Description for GitHub (max 350 chars):**

PC814 zero-crossing detection library with Timer Input Capture, pull-up/pull-down config, frequency measurement, phase calculations, statistics, callbacks. Includes three-phase support with automatic phase sequence detection (ABC/ACB) and swap recommendations. For AC 220V single/three-phase applications.

## Description

A comprehensive, production-ready C library for interfacing with PC814 zero-crossing detection optocoupler. The library provides Timer Input Capture support for accurate zero-crossing detection, configurable pull-up/pull-down GPIO settings, line frequency measurement, phase angle calculations, and callback functions for zero-crossing events.

## Features

### Core Features
- ✅ Timer Input Capture for zero-crossing detection
- ✅ Configurable pull-up/pull-down GPIO settings
- ✅ Automatic line frequency measurement (50/60 Hz)
- ✅ Period calculation between zero-crossings
- ✅ Zero-crossing count tracking
- ✅ Phase angle calculations
- ✅ Frequency validation with tolerance
- ✅ Modular and portable architecture

### Advanced Features
- ✅ **Pull Configuration**: Select pull-up or pull-down for GPIO
- ✅ **Edge Detection**: Configurable rising or falling edge
- ✅ **Frequency Validation**: Automatic validation with configurable tolerance
- ✅ **Phase Calculations**: Calculate phase angle from time offset
- ✅ **Timing Calculations**: Calculate time offset for desired phase angle
- ✅ **Time Tracking**: Track time since last zero-crossing
- ✅ **Callback Support**: Support for zero-crossing event callbacks
- ✅ **Statistics**: Zero-crossing count and timing statistics
- ✅ **Error Handling**: Complete error management
- ✅ **Data Validation**: Automatic data validity checking

## Hardware Connections

- **PC814 Output**: Connected to Timer Input Capture pin
- **PC814 VCC**: Connected to microcontroller supply (3.3V or 5V)
- **PC814 GND**: Connected to ground
- **PC814 AC Input**: Connected to AC 220V line (through appropriate isolation)

## File Structure

- `PC814.h`: Header file with all definitions and functions
- `PC814.c`: Complete library implementation
- `PC814_Example.c`: Complete usage example with Timer Input Capture

## Quick Start

### 1. Implement Port Functions

Implement port functions according to your hardware:

```c
pc814_port_t pc814_port = {
    .timer_get_capture_value = your_timer_get_capture,
    .timer_get_frequency = your_timer_get_freq,
    .timer_reset_capture = your_timer_reset,
    .timer_start_capture = your_timer_start,
    .timer_stop_capture = your_timer_stop,
    .gpio_set_pull_up = your_gpio_pull_up,
    .gpio_set_pull_down = your_gpio_pull_down,
    .get_time_us = your_get_time_us,
    .delay_us = your_delay_us,
    .delay_ms = your_delay_ms
};
```

### 2. Initialize with Pull-Up

```c
pc814_handle_t pc814;
pc814_init(&pc814, &pc814_port, PC814_PULL_UP, PC814_EDGE_RISING);
pc814_set_expected_frequency(&pc814, 50);  // 50Hz for 220V AC
pc814_start(&pc814);
```

### 3. Initialize with Pull-Down

```c
pc814_handle_t pc814;
pc814_init(&pc814, &pc814_port, PC814_PULL_DOWN, PC814_EDGE_FALLING);
pc814_set_expected_frequency(&pc814, 50);  // 50Hz for 220V AC
pc814_start(&pc814);
```

### 4. Process Input Capture

In Timer Input Capture callback:

```c
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2) {
        PC814_TIM_IC_CaptureCallback(htim);
    }
}
```

### 5. Read Data

```c
pc814_data_t data;
if (pc814_read_data(&pc814, &data) == PC814_OK && data.valid) {
    uint32_t freq = data.frequency_hz;      // Line frequency (Hz)
    uint32_t period = data.period_us;       // Period (microseconds)
    uint32_t count = data.count;             // Zero-crossing count
}
```

## Timer Input Capture Configuration

### Configuration in STM32CubeMX

1. **Select Timer**: Choose an appropriate Timer (e.g., TIM2)
2. **Input Capture Mode**: 
   - Set Channel to Input Capture
   - Mode: Input Capture direct mode
   - Polarity: Rising Edge (for pull-up) or Falling Edge (for pull-down)
3. **Prescaler**: Configure for appropriate timer frequency
   - Example: For 1MHz timer frequency, Prescaler = 84 (if clock is 84MHz)
4. **Period**: Maximum timer value

### Example Configuration

```c
TIM_HandleTypeDef htim2;

void MX_TIM2_Init(void)
{
    TIM_IC_InitTypeDef sConfigIC = {0};
    
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 84 - 1;        // For 1MHz timer frequency
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 0xFFFFFFFF;        // Maximum period
    htim2.Init.ClockDivision = TIM_CLKDIVISION_DIV1;
    
    if (HAL_TIM_IC_Init(&htim2) != HAL_OK) {
        Error_Handler();
    }
    
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;  // or FALLING
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
    sConfigIC.ICFilter = 0;
    
    if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_1) != HAL_OK) {
        Error_Handler();
    }
}
```

## Main Functions

### Basic Functions
- `pc814_init()`: Initialize handle with pull-up/pull-down configuration
- `pc814_process_capture()`: Process Timer Input Capture (call from callback)
- `pc814_read_data()`: Read zero-crossing data
- `pc814_start()`: Start zero-crossing detection
- `pc814_stop()`: Stop zero-crossing detection
- `pc814_reset()`: Reset handle and statistics

### Data Reading Functions
- `pc814_get_frequency()`: Get line frequency (Hz)
- `pc814_get_period_us()`: Get period between zero-crossings (microseconds)
- `pc814_get_count()`: Get zero-crossing count
- `pc814_get_time_since_zc()`: Get time since last zero-crossing
- `pc814_is_data_valid()`: Check if data is valid

### Configuration Functions
- `pc814_set_expected_frequency()`: Set expected line frequency (50/60 Hz)
- `pc814_set_frequency_tolerance()`: Set frequency tolerance for validation (%)
- `pc814_set_callback()`: Set zero-crossing callback

### Calculation Functions
- `pc814_calc_phase_angle()`: Calculate phase angle from time offset
- `pc814_calc_time_for_phase()`: Calculate time offset for desired phase angle
- `pc814_get_half_period_us()`: Get half period (for 180° calculations)
- `pc814_get_quarter_period_us()`: Get quarter period (for 90° calculations)

### Statistics Functions
- `pc814_get_statistics()`: Get complete statistics
- `pc814_reset_statistics()`: Reset statistics

### Utility Functions
- `pc814_wait_for_zc()`: Wait for next zero-crossing (blocking)
- `pc814_is_new_zc()`: Check if new zero-crossing occurred

## Return Codes

- `PC814_OK`: Success
- `PC814_ERROR`: General error
- `PC814_NOT_INITIALIZED`: Handle not initialized
- `PC814_INVALID_PARAM`: Invalid parameter

## Usage Examples

### Example 1: Basic Usage with Pull-Up

```c
pc814_handle_t pc814;
pc814_data_t data;

// Initialize with pull-up
pc814_init(&pc814, &pc814_port, PC814_PULL_UP, PC814_EDGE_RISING);
pc814_set_expected_frequency(&pc814, 50);
pc814_start(&pc814);

// Read data
if (pc814_read_data(&pc814, &data) == PC814_OK && data.valid) {
    printf("Frequency: %lu Hz\n", data.frequency_hz);
    printf("Period: %lu us\n", data.period_us);
}
```

### Example 2: Phase Angle Calculation

```c
uint32_t time_since_zc = pc814_get_time_since_zc(&pc814);
uint32_t freq = pc814_get_frequency(&pc814);
float phase = pc814_calc_phase_angle(time_since_zc, freq);
printf("Current phase: %.2f degrees\n", phase);
```

### Example 3: Calculate Timing for Phase Control

```c
uint32_t freq = pc814_get_frequency(&pc814);

// Calculate time for 90 degrees phase shift
uint32_t time_90deg = pc814_calc_time_for_phase(90.0f, freq);
printf("Time for 90 degrees: %lu us\n", time_90deg);

// Use this timing for phase control (e.g., TRIAC triggering)
```

### Example 4: Wait for Zero-Crossing

```c
// Blocking wait (with timeout)
if (pc814_wait_for_zc(&pc814, 1000) == PC814_OK) {
    printf("Zero-crossing detected!\n");
} else {
    printf("Timeout waiting for zero-crossing\n");
}

// Or non-blocking check
uint32_t last_count = pc814_get_count(&pc814);
if (pc814_is_new_zc(&pc814, last_count)) {
    printf("New zero-crossing detected!\n");
}
```

### Example 5: Get Statistics

```c
pc814_statistics_t stats;
pc814_get_statistics(&pc814, &stats);

printf("Total ZC: %lu\n", stats.total_zc_count);
printf("Valid ZC: %lu\n", stats.valid_zc_count);
printf("Min Period: %lu us\n", stats.min_period_us);
printf("Max Period: %lu us\n", stats.max_period_us);
printf("Avg Period: %lu us\n", stats.avg_period_us);
printf("Avg Frequency: %.2f Hz\n", stats.avg_frequency_hz);
```

### Example 6: Quick Phase Calculations

```c
// Get half period (for 180 degrees)
uint32_t half_period = pc814_get_half_period_us(&pc814);

// Get quarter period (for 90 degrees)
uint32_t quarter_period = pc814_get_quarter_period_us(&pc814);

printf("Half period (180°): %lu us\n", half_period);
printf("Quarter period (90°): %lu us\n", quarter_period);
```

### Example 7: Three-Phase System

```c
// Initialize three phases
pc814_handle_t phase_a, phase_b, phase_c;
pc814_init(&phase_a, &port_a, PC814_PULL_UP, PC814_EDGE_RISING);
pc814_init(&phase_b, &port_b, PC814_PULL_UP, PC814_EDGE_RISING);
pc814_init(&phase_c, &port_c, PC814_PULL_UP, PC814_EDGE_RISING);

// Initialize three-phase system
pc814_threephase_t threephase;
pc814_threephase_init(&threephase, &phase_a, &phase_b, &phase_c);

// Process and detect sequence
pc814_threephase_process(&threephase);

// Check sequence
if (pc814_threephase_is_sequence_correct(&threephase)) {
    printf("Sequence is CORRECT (ABC)\n");
} else {
    printf("Sequence is REVERSE - needs correction\n");
    
    // Get which phases to swap
    bool swap_ab, swap_bc, swap_ca;
    pc814_threephase_get_swap_recommendation(&threephase, &swap_ab, &swap_bc, &swap_ca);
    
    if (swap_bc) {
        printf("SWAP phases B and C\n");
    } else if (swap_ab) {
        printf("SWAP phases A and B\n");
    } else if (swap_ca) {
        printf("SWAP phases C and A\n");
    }
}

// Get phase angles
float ab_angle = pc814_threephase_get_phase_angle(&threephase, PC814_PHASE_A, PC814_PHASE_B);
float bc_angle = pc814_threephase_get_phase_angle(&threephase, PC814_PHASE_B, PC814_PHASE_C);
printf("A-B angle: %.2f°, B-C angle: %.2f°\n", ab_angle, bc_angle);

// Get frequencies
uint32_t freq_a = pc814_threephase_get_phase_frequency(&threephase, PC814_PHASE_A);
uint32_t freq_b = pc814_threephase_get_phase_frequency(&threephase, PC814_PHASE_B);
uint32_t freq_c = pc814_threephase_get_phase_frequency(&threephase, PC814_PHASE_C);
printf("Frequencies: A=%lu Hz, B=%lu Hz, C=%lu Hz\n", freq_a, freq_b, freq_c);
```

## Pull-Up vs Pull-Down Configuration

### Pull-Up Configuration
- Use when PC814 output is active-low
- Edge type: `PC814_EDGE_FALLING`
- GPIO configured with internal pull-up resistor

### Pull-Down Configuration
- Use when PC814 output is active-high
- Edge type: `PC814_EDGE_RISING`
- GPIO configured with internal pull-down resistor

## Important Notes

1. **Timer Configuration**: Timer must be properly configured for Input Capture
2. **Pull Configuration**: Choose pull-up or pull-down based on PC814 output characteristics
3. **Edge Type**: Match edge type with pull configuration
4. **Frequency Validation**: Set appropriate tolerance for frequency validation
5. **Time Functions**: Implement accurate microsecond time function for phase calculations
6. **AC Safety**: Ensure proper isolation when connecting to AC 220V

## Use Cases

### Single-Phase Applications
- ✅ AC phase control (TRIAC, SSR control)
- ✅ Power factor correction
- ✅ AC voltage/current measurement synchronization
- ✅ Dimmer control systems
- ✅ Power monitoring systems
- ✅ Energy measurement synchronization

### Three-Phase Applications
- ✅ Three-phase motor control
- ✅ Three-phase power monitoring
- ✅ Phase sequence verification
- ✅ Phase imbalance detection
- ✅ Three-phase inverter control
- ✅ Industrial automation systems
- ✅ Power quality analysis

## Statistics and Monitoring

The library tracks zero-crossing statistics:
- Total zero-crossing count
- Line frequency measurement
- Period between zero-crossings
- Time since last zero-crossing
- Data validity status

## Error Handling

The library includes comprehensive error handling:
- Initialization validation
- Frequency validation with tolerance
- Timer capture error handling
- Data validity checking

## Performance Considerations

- **Timer Frequency**: Higher timer frequency = better accuracy
- **Prescaler**: Adjust prescaler for optimal resolution
- **Filter**: Use timer input filter to reduce noise
- **Interrupt Priority**: Set appropriate interrupt priority

## Integration with Other Systems

### TRIAC Control Example

```c
void on_zero_crossing(pc814_handle_t *handle, pc814_data_t *data)
{
    // Calculate delay for 90 degrees phase shift
    uint32_t delay_us = pc814_calc_time_for_phase(90.0f, data->frequency_hz);
    
    // Schedule TRIAC trigger after delay
    // Your TRIAC trigger code here
}

pc814_set_callback(&pc814, on_zero_crossing);
```

### Dimmer Control Example

```c
void dimmer_control(float brightness_percent)
{
    // Calculate phase angle for desired brightness
    float phase = (brightness_percent / 100.0f) * 180.0f;  // 0-180 degrees
    
    uint32_t freq = pc814_get_frequency(&pc814);
    uint32_t delay_us = pc814_calc_time_for_phase(phase, freq);
    
    // Wait for zero-crossing, then trigger after delay
    // Your dimmer control code here
}
```

## Troubleshooting

### No Zero-Crossing Detected
- Check PC814 connections
- Verify Timer Input Capture configuration
- Check GPIO pull-up/pull-down settings
- Verify edge type matches pull configuration

### Invalid Frequency
- Check expected frequency setting (50 or 60 Hz)
- Adjust frequency tolerance if needed
- Verify AC line frequency

### Timer Overflow
- Increase timer period
- Adjust prescaler for better range

## Three-Phase System Support

The library includes comprehensive support for three-phase AC systems with phase sequence detection and correction recommendations.

### Features
- ✅ **Three-Phase Detection**: Support for three PC814 units (one per phase)
- ✅ **Phase Sequence Detection**: Automatic detection of ABC (correct) or ACB (reverse) sequence
- ✅ **Phase Relationship Analysis**: Calculate phase angles between all phases
- ✅ **Frequency Measurement**: Individual frequency measurement for each phase
- ✅ **Swap Recommendations**: Automatic recommendation of which phases to swap
- ✅ **Imbalance Detection**: Calculate phase imbalance percentage
- ✅ **Synchronization Check**: Verify all phases are synchronized

### Quick Start for Three-Phase

```c
// Initialize three phases
pc814_handle_t phase_a, phase_b, phase_c;
pc814_init(&phase_a, &port_a, PC814_PULL_UP, PC814_EDGE_RISING);
pc814_init(&phase_b, &port_b, PC814_PULL_UP, PC814_EDGE_RISING);
pc814_init(&phase_c, &port_c, PC814_PULL_UP, PC814_EDGE_RISING);

// Initialize three-phase system
pc814_threephase_t threephase;
pc814_threephase_init(&threephase, &phase_a, &phase_b, &phase_c);

// Process and detect sequence
pc814_threephase_process(&threephase);
pc814_sequence_t sequence = pc814_threephase_get_sequence(&threephase);

// Check if sequence is correct
if (pc814_threephase_is_sequence_correct(&threephase)) {
    printf("Sequence is CORRECT (ABC)\n");
} else {
    printf("Sequence is REVERSE (ACB) - needs correction\n");
    
    // Get swap recommendations
    bool swap_ab, swap_bc, swap_ca;
    pc814_threephase_get_swap_recommendation(&threephase, &swap_ab, &swap_bc, &swap_ca);
    
    if (swap_bc) {
        printf("SWAP phases B and C\n");
    }
}
```

### Three-Phase Functions

- `pc814_threephase_init()`: Initialize three-phase system
- `pc814_threephase_process()`: Process all phases and detect sequence
- `pc814_threephase_detect_sequence()`: Detect phase sequence
- `pc814_threephase_get_sequence()`: Get current sequence
- `pc814_threephase_is_sequence_correct()`: Check if sequence is correct
- `pc814_threephase_get_swap_recommendation()`: Get which phases to swap
- `pc814_threephase_get_correction_message()`: Get human-readable correction message
- `pc814_threephase_get_phase_angle()`: Get angle between two phases
- `pc814_threephase_get_phase_frequency()`: Get frequency of specific phase
- `pc814_threephase_get_imbalance()`: Get phase imbalance percentage
- `pc814_threephase_is_synchronized()`: Check if all phases are synchronized

## File Structure

- `PC814.h`: Header file with all definitions and functions
- `PC814.c`: Complete library implementation (~500+ lines)
- `PC814_Example.c`: Complete usage examples with 8+ examples
- `PC814_ThreePhase.h`: Three-phase system header
- `PC814_ThreePhase.c`: Three-phase system implementation
- `PC814_ThreePhase_Example.c`: Three-phase usage examples
- `README.md`: Complete documentation
- `DESCRIPTION.md`: Short description for GitHub
- `FEATURES.md`: Complete feature list

## License

[Add your license here]

## Author

**Ehsan Zehni**

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## Acknowledgments

This library is designed for use with PC814 zero-crossing detection optocoupler in embedded systems applications.


# Quick Start Guide

**Author:** Ehsan Zehni

## Single-Phase Setup (5 minutes)

### Step 1: Copy Files
Copy `PC814.h` and `PC814.c` to your project.

### Step 2: Implement Port Functions
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

### Step 3: Initialize
```c
pc814_handle_t pc814;
pc814_init(&pc814, &pc814_port, PC814_PULL_UP, PC814_EDGE_RISING);
pc814_set_expected_frequency(&pc814, 50);  // 50Hz
pc814_start(&pc814);
```

### Step 4: Use
```c
// In Timer Input Capture callback
pc814_process_capture(&pc814);

// Read data
pc814_data_t data;
if (pc814_read_data(&pc814, &data) == PC814_OK) {
    printf("Frequency: %lu Hz\n", data.frequency_hz);
}
```

## Three-Phase Setup

### Step 1: Copy Additional Files
Copy `PC814_ThreePhase.h` and `PC814_ThreePhase.c` to your project.

### Step 2: Initialize Three Phases
```c
pc814_handle_t phase_a, phase_b, phase_c;
pc814_init(&phase_a, &port_a, PC814_PULL_UP, PC814_EDGE_RISING);
pc814_init(&phase_b, &port_b, PC814_PULL_UP, PC814_EDGE_RISING);
pc814_init(&phase_c, &port_c, PC814_PULL_UP, PC814_EDGE_RISING);
```

### Step 3: Initialize Three-Phase System
```c
pc814_threephase_t threephase;
pc814_threephase_init(&threephase, &phase_a, &phase_b, &phase_c);
```

### Step 4: Detect Sequence
```c
pc814_threephase_process(&threephase);

if (pc814_threephase_is_sequence_correct(&threephase)) {
    printf("Sequence is CORRECT (ABC)\n");
} else {
    bool swap_ab, swap_bc, swap_ca;
    pc814_threephase_get_swap_recommendation(&threephase, &swap_ab, &swap_bc, &swap_ca);
    
    if (swap_bc) printf("SWAP B and C\n");
}
```

## Complete Examples

See `PC814_Example.c` and `PC814_ThreePhase_Example.c` for complete working examples.


# PC814 Three-Phase System Guide

**Author:** Ehsan Zehni

Complete guide for using PC814 library with three-phase AC systems.

## Overview

The three-phase system support allows you to use three PC814 optocouplers (one for each phase) to detect zero-crossings and automatically determine phase sequence (ABC or ACB) and provide correction recommendations.

## Hardware Setup

### Connections

- **Phase A**: PC814 #1 output → Timer Input Capture #1
- **Phase B**: PC814 #2 output → Timer Input Capture #2  
- **Phase C**: PC814 #3 output → Timer Input Capture #3

Each PC814 should be connected to its respective phase of the three-phase AC system (220V line-to-neutral or 380V line-to-line).

### Timer Configuration

You need three timers (or one timer with three channels) configured for Input Capture:

- Timer 1: Phase A Input Capture
- Timer 2: Phase B Input Capture
- Timer 3: Phase C Input Capture

## Phase Sequence Detection

### Correct Sequence (ABC)

In correct sequence:
- Phase A leads Phase B by 120°
- Phase B leads Phase C by 120°
- Phase C leads Phase A by 120°

### Reverse Sequence (ACB)

In reverse sequence:
- Phase A leads Phase C by 120°
- Phase C leads Phase B by 120°
- Phase B leads Phase A by 120°

The library automatically detects which sequence you have and recommends which phases to swap.

## Usage Example

```c
// Initialize three phases
pc814_handle_t phase_a, phase_b, phase_c;
pc814_init(&phase_a, &port_a, PC814_PULL_UP, PC814_EDGE_RISING);
pc814_init(&phase_b, &port_b, PC814_PULL_UP, PC814_EDGE_RISING);
pc814_init(&phase_c, &port_c, PC814_PULL_UP, PC814_EDGE_RISING);

// Start all phases
pc814_start(&phase_a);
pc814_start(&phase_b);
pc814_start(&phase_c);

// Initialize three-phase system
pc814_threephase_t threephase;
pc814_threephase_init(&threephase, &phase_a, &phase_b, &phase_c);

// Process periodically
while (1) {
    pc814_threephase_process(&threephase);
    
    // Check sequence
    if (pc814_threephase_is_sequence_correct(&threephase)) {
        printf("Sequence is CORRECT (ABC)\n");
    } else {
        printf("Sequence is REVERSE (ACB)\n");
        
        // Get swap recommendation
        bool swap_ab, swap_bc, swap_ca;
        pc814_threephase_get_swap_recommendation(&threephase, 
                                                  &swap_ab, &swap_bc, &swap_ca);
        
        if (swap_bc) {
            printf("ACTION: Swap phases B and C\n");
        } else if (swap_ab) {
            printf("ACTION: Swap phases A and B\n");
        } else if (swap_ca) {
            printf("ACTION: Swap phases C and A\n");
        }
    }
    
    HAL_Delay(1000);
}
```

## Understanding Swap Recommendations

### Swap B and C
- Most common correction for reverse sequence
- Swaps phases B and C to convert ACB to ABC

### Swap A and B
- Used when phase order is BAC instead of ABC
- Swaps phases A and B

### Swap C and A
- Used when phase order is CBA instead of ABC
- Swaps phases C and A

## Phase Angle Analysis

The library calculates phase angles between all phases:

```c
pc814_phase_relationship_t rel;
pc814_threephase_get_relationship(&threephase, &rel);

printf("A-B angle: %.2f degrees\n", rel.phase_ab_angle);
printf("B-C angle: %.2f degrees\n", rel.phase_bc_angle);
printf("C-A angle: %.2f degrees\n", rel.phase_ca_angle);
```

**Expected values:**
- Correct sequence (ABC): All angles ≈ 120°
- Reverse sequence (ACB): Some angles ≈ 240° (or -120°)

## Frequency Monitoring

Monitor frequency of each phase:

```c
uint32_t freq_a = pc814_threephase_get_phase_frequency(&threephase, PC814_PHASE_A);
uint32_t freq_b = pc814_threephase_get_phase_frequency(&threephase, PC814_PHASE_B);
uint32_t freq_c = pc814_threephase_get_phase_frequency(&threephase, PC814_PHASE_C);

printf("Frequencies: A=%lu Hz, B=%lu Hz, C=%lu Hz\n", freq_a, freq_b, freq_c);

// Check synchronization
if (pc814_threephase_is_synchronized(&threephase)) {
    printf("All phases are synchronized\n");
}
```

## Phase Imbalance Detection

Detect phase imbalance:

```c
float imbalance = pc814_threephase_get_imbalance(&threephase);

if (imbalance >= 0) {
    printf("Phase imbalance: %.2f%%\n", imbalance);
    
    if (imbalance > 5.0f) {
        printf("WARNING: High phase imbalance detected!\n");
    }
}
```

## Troubleshooting

### Sequence Always Shows ERROR
- Check that all three phases are connected
- Verify all timers are working
- Check that zero-crossings are being detected for all phases
- Increase tolerance: `pc814_threephase_set_tolerance(&threephase, 15.0f)`

### Incorrect Swap Recommendation
- Verify phase connections are correct
- Check that all PC814 units are working
- Ensure timers are synchronized
- Review phase angle values manually

### Phases Not Synchronized
- Check AC line connections
- Verify all phases are from the same source
- Check for phase loss or open circuit

## Important Notes

1. **Timer Synchronization**: All three timers should use the same clock source
2. **Time Reference**: System time function must be accurate for phase angle calculations
3. **Tolerance**: Adjust tolerance based on your system accuracy requirements
4. **Safety**: Always ensure proper isolation when working with AC 220V/380V

## Complete Example

See `PC814_ThreePhase_Example.c` for complete implementation examples.


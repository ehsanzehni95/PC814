# Installation Guide

**Author:** Ehsan Zehni

## Quick Installation

1. Copy the library files to your project:
   - `PC814.h` and `PC814.c` (single-phase support)
   - `PC814_ThreePhase.h` and `PC814_ThreePhase.c` (three-phase support, optional)

2. Add the source files to your build system

3. Include the header in your code:
   ```c
   #include "PC814.h"
   // For three-phase support:
   #include "PC814_ThreePhase.h"
   ```

4. Implement the port functions (see `PC814_Example.c`)

5. Initialize and use the library

## File Organization

### Required Files (Single-Phase)
- `PC814.h` - Header file
- `PC814.c` - Implementation

### Optional Files (Three-Phase)
- `PC814_ThreePhase.h` - Three-phase header
- `PC814_ThreePhase.c` - Three-phase implementation

### Example Files
- `PC814_Example.c` - Single-phase examples
- `PC814_ThreePhase_Example.c` - Three-phase examples
- `PC814_Port_Example.c` - Port functions example (if exists)

## Dependencies

- Standard C library (stdint.h, stdbool.h, string.h, math.h)
- HAL library for your platform (STM32 HAL, etc.)
- Timer Input Capture support
- GPIO support

## Build System Integration

### STM32CubeIDE / Makefile
Add source files to your project and include paths.

### CMake
```cmake
add_library(pc814 STATIC
    PC814.c
    PC814_ThreePhase.c  # Optional
)

target_include_directories(pc814 PUBLIC .)
```

## Configuration

No special configuration required. The library uses port functions that you implement according to your hardware.

## Testing

See example files for testing and verification:
- `PC814_Example.c` - Single-phase examples
- `PC814_ThreePhase_Example.c` - Three-phase examples


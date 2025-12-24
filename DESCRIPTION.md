# Library Description

**Author:** Ehsan Zehni

**Short Description for GitHub (max 350 chars):**

PC814 zero-crossing detection library with Timer Input Capture, pull-up/pull-down config, frequency measurement, phase calculations, statistics, callbacks. Includes three-phase support with automatic phase sequence detection (ABC/ACB) and swap recommendations. For AC 220V single/three-phase applications.

**Detailed Description:**

A comprehensive, production-ready C library for interfacing with PC814 zero-crossing detection optocoupler. The library provides Timer Input Capture support for accurate zero-crossing detection from AC 220V lines, configurable pull-up/pull-down GPIO settings, automatic line frequency measurement (50/60 Hz), period calculation between zero-crossings, phase angle calculations, and callback functions for zero-crossing events.

**Key Features:**
- Timer Input Capture for accurate zero-crossing detection
- Configurable pull-up/pull-down GPIO settings
- Automatic line frequency measurement and validation
- Period calculation between zero-crossings
- Phase angle calculations from time offset
- Time offset calculation for desired phase angle
- Zero-crossing count tracking with statistics
- Frequency validation with configurable tolerance
- Complete statistics (min, max, average period and frequency)
- Callback functions for zero-crossing events
- Blocking wait functions for synchronization
- Helper functions for common phase calculations (90°, 180°)
- Modular architecture for easy portability

**Target Platforms:**
- STM32 (HAL)
- Other ARM Cortex-M microcontrollers
- Any platform with Timer Input Capture and GPIO support

**Use Cases:**
- Single-phase AC phase control (TRIAC, SSR control)
- Power factor correction
- AC voltage/current measurement synchronization
- Dimmer control systems
- **Three-phase motor control**
- **Three-phase power monitoring**
- **Phase sequence verification and correction**
- **Phase imbalance detection**
- Power monitoring systems
- Energy measurement synchronization
- Industrial automation systems


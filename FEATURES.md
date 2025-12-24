# PC814 Library Features

**Author:** Ehsan Zehni  
**Version:** 1.0

## ✅ Implemented Features

### 🔧 Single-Phase Core Features
- [x] Timer Input Capture for zero-crossing detection
- [x] Configurable pull-up/pull-down GPIO settings
- [x] Automatic line frequency measurement (50/60 Hz)
- [x] Period calculation between zero-crossings
- [x] Zero-crossing count tracking
- [x] Phase angle calculations
- [x] Frequency validation with tolerance
- [x] Modular and portable architecture

### 📊 Data Reading
- [x] Read zero-crossing data structure
- [x] Get line frequency
- [x] Get period in microseconds
- [x] Get zero-crossing count
- [x] Get time since last zero-crossing
- [x] Data validity checking

### 🎯 Configuration
- [x] Pull-up configuration
- [x] Pull-down configuration
- [x] Rising edge detection
- [x] Falling edge detection
- [x] Expected frequency setting (50/60 Hz)
- [x] Frequency tolerance setting

### 📈 Statistics and Monitoring
- [x] Total zero-crossing count
- [x] Valid zero-crossing count
- [x] Invalid zero-crossing count
- [x] Minimum period tracking
- [x] Maximum period tracking
- [x] Average period calculation
- [x] Minimum frequency tracking
- [x] Maximum frequency tracking
- [x] Average frequency calculation
- [x] Reset statistics

### 🔔 Callback and Events
- [x] Zero-crossing callback function
- [x] Automatic callback on valid zero-crossing
- [x] Asynchronous processing support

### ⚙️ Advanced Calculations
- [x] Phase angle calculation from time offset
- [x] Time offset calculation for desired phase angle
- [x] Half period calculation (180°)
- [x] Quarter period calculation (90°)

### 🛡️ Error Handling
- [x] Initialization validation
- [x] Frequency validation with tolerance
- [x] Timer capture error handling
- [x] Data validity checking
- [x] Timeout handling

### 🔧 Utility Functions
- [x] Blocking wait for zero-crossing
- [x] Non-blocking zero-crossing check
- [x] Start/stop detection
- [x] Reset handle and statistics

### ⚡ Three-Phase System Features
- [x] Three-phase system support (three PC814 units)
- [x] Phase sequence detection (ABC/ACB)
- [x] Phase relationship analysis
- [x] Individual phase frequency measurement
- [x] Phase angle calculation between phases
- [x] Automatic swap recommendations
- [x] Phase imbalance detection
- [x] Synchronization verification
- [x] Correction message generation

## 📋 Complete Function List

### Basic Functions (6 functions)
1. `pc814_init()` - Initialize with pull-up/pull-down
2. `pc814_process_capture()` - Process Input Capture
3. `pc814_read_data()` - Read zero-crossing data
4. `pc814_start()` - Start detection
5. `pc814_stop()` - Stop detection
6. `pc814_reset()` - Reset handle

### Data Reading Functions (5 functions)
7. `pc814_get_frequency()` - Get line frequency
8. `pc814_get_period_us()` - Get period
9. `pc814_get_count()` - Get zero-crossing count
10. `pc814_get_time_since_zc()` - Get time since last ZC
11. `pc814_is_data_valid()` - Check data validity

### Configuration Functions (3 functions)
12. `pc814_set_expected_frequency()` - Set expected frequency
13. `pc814_set_frequency_tolerance()` - Set tolerance
14. `pc814_set_callback()` - Set callback function

### Calculation Functions (4 functions)
15. `pc814_calc_phase_angle()` - Calculate phase angle
16. `pc814_calc_time_for_phase()` - Calculate time for phase
17. `pc814_get_half_period_us()` - Get half period
18. `pc814_get_quarter_period_us()` - Get quarter period

### Statistics Functions (2 functions)
19. `pc814_get_statistics()` - Get statistics
20. `pc814_reset_statistics()` - Reset statistics

### Utility Functions (2 functions)
21. `pc814_wait_for_zc()` - Wait for zero-crossing
22. `pc814_is_new_zc()` - Check for new zero-crossing

### Three-Phase Functions (11+ functions)
23. `pc814_threephase_init()` - Initialize three-phase system
24. `pc814_threephase_process()` - Process all phases
25. `pc814_threephase_detect_sequence()` - Detect sequence
26. `pc814_threephase_get_sequence()` - Get sequence
27. `pc814_threephase_is_sequence_correct()` - Check if correct
28. `pc814_threephase_get_swap_recommendation()` - Get swap recommendation
29. `pc814_threephase_get_correction_message()` - Get correction message
30. `pc814_threephase_get_phase_angle()` - Get phase angle
31. `pc814_threephase_get_phase_frequency()` - Get phase frequency
32. `pc814_threephase_get_imbalance()` - Get imbalance
33. `pc814_threephase_is_synchronized()` - Check synchronization

**Total: 33+ comprehensive functions**

## 🎯 Use Cases

- ✅ AC phase control (TRIAC, SSR control)
- ✅ Power factor correction
- ✅ AC voltage/current measurement synchronization
- ✅ Dimmer control systems
- ✅ Motor control applications
- ✅ Power monitoring systems
- ✅ Energy measurement synchronization
- ✅ AC waveform analysis
- ✅ Phase-locked loop (PLL) applications

## 📊 Library Statistics

- **Lines of Code**: ~1000+ lines (single-phase + three-phase)
- **Functions**: 33+ functions
- **Data Structures**: 5+ structures
- **Examples**: 10+ complete examples
- **Documentation**: Complete in English
- **Three-Phase Support**: Complete with sequence detection

## ✨ Highlights

1. **Comprehensive**: Complete coverage of zero-crossing detection needs
2. **Flexibility**: Configurable pull-up/pull-down and edge detection
3. **Reliability**: Complete error management and validation
4. **Efficiency**: Optimized for microcontrollers
5. **Documentation**: Complete documentation in English
6. **Examples**: Practical and comprehensive examples


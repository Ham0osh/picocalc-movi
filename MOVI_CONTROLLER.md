# Movi Pro Gimbal Controller for PicoCalc

## Overview
This is a complete Movi Pro gimbal control system for the Raspberry Pi Pico-based PicoCalc, enabling real-time gimbal and FIZ lens control via the FreeFly QX Protocol over UART.

## Architecture

### Core Modules

#### 1. **movi_controller** (movi_controller.h/c)
High-level gimbal control interface that:
- Wraps the FreeFly API with Pico-specific functionality
- Manages three control modes:
  - **RATE**: Velocity-based pan/tilt control (joystick-like)
  - **POSITION**: Absolute gimbal angle control
  - **LENS**: Focus/Iris/Zoom control
- Handles gimbal kill state, recording, fault clearing, and auto-calibration
- Provides telemetry getters (battery voltage, quaternions, lens positions, states)

#### 2. **qx_uart** (qx_uart.h/c)
UART communication layer:
- Initializes UART0 at 111111 baud (gimbal control port)
- Bridges FreeFly API's simple buffer system with Pico UART hardware
- `qx_uart_send_pending()`: Drains transmit buffer to UART
- `qx_uart_recv_pending()`: Feeds UART bytes through QX protocol state machine

#### 3. **movi_input** (movi_input.h/c)
Input handling system:
- Maps PicoCalc D-pad keyboard to gimbal control:
  - `KEY_UP/DOWN`: Tilt control
  - `KEY_LEFT/RIGHT`: Pan control
  - `KEY_F1-F5`: Mode switch, gimbal kill, record, clear faults, auto-calibrate
- Applies deadband to prevent drift
- Converts keyboard events to control inputs

#### 4. **movi_display** (movi_display.h/c)
Status display with ANSI formatting:
- Real-time telemetry display (~1Hz update)
- Shows:
  - Current control mode
  - Battery voltages (left/right)
  - Gimbal quaternion (rotation state)
  - Lens positions and states (Focus/Iris/Zoom)
  - Range limit active status
  - Camera recording status
- Control legend for user reference

### Integration Points

#### main.c
- Replaced REPL with 50Hz gimbal control loop
- Initialization sequence:
  1. Hardware init (picocalc, keyboard, LED)
  2. Movi controller init (UART, FreeFly API)
  3. Input and display init
  4. Main loop: receive UART, update display, check keyboard, send controls

#### CMakeLists.txt
Added:
- Movi controller source files
- FreeFly API source files (QX_Protocol, QX_Parsing_Functions, simple_buffer)
- UART communication layer

## Control Flow

### 50Hz Update Cycle
```
Loop iteration (every 20ms):
  1. qx_uart_recv_pending()   - Process incoming gimbal status
  2. movi_update()            - Send control commands
  3. movi_input_update()      - Poll inputs
  4. movi_display_update()    - Update telemetry display (~1sec)
  5. Check keyboard events and call movi_input_handle_key()
```

### Command Transmission
1. User adjusts D-pad → `movi_input_handle_key()`
2. Input mapped to control values → `movi_set_control_input()`
3. FreeFly API structure updated with control values
4. `FreeflyAPI.send()` called → populates QX transmit buffer
5. `qx_uart_send_pending()` → extracts bytes and sends via UART

### Telemetry Reception
1. Gimbal sends status packet over UART
2. `qx_uart_recv_pending()` → feeds bytes to QX protocol state machine
3. QX parser populates `FreeflyAPI.status` structure
4. Getter functions access status data for display

## D-Pad Control Mapping

| Input | Action |
|-------|--------|
| D-Pad Up | Tilt gimbal up (positive) |
| D-Pad Down | Tilt gimbal down (negative) |
| D-Pad Left | Pan gimbal left (negative) |
| D-Pad Right | Pan gimbal right (positive) |
| F1 | Cycle control mode |
| F2 | Toggle gimbal kill |
| F3 | Record button |
| F4 | Clear FIZ faults |
| F5 | Auto-calibrate FIZ axes |

## Control Modes

### Mode 0: Gimbal Rate (Pan/Tilt)
- D-pad controls gimbal velocity
- Value range: -1.0 to +1.0 (normalized velocity)
- Deadband threshold: 0.07

### Mode 1: Gimbal Position (Pan/Tilt)
- D-pad controls absolute gimbal angle
- Value range: -1.0 to +1.0 (normalized angle)
- No deadband for precise positioning

### Mode 2: Lens Control (Focus/Iris/Zoom)
- D-pad controls lens positions
- Up/Down: Focus position
- Left/Right: Iris position (ideally would use separate axis)

## Status Display

The display refreshes approximately every 1 second and shows:
- **Mode**: Current control mode (for user feedback)
- **Gimbal Kill**: ON/OFF state
- **Battery**: Left and right channel voltages
- **Gimbal Position**: Quaternion components (i, j, k, r)
- **Lens Control**: Position counts and states for Focus, Iris, Zoom
- **Range Limits**: Active/inactive status for lens range limits
- **Camera**: Recording ON/OFF
- **Control Legend**: Key mappings for reference

## UART Configuration

- **Baud Rate**: 111111 (gimbal control)
- **Data Bits**: 8
- **Stop Bits**: 1
- **Parity**: None
- **Hardware**: UART0 on Pico (GP0=TX, GP1=RX)

## API Data Structures

### Control Input
```c
typedef struct {
    float pan, tilt, roll;      // Gimbal axes
    float focus, iris, zoom;    // Lens axes
} movi_control_input_t;
```

### Status (from FreeFly API)
```c
typedef struct {
    float battery_v_left, battery_v_right;
    float gimbal_i, gimbal_j, gimbal_k, gimbal_r;  // Quaternion
    uint16_t focus_position, iris_position, zoom_position;
    Lens_Axis_State_General_e focus_state, iris_state, zoom_state;
    uint8_t focus_range_limits_active, iris_range_limits_active, zoom_range_limits_active;
    uint8_t camera_recording;
} ff_api_status_t;
```

## Future Enhancements

1. **Better Lens Control Mapping**: Currently uses D-pad; could add:
   - Axis selection with modifier buttons
   - Analog joystick input if available
   - Direct focus/iris/zoom buttons

2. **Advanced Display**:
   - Animated gimbal position visualization
   - Euler angle display (converted from quaternion)
   - Battery level indicator
   - Connection status indicator

3. **Configuration**:
   - Configurable control parameters (max rates, deadbands)
   - Persist settings to SD card
   - Control profile switching

4. **Extended Features**:
   - Motion profile recording/playback
   - Gimbal limit configuration UI
   - Telemetry logging to SD card
   - Live FPS counter for gimbal

5. **Safety**:
   - Command validation and rate limiting
   - Watchdog timer for UART connection
   - Automatic gimbal kill on communication loss
   - User confirmation for critical operations

## Building and Running

### Prerequisites
- Raspberry Pi Pico or compatible board
- PicoCalc hardware with display and keyboard
- Movi Pro gimbal with UART interface
- Pico SDK 2.2.0+

### Build
```bash
cd /workspaces/picocalc-movi
mkdir -p build
cd build
cmake ..
make
```

### Flash
```bash
# Using picotool
picotool load -fx build/picocalc-text-starter.uf2
```

## Testing Notes

1. **Without Gimbal**: Program will still compile and run; telemetry will show default/zero values
2. **UART Connection**: Verify gimbal is at 111111 baud before connecting
3. **Keyboard Input**: Test D-pad by checking mode switching and gimbal kill toggle
4. **Display Update**: Should refresh approximately every 1 second

## Known Limitations

1. Lens control mapping to D-pad is simplified (single axis per direction)
2. Display updates at ~1Hz due to formatting overhead (can be optimized)
3. No watchdog for UART connection loss yet
4. Quaternion display is raw values (could be converted to Euler angles)
5. Recording button requires sustained press (momentary press not supported)

## Files Modified/Created

**New Files:**
- `movi_controller.h/c` - Main gimbal control interface
- `movi_input.h/c` - D-pad input mapping
- `movi_display.h/c` - Telemetry UI
- `qx_uart.h/c` - UART communication layer

**Modified Files:**
- `main.c` - Replaced REPL with gimbal control loop
- `CMakeLists.txt` - Added new source files and FreeFly API

## References

- FreeFly QX Protocol API (v1.0)
- Raspberry Pi Pico SDK Documentation
- Movi Pro Serial Interface Specification (111111 baud, QX277/QX287 packets)

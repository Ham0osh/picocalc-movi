/*
 * Movi Pro Gimbal Controller for PicoCalc
 * 
 * High-level interface to control Movi Pro gimbal via FreeFly API
 * Handles D-pad input mapping, UART communication, and control modes
 */

#ifndef MOVI_CONTROLLER_H
#define MOVI_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>

// Control modes
typedef enum {
    MOVI_MODE_GIMBAL_RATE = 0,      // Pan/Tilt rate control
    MOVI_MODE_GIMBAL_POSITION = 1,  // Pan/Tilt absolute position
    MOVI_MODE_LENS_CONTROL = 2,     // Focus/Iris/Zoom control
    MOVI_MODE_COUNT = 3
} movi_control_mode_t;

// Gimbal control input
typedef struct {
    float pan;                       // -1.0 to +1.0
    float tilt;                      // -1.0 to +1.0
    float roll;                      // -1.0 to +1.0
    float focus;                     // -1.0 to +1.0
    float iris;                      // -1.0 to +1.0
    float zoom;                      // -1.0 to +1.0
} movi_control_input_t;

// Button events
typedef enum {
    MOVI_BTN_NONE = 0,
    MOVI_BTN_MODE_CYCLE,            // Cycle through control modes
    MOVI_BTN_GIMBAL_KILL,           // Toggle gimbal kill
    MOVI_BTN_RECORD,                // Start/stop recording
    MOVI_BTN_CLEAR_FAULTS,          // Clear FIZ faults
    MOVI_BTN_AUTO_CAL,              // Auto-calibrate FIZ
} movi_button_event_t;

/*---------------------------------------------------------------------------
 * Public API
 *---------------------------------------------------------------------------*/

/**
 * Initialize the Movi controller
 * Must be called before any other movi_* functions
 */
void movi_init(void);

/**
 * Update the controller - call this every 20ms (50Hz)
 * Handles UART communication with gimbal
 */
void movi_update(void);

/**
 * Set control inputs from D-pad/buttons
 */
void movi_set_control_input(const movi_control_input_t *input);

/**
 * Handle button events (mode switch, gimbal kill, etc)
 */
void movi_handle_button_event(movi_button_event_t event);

/**
 * Get current control mode
 */
movi_control_mode_t movi_get_control_mode(void);

/**
 * Set recording button state (held down or released)
 */
void movi_set_record_button(bool pressed);

/**
 * Set clear faults button state
 */
void movi_set_clear_faults_button(bool pressed);

/**
 * Set auto-calibrate button state
 */
void movi_set_autocal_button(bool pressed);

/**
 * Get gimbal kill state
 */
bool movi_get_gimbal_kill_state(void);

/**
 * Get gimbal battery voltage (left channel)
 */
float movi_get_battery_voltage_left(void);

/**
 * Get gimbal battery voltage (right channel)
 */
float movi_get_battery_voltage_right(void);

/**
 * Get gimbal quaternion components (for rotation)
 */
void movi_get_gimbal_quaternion(float *i, float *j, float *k, float *r);

/**
 * Get focus lens position (0-65535 counts)
 */
uint16_t movi_get_focus_position(void);

/**
 * Get iris lens position (0-65535 counts)
 */
uint16_t movi_get_iris_position(void);

/**
 * Get zoom lens position (0-65535 counts)
 */
uint16_t movi_get_zoom_position(void);

/**
 * Get lens axis state as string (for display)
 */
const char *movi_get_lens_state_string(uint8_t lens_state);

/**
 * Get focus state
 */
uint8_t movi_get_focus_state(void);

/**
 * Get iris state
 */
uint8_t movi_get_iris_state(void);

/**
 * Get zoom state
 */
uint8_t movi_get_zoom_state(void);

/**
 * Get camera recording status
 */
bool movi_get_camera_recording(void);

/**
 * Get lens range limits active flags
 */
void movi_get_range_limits(bool *focus_active, bool *iris_active, bool *zoom_active);

#endif // MOVI_CONTROLLER_H

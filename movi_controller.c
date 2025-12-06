/*
 * Movi Pro Gimbal Controller Implementation
 */

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "pico/stdlib.h"

#include "movi_controller.h"
#include "qx_uart.h"
#include "api/freefly/QX_Protocol_App.h"
#include "api/freefly/QX_Protocol.h"

/*---------------------------------------------------------------------------
 * Private state
 *---------------------------------------------------------------------------*/

static movi_control_mode_t current_mode = MOVI_MODE_GIMBAL_RATE;
static bool gimbal_kill = false;
static uint32_t update_counter = 0;

// Deadband threshold for joystick inputs
#define DEADBAND_THRESHOLD 0.07f

/*---------------------------------------------------------------------------
 * Helper functions
 *---------------------------------------------------------------------------*/

/**
 * Apply deadband to joystick input
 */
static float apply_deadband(float value)
{
    if (value > DEADBAND_THRESHOLD || value < -DEADBAND_THRESHOLD) {
        return value;
    }
    return 0.0f;
}

/**
 * Clamp float to [-1.0, 1.0]
 */
static float clamp_float(float value)
{
    if (value > 1.0f) return 1.0f;
    if (value < -1.0f) return -1.0f;
    return value;
}



/**
 * Lens state to string conversion
 */
static const char *lens_state_to_string(Lens_Axis_State_General_e state)
{
    switch (state) {
        case Lens_AxisState_Disabled:
            return "Disabled";
        case Lens_AxisState_Reset:
            return "Reset";
        case Lens_AxisState_Faulted:
            return "Faulted";
        case Lens_AxisState_Move_to_Command:
            return "Moving";
        case Lens_AxisState_Calibrated:
            return "Calibrated";
        case Lens_AxisState_Uncalibrated:
            return "Uncalibrated";
        case Lens_AxisState_Man_Cal_Set_Max:
            return "Man Cal Max";
        case Lens_AxisState_Man_Cal_Set_Min:
            return "Man Cal Min";
        case Lens_AxisState_Auto_Cal_SensingTorque:
            return "Auto Cal Torque";
        case Lens_AxisState_Auto_Cal_Set_Max:
            return "Auto Cal Max";
        case Lens_AxisState_Auto_Cal_Set_Min:
            return "Auto Cal Min";
        default:
            return "Unknown";
    }
}

/*---------------------------------------------------------------------------
 * Public API Implementation
 *---------------------------------------------------------------------------*/

void movi_init(void)
{
    // Initialize UART for gimbal communication
    qx_uart_init();
    
    // Initialize the Freefly API
    FreeflyAPI.begin();
    
    // Set default control mode
    current_mode = MOVI_MODE_GIMBAL_RATE;
    gimbal_kill = false;
    
    printf("Movi controller initialized\n");
}

void movi_update(void)
{
    // Process incoming UART data (QX Protocol)
    qx_uart_recv_pending();
    
    // Send control packet at 50Hz
    if ((update_counter % 1) == 0) {
        FreeflyAPI.send();
        qx_uart_send_pending();
    }
    
    update_counter++;
}

void movi_set_control_input(const movi_control_input_t *input)
{
    if (!input) return;
    
    // Apply deadband
    float pan = apply_deadband(input->pan);
    float tilt = apply_deadband(input->tilt);
    float roll = apply_deadband(input->roll);
    float focus = apply_deadband(input->focus);
    float iris = apply_deadband(input->iris);
    float zoom = apply_deadband(input->zoom);
    
    // Update control based on current mode
    switch (current_mode) {
        case MOVI_MODE_GIMBAL_RATE:
            FreeflyAPI.control.pan.type = RATE;
            FreeflyAPI.control.tilt.type = RATE;
            FreeflyAPI.control.roll.type = DEFER;
            FreeflyAPI.control.focus.type = DEFER;
            FreeflyAPI.control.iris.type = DEFER;
            FreeflyAPI.control.zoom.type = DEFER;
            
            FreeflyAPI.control.pan.value = pan;
            FreeflyAPI.control.tilt.value = tilt;
            FreeflyAPI.control.roll.value = 0.0f;
            FreeflyAPI.control.focus.value = 0.0f;
            FreeflyAPI.control.iris.value = 0.0f;
            FreeflyAPI.control.zoom.value = 0.0f;
            break;
            
        case MOVI_MODE_GIMBAL_POSITION:
            FreeflyAPI.control.pan.type = ABSOLUTE;
            FreeflyAPI.control.tilt.type = ABSOLUTE;
            FreeflyAPI.control.roll.type = DEFER;
            FreeflyAPI.control.focus.type = DEFER;
            FreeflyAPI.control.iris.type = DEFER;
            FreeflyAPI.control.zoom.type = DEFER;
            
            // Use raw input without deadband for position control
            FreeflyAPI.control.pan.value = input->pan;
            FreeflyAPI.control.tilt.value = input->tilt;
            FreeflyAPI.control.roll.value = 0.0f;
            FreeflyAPI.control.focus.value = 0.0f;
            FreeflyAPI.control.iris.value = 0.0f;
            FreeflyAPI.control.zoom.value = 0.0f;
            break;
            
        case MOVI_MODE_LENS_CONTROL:
            FreeflyAPI.control.pan.type = DEFER;
            FreeflyAPI.control.tilt.type = DEFER;
            FreeflyAPI.control.roll.type = DEFER;
            FreeflyAPI.control.focus.type = ABSOLUTE;
            FreeflyAPI.control.iris.type = ABSOLUTE;
            FreeflyAPI.control.zoom.type = ABSOLUTE;
            
            FreeflyAPI.control.pan.value = 0.0f;
            FreeflyAPI.control.tilt.value = 0.0f;
            FreeflyAPI.control.roll.value = 0.0f;
            FreeflyAPI.control.focus.value = focus;
            FreeflyAPI.control.iris.value = iris;
            FreeflyAPI.control.zoom.value = zoom;
            break;
    }
}

void movi_handle_button_event(movi_button_event_t event)
{
    switch (event) {
        case MOVI_BTN_MODE_CYCLE:
            current_mode = (current_mode + 1) % MOVI_MODE_COUNT;
            printf("Mode switched to: %d\n", current_mode);
            break;
            
        case MOVI_BTN_GIMBAL_KILL:
            gimbal_kill = !gimbal_kill;
            FreeflyAPI.control.gimbal_kill = gimbal_kill ? 1 : 0;
            printf("Gimbal kill: %s\n", gimbal_kill ? "ON" : "OFF");
            break;
            
        case MOVI_BTN_RECORD:
            // Handled via movi_set_record_button()
            break;
            
        case MOVI_BTN_CLEAR_FAULTS:
            FreeflyAPI.control.fiz_clearFaults_all_flag = 1;
            printf("Clearing FIZ faults...\n");
            break;
            
        case MOVI_BTN_AUTO_CAL:
            FreeflyAPI.control.fiz_autoCalStart_all_flag = 1;
            printf("Starting FIZ auto-calibration...\n");
            break;
            
        default:
            break;
    }
}

movi_control_mode_t movi_get_control_mode(void)
{
    return current_mode;
}

void movi_set_record_button(bool pressed)
{
    FreeflyAPI.control.fiz_record_button_flag = pressed ? 1 : 0;
}

void movi_set_clear_faults_button(bool pressed)
{
    FreeflyAPI.control.fiz_clearFaults_all_flag = pressed ? 1 : 0;
}

void movi_set_autocal_button(bool pressed)
{
    FreeflyAPI.control.fiz_autoCalStart_all_flag = pressed ? 1 : 0;
}

bool movi_get_gimbal_kill_state(void)
{
    return gimbal_kill;
}

float movi_get_battery_voltage_left(void)
{
    return FreeflyAPI.status.battery_v_left;
}

float movi_get_battery_voltage_right(void)
{
    return FreeflyAPI.status.battery_v_right;
}

void movi_get_gimbal_quaternion(float *i, float *j, float *k, float *r)
{
    if (i) *i = FreeflyAPI.status.gimbal_i;
    if (j) *j = FreeflyAPI.status.gimbal_j;
    if (k) *k = FreeflyAPI.status.gimbal_k;
    if (r) *r = FreeflyAPI.status.gimbal_r;
}

uint16_t movi_get_focus_position(void)
{
    return FreeflyAPI.status.focus_position;
}

uint16_t movi_get_iris_position(void)
{
    return FreeflyAPI.status.iris_position;
}

uint16_t movi_get_zoom_position(void)
{
    return FreeflyAPI.status.zoom_position;
}

const char *movi_get_lens_state_string(uint8_t lens_state)
{
    return lens_state_to_string((Lens_Axis_State_General_e)lens_state);
}

uint8_t movi_get_focus_state(void)
{
    return FreeflyAPI.status.focus_state;
}

uint8_t movi_get_iris_state(void)
{
    return FreeflyAPI.status.iris_state;
}

uint8_t movi_get_zoom_state(void)
{
    return FreeflyAPI.status.zoom_state;
}

bool movi_get_camera_recording(void)
{
    return FreeflyAPI.status.camera_recording != 0;
}

void movi_get_range_limits(bool *focus_active, bool *iris_active, bool *zoom_active)
{
    if (focus_active) *focus_active = FreeflyAPI.status.focus_range_limits_active != 0;
    if (iris_active) *iris_active = FreeflyAPI.status.iris_range_limits_active != 0;
    if (zoom_active) *zoom_active = FreeflyAPI.status.zoom_range_limits_active != 0;
}

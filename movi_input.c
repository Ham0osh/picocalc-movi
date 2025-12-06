/*
 * Movi Pro D-Pad Input Handler
 * 
 * Maps PicoCalc keyboard D-pad to gimbal control
 */

#include <stdio.h>
#include <string.h>

#include "movi_input.h"
#include "movi_controller.h"
#include "drivers/keyboard.h"

/*---------------------------------------------------------------------------
 * Input state
 *---------------------------------------------------------------------------*/

static movi_control_input_t current_input = {0};
static uint8_t last_button_state[MOVI_INPUT_NUM_BUTTONS] = {0};

/*---------------------------------------------------------------------------
 * Button debounce counters
 *---------------------------------------------------------------------------*/

#define DEBOUNCE_CYCLES 5

static uint8_t debounce_mode_switch = 0;
static uint8_t debounce_gimbal_kill = 0;
static uint8_t debounce_record = 0;
static uint8_t debounce_clear_faults = 0;
static uint8_t debounce_autocal = 0;

/*---------------------------------------------------------------------------
 * Helper functions
 *---------------------------------------------------------------------------*/

/**
 * Detect rising edge on debounced button
 * Returns true if button just pressed
 */
static bool debounce_button(uint8_t *counter, bool pressed, uint8_t threshold)
{
    if (pressed) {
        (*counter)++;
        if (*counter >= threshold) {
            *counter = threshold;
            return false;  // Already was pressed
        }
    } else {
        (*counter)--;
        if (*counter == 0) {
            return false;  // Already was released
        }
    }
    
    // Check for rising edge (transition from not-pressed to pressed)
    return pressed && (*counter == threshold);
}

/*---------------------------------------------------------------------------
 * Public API
 *---------------------------------------------------------------------------*/

void movi_input_init(void)
{
    // Initialize keyboard
    keyboard_init();
    keyboard_set_background_poll(true);
    
    printf("Input handler initialized\n");
}

void movi_input_update(void)
{
    // Update analog inputs (D-pad is digital)
    // D-Pad mapping:
    // - KEY_UP (0xB5) -> Tilt up (positive)
    // - KEY_DOWN (0xB6) -> Tilt down (negative)
    // - KEY_LEFT (0xB4) -> Pan left (negative)
    // - KEY_RIGHT (0xB7) -> Pan right (positive)
    
    // Check keyboard for pressed keys
    bool dpad_up = false;
    bool dpad_down = false;
    bool dpad_left = false;
    bool dpad_right = false;
    
    // For this implementation, we'll check for key states
    // The keyboard driver provides synchronous key polling
    // We'll handle this in a real polling loop
    
    // Default: clear inputs if no keys pressed
    current_input.pan = 0.0f;
    current_input.tilt = 0.0f;
    current_input.roll = 0.0f;
    current_input.focus = 0.0f;
    current_input.iris = 0.0f;
    current_input.zoom = 0.0f;
    
    // Set control input
    movi_set_control_input(&current_input);
}

void movi_input_handle_key(char key)
{
    // Handle D-pad keys
    switch (key) {
        case KEY_UP:
            current_input.tilt = 1.0f;  // Tilt up
            break;
        case KEY_DOWN:
            current_input.tilt = -1.0f;  // Tilt down
            break;
        case KEY_LEFT:
            current_input.pan = -1.0f;  // Pan left
            break;
        case KEY_RIGHT:
            current_input.pan = 1.0f;  // Pan right
            break;
        
        // Function buttons
        case KEY_F1:
            movi_handle_button_event(MOVI_BTN_MODE_CYCLE);
            break;
        case KEY_F2:
            movi_handle_button_event(MOVI_BTN_GIMBAL_KILL);
            break;
        case KEY_F3:
            movi_set_record_button(true);
            break;
        case KEY_F4:
            movi_set_clear_faults_button(true);
            break;
        case KEY_F5:
            movi_set_autocal_button(true);
            break;
        
        default:
            break;
    }
    
    // Update controller with new inputs
    movi_set_control_input(&current_input);
}

const movi_control_input_t *movi_input_get_current(void)
{
    return &current_input;
}

void movi_input_reset_inputs(void)
{
    current_input.pan = 0.0f;
    current_input.tilt = 0.0f;
    current_input.roll = 0.0f;
    current_input.focus = 0.0f;
    current_input.iris = 0.0f;
    current_input.zoom = 0.0f;
}

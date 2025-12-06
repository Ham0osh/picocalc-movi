/*
 * Movi Pro Input Handler Header
 */

#ifndef MOVI_INPUT_H
#define MOVI_INPUT_H

#include "movi_controller.h"

#define MOVI_INPUT_NUM_BUTTONS 5

/**
 * Initialize input handling
 */
void movi_input_init(void);

/**
 * Update input state - call this regularly from main loop
 */
void movi_input_update(void);

/**
 * Handle a keyboard key press
 */
void movi_input_handle_key(char key);

/**
 * Get current control input
 */
const movi_control_input_t *movi_input_get_current(void);

/**
 * Reset all inputs to zero
 */
void movi_input_reset_inputs(void);

#endif // MOVI_INPUT_H

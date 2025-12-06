/*
 * Movi Pro Display Header
 */

#ifndef MOVI_DISPLAY_H
#define MOVI_DISPLAY_H

/**
 * Initialize the display
 */
void movi_display_init(void);

/**
 * Update the display with current telemetry
 * Call this every loop iteration
 */
void movi_display_update(void);

/**
 * Clear the display
 */
void movi_display_clear(void);

/**
 * Print a status message
 */
void movi_display_status(const char *status);

/**
 * Print an error message
 */
void movi_display_error(const char *error);

#endif // MOVI_DISPLAY_H

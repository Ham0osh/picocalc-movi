/*
 * Movi Pro Display - Telemetry UI
 * 
 * Displays gimbal status and telemetry on the PicoCalc display
 */

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "movi_display.h"
#include "movi_controller.h"

/*---------------------------------------------------------------------------
 * Display constants
 *---------------------------------------------------------------------------*/

#define ANSI_CLEAR          "\033c"
#define ANSI_HOME           "\033[H"
#define ANSI_BOLD           "\033[1m"
#define ANSI_RESET          "\033[0m"
#define ANSI_INVERSE        "\033[7m"

/*---------------------------------------------------------------------------
 * Display state
 *---------------------------------------------------------------------------*/

static uint32_t display_update_count = 0;

/*---------------------------------------------------------------------------
 * Display functions
 *---------------------------------------------------------------------------*/

void movi_display_init(void)
{
    // Clear screen
    printf("%s", ANSI_CLEAR);
    display_update_count = 0;
}

void movi_display_update(void)
{
    // Update display every ~1 second (adjust counter as needed)
    if (display_update_count++ < 50) {
        return;
    }
    display_update_count = 0;
    
    // Clear and go home
    printf("%s%s", ANSI_CLEAR, ANSI_HOME);
    
    // Title
    printf("%s=== MOVI PRO GIMBAL CONTROLLER ===%s\n\n", ANSI_BOLD, ANSI_RESET);
    
    // Mode display
    const char *mode_name = "Unknown";
    switch (movi_get_control_mode()) {
        case MOVI_MODE_GIMBAL_RATE:
            mode_name = "Gimbal Rate (Pan/Tilt)";
            break;
        case MOVI_MODE_GIMBAL_POSITION:
            mode_name = "Gimbal Position (Pan/Tilt)";
            break;
        case MOVI_MODE_LENS_CONTROL:
            mode_name = "Lens Control (Focus/Iris/Zoom)";
            break;
        default:
            break;
    }
    printf("Mode: %s%s%s\n", ANSI_INVERSE, mode_name, ANSI_RESET);
    printf("Gimbal Kill: %s\n\n", movi_get_gimbal_kill_state() ? "ON" : "OFF");
    
    // Battery status
    printf("%s--- BATTERY ---%s\n", ANSI_BOLD, ANSI_RESET);
    float bat_left = movi_get_battery_voltage_left();
    float bat_right = movi_get_battery_voltage_right();
    printf("Left:  %.2f V  Right: %.2f V\n\n", bat_left, bat_right);
    
    // Gimbal status (quaternion)
    printf("%s--- GIMBAL POSITION (Quaternion) ---%s\n", ANSI_BOLD, ANSI_RESET);
    float qi, qj, qk, qr;
    movi_get_gimbal_quaternion(&qi, &qj, &qk, &qr);
    printf("i: % .4f  j: % .4f\n", qi, qj);
    printf("k: % .4f  r: % .4f\n\n", qk, qr);
    
    // Lens status
    printf("%s--- LENS CONTROL ---%s\n", ANSI_BOLD, ANSI_RESET);
    printf("Focus: %5d counts  State: %s\n", 
           movi_get_focus_position(), 
           movi_get_lens_state_string(movi_get_focus_state()));
    printf("Iris:  %5d counts  State: %s\n", 
           movi_get_iris_position(), 
           movi_get_lens_state_string(movi_get_iris_state()));
    printf("Zoom:  %5d counts  State: %s\n\n", 
           movi_get_zoom_position(), 
           movi_get_lens_state_string(movi_get_zoom_state()));
    
    // Range limits
    bool f_active, i_active, z_active;
    movi_get_range_limits(&f_active, &i_active, &z_active);
    printf("%s--- RANGE LIMITS ---%s\n", ANSI_BOLD, ANSI_RESET);
    printf("Focus: %s  Iris: %s  Zoom: %s\n\n",
           f_active ? "ON" : "OFF",
           i_active ? "ON" : "OFF",
           z_active ? "ON" : "OFF");
    
    // Camera status
    printf("%s--- CAMERA ---%s\n", ANSI_BOLD, ANSI_RESET);
    printf("Recording: %s\n\n", movi_get_camera_recording() ? "YES" : "NO");
    
    // Controls
    printf("%s--- CONTROLS ---%s\n", ANSI_BOLD, ANSI_RESET);
    printf("D-Pad: Pan/Tilt       Select: Mode\n");
    printf("B1: Gimbal Kill       B2: Record\n");
    printf("B3: Clear Faults      B4: Auto-Cal\n");
}

void movi_display_clear(void)
{
    printf("%s", ANSI_CLEAR);
}

void movi_display_status(const char *status)
{
    printf("%s[*]%s %s\n", ANSI_BOLD, ANSI_RESET, status);
}

void movi_display_error(const char *error)
{
    printf("%s[!]%s %s\n", ANSI_BOLD, ANSI_RESET, error);
}

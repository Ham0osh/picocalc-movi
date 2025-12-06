#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"

#include "drivers/picocalc.h"
#include "drivers/display.h"
#include "drivers/keyboard.h"
#include "drivers/onboard_led.h"

#include "movi_controller.h"
#include "movi_display.h"
#include "movi_input.h"

bool power_off_requested = false;

void set_onboard_led(uint8_t led)
{
    led_set(led & 0x01);
}

int main()
{
    // Initialize hardware
    int led_init_result = led_init();

    stdio_init_all();
    picocalc_init();
    if (led_init_result == 0) {
        display_set_led_callback(set_onboard_led);
    }

    // Initialize Movi controller system
    movi_display_init();
    movi_input_init();
    movi_init();

    printf("\033c\033[1m\n=== MOVI PRO GIMBAL CONTROLLER ===\033[0m\n");
    printf("Initializing...\n\n");

    // Give gimbal time to initialize
    sleep_ms(500);

    // Main control loop (50Hz target)
    const uint32_t loop_period_ms = 20;  // 50Hz = 20ms per iteration
    uint64_t last_update_us = time_us_64();

    while (true)
    {
        uint64_t now_us = time_us_64();
        uint32_t elapsed_us = now_us - last_update_us;

        if (elapsed_us >= (loop_period_ms * 1000))
        {
            // Update controllers
            movi_update();
            movi_input_update();
            movi_display_update();

            last_update_us = now_us;
        }

        // Check for keyboard input
        if (keyboard_key_available())
        {
            char key = keyboard_get_key();
            movi_input_handle_key(key);
        }

        // Small sleep to prevent busy-waiting
        sleep_us(100);
    }

    return 0;
}

# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(PicoCalc_MoviCon_v1.0 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()


# Add executable. Default name is the project name, version 0.1

add_executable(PicoCalc_MoviCon_v1.0
        main.c
        commands.c
        commands.h
        songs.c
        songs.h
        tests.c
        tests.h
        movi_controller.c
        movi_controller.h
        movi_display.c
        movi_display.h
        movi_input.c
        movi_input.h
        qx_uart.c
        qx_uart.h
        drivers/audio.c
        drivers/audio.h
        drivers/clib.c
        drivers/display.c
        drivers/display.h
        drivers/fat32.c
        drivers/fat32.h
        drivers/font-5x10.c
        drivers/font-8x10.c
        drivers/font.h
        drivers/keyboard.c
        drivers/keyboard.h
        drivers/lcd.c
        drivers/lcd.h
        drivers/onboard_led.c
        drivers/onboard_led.h
        drivers/picocalc.c
        drivers/picocalc.h
        drivers/sdcard.c
        drivers/sdcard.h
        drivers/southbridge.c
        drivers/southbridge.h
        api/freefly/QX_Protocol.c
        api/freefly/QX_Protocol.h
        api/freefly/QX_Protocol_App.c
        api/freefly/QX_Protocol_App.h
        api/freefly/QX_Parsing_Functions.c
        api/freefly/QX_Parsing_Functions.h
        api/freefly/QX_App_Config.h
        api/freefly/FreeflyAPI.h
        api/freefly/simple_buffer.c
        api/freefly/simple_buffer.h
        )

pico_set_program_name(PicoCalc_MoviCon_v1.0 "PicoCalc_MoviCon_v1.0")
pico_set_program_version(PicoCalc_MoviCon_v1.0 "1.0")


# Generate PIO header
pico_generate_pio_header(PicoCalc_MoviCon_v1.0 ${CMAKE_CURRENT_LIST_DIR}/drivers/audio.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(PicoCalc_MoviCon_v1.0 0)
pico_enable_stdio_usb(PicoCalc_MoviCon_v1.0 0)

# Turn on all warnings (but don't treat as errors - we have unused defs that will be used later)
target_compile_options(PicoCalc_MoviCon_v1.0 PRIVATE -Wall)

# Add the standard library to the build
target_link_libraries(PicoCalc_MoviCon_v1.0
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(PicoCalc_MoviCon_v1.0 PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(PicoCalc_MoviCon_v1.0
        pico_stdlib
        pico_printf
        pico_float
        pico_status_led
        pico_rand
        hardware_gpio
        hardware_i2c
        hardware_spi
        hardware_pio
        hardware_clocks
        )

pico_add_extra_outputs(PicoCalc_MoviCon_v1.0)


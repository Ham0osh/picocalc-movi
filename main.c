// UART Tester for PicoCalc on GP4 (TX) / GP5 (RX)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"

#include "drivers/picocalc.h"
#include "drivers/display.h"
#include "drivers/keyboard.h"

#define UART_PORT uart1
#define UART_TX_PIN 5
#define UART_RX_PIN 4

#define DEFAULT_BAUD 111111

static volatile bool paused = false;
static int current_baud = DEFAULT_BAUD;

// ANSI colours
#define ANSI_RESET "\033[m"
#define ANSI_WHITE "\033[37m"
#define ANSI_YELLOW "\033[33m"
#define ANSI_BLUE "\033[34m"

// Format uptime into hh:mm ss
static void format_timestamp(char *buf, size_t buflen)
{
    uint64_t us = time_us_64();
    uint64_t s = us / 1000000ULL;
    unsigned int hh = (unsigned int)((s / 3600) % 100);  // wrap at 99 hours
    unsigned int mm = (unsigned int)((s % 3600) / 60);
    unsigned int ss = (unsigned int)(s % 60);
    snprintf(buf, buflen, "%02u:%02u %02u", hh, mm, ss);
}

static void draw_header(void)
{
    // Clear line and print header with pin definitions
    display_emit('\033'); display_emit('['); display_emit('H');           // Home
    display_emit('\033'); display_emit('['); display_emit('2'); display_emit('J'); // Clear screen
    display_emit('\033'); display_emit('['); display_emit('?'); display_emit('2'); display_emit('5'); display_emit('l'); // hide cursor

    // White text
    printf(ANSI_WHITE);
    printf("GP4=RX  GP5=TX  3.3V ONLY");
    if (paused)
    {
        printf("  [STOPPED]");
    }
    printf("\n" ANSI_RESET);
}

static void uart_apply_baud(int baud)
{
    current_baud = baud;
    uart_set_baudrate(UART_PORT, baud);
}

static void uart_init_gpio_and_port(int baud)
{
    // Mux pins to UART
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // Flow control off, FIFO on, CR/LF translation off
    uart_set_hw_flow(UART_PORT, false, false);
    uart_set_fifo_enabled(UART_PORT, true);
    uart_set_translate_crlf(UART_PORT, false);

    uart_init(UART_PORT, baud);
}

static void log_rx_char(char ch)
{
    char ts[16];
    format_timestamp(ts, sizeof ts);
    printf(ANSI_YELLOW "%s Rx~ %c\n" ANSI_RESET, ts, ch);
}

static void log_tx_text(const char *text)
{
    char ts[16];
    format_timestamp(ts, sizeof ts);
    printf(ANSI_BLUE "%s Tx~ %s\n" ANSI_RESET, ts, text);
}

static void log_tx_char(char ch)
{
    char ts[16];
    format_timestamp(ts, sizeof ts);
    printf(ANSI_BLUE "%s Tx~ %c\n" ANSI_RESET, ts, ch);
}

// Send helpers
static void send_byte_pattern(uint8_t value, int count)
{
    for (int i = 0; i < count; ++i)
    {
        uart_putc_raw(UART_PORT, value);
    }
}

static void send_string(const char *s)
{
    while (*s)
    {
        uart_putc_raw(UART_PORT, *s++);
    }
}

// Simple baud menu: edit 6 digits with D-pad and number keys; Enter to save
static void open_baud_menu(void)
{
    paused = true;
    draw_header();

    char digits[7];
    snprintf(digits, sizeof digits, "%06d", current_baud);
    int pos = 0;

    printf(ANSI_WHITE "Set Baud (6 digits): %s\n" ANSI_RESET, digits);
    printf("Use D-pad to move, 0-9 to set, Enter to save\n");

    // Show cursor indicator below digits
    while (1)
    {
        // Render position indicator
        printf("      ");
        for (int i = 0; i < 6; ++i)
            printf(i == pos ? "^" : " ");
        printf("\r\n");

        // Poll keyboard
        while (!keyboard_key_available())
        {
            sleep_ms(50);
        }
        char ch = keyboard_get_key();

        if (ch >= '0' && ch <= '9')
        {
            digits[pos] = ch;
            printf("\033[2A");
            printf(ANSI_WHITE "Set Baud (6 digits): %s\n" ANSI_RESET, digits);
        }
        else if (ch == KEY_LEFT)
        {
            if (pos > 0) pos--;
        }
        else if (ch == KEY_RIGHT)
        {
            if (pos < 5) pos++;
        }
        else if (ch == '\r' || ch == '\n')
        {
            int baud = atoi(digits);
            if (baud < 1200) baud = 1200; // basic guard
            uart_apply_baud(baud);
            break;
        }
    }

    paused = false;
    draw_header();
}

int main(void)
{
    // Init PicoCalc high-level + drivers
    picocalc_init();
    keyboard_init();
    display_init();

    // Init UART1 on GP4/GP5
    uart_init_gpio_and_port(DEFAULT_BAUD);

    draw_header();

    while (1)
    {
        // Handle incoming UART RX
        if (!paused)
        {
            while (uart_is_readable(UART_PORT))
            {
                char ch = uart_getc(UART_PORT);
                log_rx_char(ch);
            }
        }

        // Keyboard input to TX and controls
        if (keyboard_key_available())
        {
            char key = keyboard_get_key();

            // Controls
            if (key == KEY_F1)
            {
                if (!paused)
                {
                    send_byte_pattern(0x55, 32);
                    log_tx_text("0x55 x32");
                }
            }
            else if (key == KEY_F2)
            {
                if (!paused)
                {
                    send_byte_pattern(0xAA, 32);
                    log_tx_text("0xAA x32");
                }
            }
            else if (key == KEY_F3)
            {
                if (!paused)
                {
                    const char *msg = "hello world\r\n";
                    send_string(msg);
                    log_tx_text("hello world");
                }
            }
            else if (key == KEY_F4)
            {
                paused = !paused;
            }
            else if (key == KEY_F5)
            {
                draw_header();
            }
            else if (key == KEY_HOME || key == KEY_F10)
            {
                open_baud_menu();
            }
            else
            {
                // Regular transmit of typed key (only if not paused)
                if (!paused)
                {
                    uart_putc_raw(UART_PORT, key);
                    log_tx_char(key);
                }
            }
        }

        sleep_ms(10);
    }

    return 0;
}

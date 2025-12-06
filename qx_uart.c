/*
 * QX Protocol UART Interface for Pico
 * 
 * This module bridges the QX Protocol library with the Pico UART hardware
 */

#include <stdint.h>
#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/uart.h"

#include "api/freefly/simple_buffer.h"
#include "api/freefly/QX_Protocol.h"

/*---------------------------------------------------------------------------
 * UART Configuration (Gimbal control port)
 *---------------------------------------------------------------------------*/

#define GIMBAL_UART uart0
#define GIMBAL_UART_BAUD 111111
#define GIMBAL_UART_TX 0
#define GIMBAL_UART_RX 1

/*---------------------------------------------------------------------------
 * Public API
 *---------------------------------------------------------------------------*/

/**
 * Initialize UART for gimbal communication
 */
void qx_uart_init(void)
{
    // Initialize UART
    uart_init(GIMBAL_UART, GIMBAL_UART_BAUD);
    
    // Set UART pins
    gpio_set_function(GIMBAL_UART_TX, GPIO_FUNC_UART);
    gpio_set_function(GIMBAL_UART_RX, GPIO_FUNC_UART);
    
    // No interrupt for now - polling in main loop
    printf("[QX] UART initialized: %d baud\n", GIMBAL_UART_BAUD);
}

/**
 * Send all pending data from QX transmit buffer to UART
 */
void qx_uart_send_pending(void)
{
    uint8_t byte;
    
    // Empty the transmit buffer (SEND_BUF_IDX = 1)
    while (BufRemove(1, (volatile uint8_t *)&byte) == 1) {
        uart_putc(GIMBAL_UART, byte);
    }
}

/**
 * Process all available UART data through QX protocol state machine
 */
void qx_uart_recv_pending(void)
{
    // Read all available bytes from UART and feed to protocol parser
    while (uart_is_readable(GIMBAL_UART)) {
        uint8_t ch = uart_getc(GIMBAL_UART);
        QX_StreamRxCharSM(QX_COMMS_PORT_UART, ch);
    }
}

/**
 * Get UART status
 */
bool qx_uart_readable(void)
{
    return uart_is_readable(GIMBAL_UART);
}

bool qx_uart_writable(void)
{
    return uart_is_writable(GIMBAL_UART);
}

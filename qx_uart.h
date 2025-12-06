/*
 * QX Protocol UART Interface Header
 */

#ifndef QX_UART_H
#define QX_UART_H

#include <stdbool.h>

/**
 * Initialize UART for gimbal communication
 */
void qx_uart_init(void);

/**
 * Send all pending data from QX transmit buffer to UART
 */
void qx_uart_send_pending(void);

/**
 * Process all available UART data through QX protocol state machine
 */
void qx_uart_recv_pending(void);

/**
 * Get UART status
 */
bool qx_uart_readable(void);
bool qx_uart_writable(void);

#endif // QX_UART_H

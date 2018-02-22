/**
 * @file uart.h
 * @author Rhys Thomas <rt8g15@soton.ac.uk>
 * @date 2018-02-07
 *
 * @brief UART functions for printing to the terminal.
 */

#ifndef UART_H
#define UART_H

#include <msp430.h>
#include <stdint.h>

#define TXPIN (BIT0) ///< UCA1TXD on P2.0
#define RXPIN (BIT1) ///< UCA1RXD on P2.1

/**
 * Initialise the UART peripheral. Useful for debugging.
 *
 * @ingroup init
 */
void uart_init(void);

/**
 * @defgroup debug Debug
 * @brief Functions for debugging the system.
 * @{
 */

/**
 * Print character over UART.
 *
 * @param ch : Character byte to print.
 * @bug This is working (Saleae) but not on the serial line? Perhaps this is
 * because I'm pulling the UART through the other MSP programmer?
 */
void uart_putc(unsigned char ch);

/**
 * Print string of characters over UART.
 *
 * @param string : Pointer to string to print.
 */
void uart_puts(const char *string);

/** @} */

#endif // UART_H

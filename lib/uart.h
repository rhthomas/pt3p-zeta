/**
 * @file uart.h
 * @author Rhys Thomas <rt8g15@soton.ac.uk>
 * @date 2018-02-07T23:59
 *
 * @brief UART functions for printing to the terminal.
 *
 * @todo What to do with Rx'd UART data?
 */

#ifndef UART_H
#define UART_H

#include <msp430.h>
#include <stdint.h>

#define TXPIN (BIT5) ///< UCA1TXD
#define RXPIN (BIT6) ///< UCA1RXD

/**
 * Initialise the UART peripheral. Useful for debugging.
 */
void uart_init(void);

/**
 * Print character over UART.
 *
 * @param ch : Character byte to print.
 */
void uart_putc(char ch);

/**
 * Print string of characters over UART.
 *
 * @param string : Pointer to string to print.
 */
void uart_puts(char *string);

#endif // UART_H

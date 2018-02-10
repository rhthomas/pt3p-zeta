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

#define TXPIN (BIT0) ///< UCA0TXD
#define RXPIN (BIT1) ///< UCA0RXD

/**
 * Initialise the UART peripheral. Useful for debugging.
 */
void uart_init(void);

/**
 * Print character over UART.
 *
 * @param ch : Character byte to print.
 */
void uart_putc(unsigned char ch);

/**
 * Print string of characters over UART.
 *
 * @param string : Pointer to string to print.
 */
void uart_puts(const char *string);

#endif // UART_H

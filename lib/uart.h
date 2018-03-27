/**
 * @file uart.h
 * @author Rhys Thomas <rt8g15@soton.ac.uk>
 * @date 2018-02-07
 *
 * @brief UART functions for printing to the terminal.
 *
 * Pin-out:
 * ```
 * MSP430FR5739
 * -------+
 *        |
 *    P2.0|-> Transmit output (UCA0TXD)
 *    P2.1|<- Receive input   (UCA0RXD)
 *        |
 * -------+
 * ```
 */

#ifndef UART_H
#define UART_H

#include <msp430.h>
#include <stdint.h>

#define TXPIN (BIT0) ///< UCA0TXD on P2.0.
#define RXPIN (BIT1) ///< UCA0RXD on P2.1.

/**
 * @brief Initialise the UART peripheral. Useful for debugging.
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
 * @brief Print character over UART.
 *
 * @param ch : Character byte to print.
 */
void uart_putc(unsigned char ch);

/**
 * @brief Print string of characters over UART.
 *
 * @param string : Pointer to string to print.
 */
void uart_puts(const char *string);

/** @} */

#endif // UART_H

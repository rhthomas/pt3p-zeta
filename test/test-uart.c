/**
 * @file test-uart.c
 * @author Rhys Thomas <rt8g15@soton.ac.uk>
 * @date 2018-01-29T14:18
 *
 * @brief UART library test.
 */

// Libraries.
#include "setup.h"
#include "uart.h"

/**
 * Main loop.
 */
void main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

    io_init();
    clock_init();
    uart_init();

    while (1) {
        __bis_SR_register(GIE);
        uart_putc('R');
        __delay_cycles(2400000); // ~1s delay
    }
}

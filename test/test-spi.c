/**
 * @file test-spi.c
 * @author Rhys Thomas <rt8g15@soton.ac.uk>
 * @date 2018-02-09
 *
 * @brief SPI library test.
 */

// Libraries.
#include "setup.h"
#include "spi.h"

/**
 * Main loop.
 */
void main(void)
{
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    io_init();
    clock_init();
    spi_init();

    while (1) {
        __bis_SR_register(GIE);
        spi_xfer(0xaa); // 10101010
        __delay_cycles(24e5); // ~1s delay
        PJOUT ^= BIT0;
    }
}

/**
 * @file test-uart.c
 * @author Rhys Thomas <rt8g15@soton.ac.uk>
 * @date 2018-01-29T14:18
 *
 * @brief UART library test.
 *
 * @todo Add LPM state changing.
 * @todo Consider methods of optimisation. Speed or size?
 * @todo Utilise ISRs for incoming data.
 * @todo Transfer SPI data via DMA.
 * @todo Decide where to print the Rx'd byte. -DDEBUG in zeta.c?
 */

#include <msp430.h>
#include "uart.h"

/**
 * Initialise IO pins and buses.
 *
 * @see ULP4.1
 */
void io_init(void)
{
    // All ports output.
    PADIR = 0xFFFF;
    PBDIR = 0xFFFF;
    PJDIR = 0xFFFF;
    // All outputs low.
    PAOUT = 0;
    PBOUT = 0;
    PJOUT = 0;
    // Reset interrupt flags.
    PAIFG = 0;
    PBIFG = 0;
}

/**
 * Initialise system clock.
 *
 * MCLK  = DCO @ 24MHz
 * SMCLK = DCO/2 @ 12MHz
 * ACLK  = VLO @ 10kHz
 */
void clock_init(void)
{
    CSCTL0_H = 0xA5; // Unlock CS registers.
    CSCTL1 |= DCORSEL | DCOFSEL0 | DCOFSEL1; // 24MHz
    // ACLK = SMCLK = MCLK = DCOCLK
    CSCTL2 |= SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
    // SMCLK=DCO/2
    CSCTL3 |= DIVA__1 | DIVS__2 | DIVM__1;
    // Power down unused clocks.
    CSCTL4 |= XT2OFF | XT1OFF;
    CSCTL0_H = 0; // Lock the registers.
}

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

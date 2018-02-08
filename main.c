/**
 * @file main.c
 * @author Rhys Thomas <rt8g15@soton.ac.uk>
 * @date 2018-01-29T14:18
 *
 * @brief Example program for custom ZetaPlus library.
 *
 * @details Based on the ZetaPlus CODEC arduino library. This example program
 * enters the radio into receive mode to handle 12x8-bit packets.
 *
 * @todo Add LPM state changing.
 * @todo Consider methods of optimisation. Speed or size?
 * @todo Utilise ISRs for incoming data.
 * @todo Transfer SPI data via DMA.
 * @todo Decide where to print the Rx'd byte. -DDEBUG in zeta.c?
 */

#include <msp430.h>
#include "zeta.h"
#include "spi.h"
#include "uart.h"
#include "hibernation.h"

// Globals for storing incoming bytes.
uint8_t rxByte, len;
// 8B data packet. TODO Decide what size.
uint8_t packet[8];

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
 * @todo Change the config for your project.
 */
void clock_init(void)
{
    CSCTL0_H = CSKEY >> 8; // Unlock CS registers.
    CSCTL1 = DCOFSEL0 | DCOFSEL1; // DCO set to max.
    // ACLK = VLOCLK, SMCLK = DCOCLK, MCLK = DCOCLK
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
    // No prescale on clocks.
    CSCTL3 = DIVA__1 | DIVS__1 | DIVS__1;
    // TODO Power down unused clocks.
    //CSCTL4 |= HFXTOFF | LFXTOFF | VLOOFF;
    CSCTL0 = 0; // Lock the registers.
}

/**
 * Main loop.
 *
 * @todo Enter into a LPM somewhere!
 */
void main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer
    /* Disable the GPIO power-on default high-z mode to
     * activate prev config port settings. */
    PM5CTL0 &= ~LOCKLPM5;

    Hibernus(); // Begin hibernus.

    io_init();
    clock_init();
    uart_init();
    spi_init();
    zeta_init();

    zeta_ready(); // Wait for radio to be ready to rx commands.

    while (1) {
        zeta_rx_mode(0x00, 0x0c); // Rx mode on ch0, 12-byte packets.
        zeta_rx_packet(&packet); // Rx packet and store in array.

        /* TODO This may not be needed since next packet will take a while to
         * come in. */
        __delay_cycles(480000); // delay(20)
    }
}

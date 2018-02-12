/**
 * @file test-spi.c
 * @author Rhys Thomas <rt8g15@soton.ac.uk>
 * @date 2018-02-09
 *
 * @brief SPI library test.
 *
 * @details Makes calls from the zeta.h library in order to test that SPI
 * commands are being correctly send out of the MOSI port.
 */

// Libraries.
#include "setup.h"
#include "spi.h"
#include "zeta.h"

uint8_t packet[12];

/**
 * Main loop.
 */
void main(void)
{
    // Stop watchdog timer.
    WDTCTL = WDTPW | WDTHOLD;

    // Setup peripherals.
    io_init();
    clock_init();
    timer_init();
    spi_init();

    // Main loop.
    while (1) {
        // Go to sleep. Wait for timer interrupt.
        __bis_SR_register(LPM3_bits | GIE);
        // Send some SPI data.
//        spi_xfer(0xAA);
        zeta_set_baud_host(4u);
    }
}

/**
 * Timer overflow ISR.
 *
 * Triggers every ~1s and exits the processor from LPM3. The MCU then sends SPI
 * data out of the SPI peripheral and returns to sleep.
 */
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_ISR(void)
{
    PJOUT ^= BIT0; // Toggle LED to show you're in ISR.
    // Exit into active mode and disable interrupts while Tx'ing.
    __bic_SR_register_on_exit(LPM3_bits | GIE);
}

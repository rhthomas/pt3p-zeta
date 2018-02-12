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
#include "zeta.h"

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
        zeta_set_baud_rf(6u);
    }
}

/**
 * Timer overflow ISR.
 *
 * Triggers every ~1s and exits the processor from LPM3. The MCU then sends a
 * packet onto the ether, sleeps the radio and returns to LPM3.
 */
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_ISR(void)
{
    PJOUT ^= BIT0; // Toggle LED to show you're in ISR.
    // Exit into active mode and disable interrupts while Tx'ing.
    __bic_SR_register_on_exit(LPM3_bits | GIE);
}

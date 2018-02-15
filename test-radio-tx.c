/**
 * @file test-radio-tx.c
 * @author Rhys Thomas <rt8g15@soton.ac.uk>
 * @date 2018-02-10
 *
 * @brief Testing transmission on the radio module.
 */

// Libraries.
#include "setup.h" // System setup functions
#include "zeta.h" // Radio

uint8_t msg[] = {'H', 'E', 'L', 'L', 'O'};

/**
 * Main loop.
 */
void main(void)
{
    // Stop watchdog timer.
    WDTCTL = WDTPW + WDTHOLD;

    // Setup peripherals.
    io_init();
    clock_init();
    timer_init();
    spi_init();
    zeta_init();

    zeta_select_mode(2u);

    // Main loop.
    while (1) {
        // Go to sleep. Wait for timer interrupt.
        __bis_SR_register(LPM3_bits | GIE);
        // Send packet. Returns to previous state (sleep) when done.
        zeta_get_vers();
//        zeta_get_settings();
//        zeta_send_packet(msg, sizeof(msg));
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
    P3OUT ^= BIT7; // Toggle LED to show you're in ISR.
    // Exit into active mode and disable interrupts while Tx'ing.
    __bic_SR_register_on_exit(LPM3_bits | GIE);
}

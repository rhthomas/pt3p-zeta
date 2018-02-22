/**
 * @file test-radio-rx.c
 * @author Rhys Thomas <rt8g15@soton.ac.uk>
 * @date 2018-02-10
 *
 * @brief Testing reception on the radio module.
 */

// Libraries.
#include "setup.h" // System setup functions
#include "uart.h" // Debugging
#include "zeta.h" // Radio

uint8_t in_packet[5u + 4u]; ///< Array for received data.
uint8_t count;

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
    uart_init();
    spi_init();
    zeta_init();

    // Set Rx mode.
    zeta_select_mode(1);

    // Main loop.
    while (1) {
        __bis_SR_register(GIE);
        // Operating on channel 0 with packet size of 12 bytes.
        zeta_rx_mode(CHANNEL, 5u + 4u);
        // Get incoming packet.
        zeta_rx_packet(in_packet);
        // Short delay after Rx'ing packet.
        __delay_cycles(48e4); // ~0.020s
        // Print packet contents to terminal.
        for (count = 0; count < (5u + 4u); count++) {
            uart_putc(in_packet[count]);
            in_packet[count] = 'x';
        }
    }
}

/**
 * Timer overflow ISR.
 *
 * Used to debug to check that the radio is listening.
 */
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_ISR(void)
{
    P3OUT ^= BIT7;
    uart_puts("Listening...\r\n");
}

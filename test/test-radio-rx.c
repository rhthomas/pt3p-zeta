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

uint8_t in_packet[PACKET_SIZE]; ///< Array for received data.
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

    // Main loop.
    while (1) {
        // Go to sleep.
        __bis_SR_register(GIE);
        // Set Rx mode.
        zeta_select_mode(1);
        // Operating on channel 0 with packet size of 12 bytes.
        zeta_rx_mode(CHANNEL, PACKET_SIZE);
        // Get incoming packet.
        zeta_rx_packet(&in_packet);
        // Short delay after Rx'ing packet.
        __delay_cycles(48e4); // ~0.020s
        // Put radio to sleep.
        zeta_select_mode(3);
        // Print packet contents to terminal.
        for (count = 0; count < PACKET_SIZE; count++) {
            uart_putc(in_packet[count]);
        }
    }
}

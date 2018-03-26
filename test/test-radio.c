/* Unified radio test program.
 *
 * Set node as transmitter: #define TXER
 * Set node as receiver: no definition.
 */

// Libraries.
#include "util.h" // System setup functions
#include "zeta.h" // Radio

volatile uint8_t exit_loop = 0;

//#define TXER ///< Node is transmitter.

#ifdef TXER
//uint8_t msg[16u] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
uint8_t msg[1] = {0}; // For count example.
#else
//uint8_t in_packet[16u + 4u]; ///< Array for received data.
uint8_t in_packet[1u + 4u] = {0, 0, 0, 0, 0}; // For count example.
#endif

void main(void)
{
    // Stop watchdog timer.
    WDTCTL = WDTPW | WDTHOLD;

    // Setup peripherals.
    io_init();
    clock_init();
    timer_init();
    spi_init();
    zeta_init();

#ifdef TXER
    zeta_select_mode(2); // Put radio in ready mode.
#else
    zeta_select_mode(1); // Put radio in receive mode.
#endif // TXER

    while (1) {
#ifdef TXER
        zeta_send_packet(msg, sizeof(msg));
        msg[0]++;
        __delay_cycles(12e6);
        P3OUT ^= BIT7; // Toggle LED.
#else
        zeta_rx_mode(CHANNEL, sizeof(in_packet) - 4u);
        if (zeta_rx_packet(in_packet)) {
            exit_loop = 0;
        } /*else {
            led_set(in_packet[4]);
        } */
        // Display last valid reception on the LEDs.
        led_set(in_packet[4]);
        __delay_cycles(24e5);
#endif // TXER
    }
}

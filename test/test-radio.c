/* Unified radio test program.
 *
 * Set node as transmitter: #define TXER
 * Set node as receiver: no definition.
 */

// Libraries.
#include "util.h" // System setup functions
#include "zeta.h" // Radio

#define TXER ///< Node is transmitter.

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
    spi_init();
    zeta_init();

#ifdef TXER
    while (1) {
        P3OUT |= BIT7;
        zeta_send_packet(msg, sizeof(msg));
        msg[0]++;
        P3OUT &= ~BIT7;
        __delay_cycles(24e6);
    }
#else
    __delay_cycles(9e5); // Allows time for start-up (required and min).
    while (1) {
        zeta_rx_mode(CHANNEL, sizeof(in_packet) - 4u);
        if (zeta_rx_packet(in_packet)) {
            // Handle timeout task here.
        }
        // Display last valid reception on the LEDs.
        led_set(in_packet[4]);
    }
#endif // TXER
}

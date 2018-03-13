/* Unified radio test program.
 *
 * Set node as transmitter: #define TXER
 * Set node as receiver: no definition.
 */

// Libraries.
#include "util.h" // System setup functions
#include "uart.h" // Debugging
#include "zeta.h" // Radio

//#define TXER ///< Node is transmitter.

#ifdef TXER
uint8_t msg[5u] = {'H', 'E', 'L', 'L', 'O'};
#else
uint8_t in_packet[5u + 4u]; ///< Array for received data.
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

    zeta_reset_default();
    zeta_rx_mode(CHANNEL, 5u);

    // Main loop.
    while (1) {
#ifdef TXER
        // Send packet.
        __delay_cycles(24e5);
        P3OUT ^= BIT7;// Toggle LED to show you're transmitting.
        zeta_send_packet(msg, sizeof(msg));
#else
        // Toggle LED when nIRQ goes low.
        zeta_rx_packet(in_packet);
        P3OUT ^= BIT7; // Toggle LED to show you're in ISR.
        __delay_cycles(24e4);
#endif // TXER
    }
}

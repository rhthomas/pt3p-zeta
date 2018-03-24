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
uint8_t msg[64u] = {
    0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
    0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
    0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
    0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
    0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
    0xAA, 0xAA, 0xAA, 0xAA};
#else
uint8_t in_packet[64u + 4u]; ///< Array for received data.
#endif

uint8_t settings[10u];

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
//    zeta_select_mode(0x2);
#else
    zeta_select_mode(0x1);
    zeta_rx_mode(CHANNEL, 64u);
    P1OUT |= BIT2;
#endif // TXER

    // Main loop.
    while (1) {
#ifdef TXER
        // Send packet.
        zeta_send_packet(msg, sizeof(msg));
        __delay_cycles(24e5);
#else
        zeta_get_settings(settings);
        if (settings[5] == 180) {
            zeta_sync_byte(0xAA, 0xAA, 0xAA, 0xAA);
        }
        if (settings[9] != 0xF) {
            zeta_rx_mode(CHANNEL, 64u);
        }
        // Receive packet.
        zeta_rx_packet(in_packet);
        __delay_cycles(24e4);
#endif // TXER
        P3OUT ^= BIT7; // Toggle LED.
    }
}

/* Unified radio test program.
 *
 * Set node as transmitter: #define TXER
 * Set node as receiver: no definition.
 */

// Libraries.
#include "util.h" // System setup functions
#include "zeta.h" // Radio

#define TXER ///< Node is transmitter.

volatile uint8_t exit_loop = 0;

#ifdef TXER
uint8_t msg[5u] = {'H','E','L','L','O'};
#else
uint8_t in_packet[5u + 4u]; ///< Array for received data.
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
    zeta_select_mode(0x2);
#else
    zeta_select_mode(0x1);
    zeta_rx_mode(CHANNEL, 5u);
#endif // TXER

    // Main loop.
    while (1) {
#ifdef TXER
        timer_start();
        zeta_get_vers();
        timer_stop();
        // Send packet.
        zeta_send_packet(msg, sizeof(msg));
        __delay_cycles(24e5);
#else
        zeta_get_vers();
//        zeta_get_settings(settings);
        // Receive packet.
//        zeta_rx_packet(in_packet);
        __delay_cycles(24e4);
#endif // TXER
        P3OUT ^= BIT7; // Toggle LED.
    }
}

/* Timeout protection.
 */
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
    timer_stop(); // Stop timer.
    exit_loop = 1;
}

/* Unified radio test program.
 *
 * Set node as transmitter: #define TXER
 * Set node as receiver: no definition.
 */

// Libraries.
#include "util.h" // System setup functions
#include "zeta.h" // Radio

//#define TXER ///< Node is transmitter.

volatile uint8_t exit_loop = 0;

#ifdef TXER
//uint8_t msg[16u] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
uint8_t count[1] = {0};
#else
//uint8_t in_packet[16u + 4u]; ///< Array for received data.
uint8_t in_count[1u + 4u];
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
    zeta_select_mode(0x2);
#else
    zeta_select_mode(0x1);
#endif // TXER

    // Main loop.
    while (1) {
#ifdef TXER
//        zeta_send_packet(msg, sizeof(msg));
        zeta_send_packet(count, 1);
        count[0]++;
        __delay_cycles(12e6);
        P3OUT ^= BIT7;// Toggle LED.
#else
//        zeta_rx_mode(CHANNEL, sizeof(in_packet) - 4u);
//        zeta_rx_packet(in_packet);
        zeta_rx_mode(CHANNEL, 1);
        zeta_rx_packet(in_count);
        led_set(in_count[4]);
        __delay_cycles(24e5);
#endif // TXER
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

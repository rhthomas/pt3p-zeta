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
uint8_t msg[1] = {0};
#else
//uint8_t in_packet[16u + 4u]; ///< Array for received data.
uint8_t in_packet[1u + 4u] = {0, 0, 0, 0, 0};
#endif

/* Flash the LEDs to indicate a timeout.
 */
void flash(void)
{
    uint8_t i;
    for (i = 0; i < 5; i++) {
        led_set(0xFF);
        __delay_cycles(4.8e5);
        led_clear();
        __delay_cycles(4.8e5);
    }
}

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
        } else {
            led_set(in_packet[4]);
        }
        __delay_cycles(24e5);
#endif // TXER
    }
}

/* Timeout protection.
 *
 * @todo Put this somewhere more suitable (i.e. inside the zeta library).
 */
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
    timer_stop(); // Stop timer.
    flash();
    exit_loop = 1;
}

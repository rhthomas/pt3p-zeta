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
uint8_t count;
#endif

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

    __bis_SR_register(GIE);

#ifdef TXER
    // Set ready mode.
    zeta_select_mode(2u);
#else
    // Set Rx mode.
    zeta_select_mode(1);
#endif // TXER

    // Main loop.
    while (1) {
#ifdef TXER
        // Go to sleep. Wait for timer interrupt.
        __bis_SR_register(LPM3_bits | GIE);
        // Send packet. Returns to previous state (sleep) when done.
//        zeta_get_vers();
        zeta_get_settings();
        zeta_send_packet(msg, sizeof(msg));
#else
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
#endif // TXER
    }
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_ISR(void)
{
    P3OUT ^= BIT7;
#ifdef TXER
    // Exit into active mode and disable interrupts while Tx'ing.
    __bic_SR_register_on_exit(LPM3_bits | GIE);
#else
    uart_puts("Listening...\r\n");
#endif // TXER
}

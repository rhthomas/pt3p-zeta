// Libraries.
#include "util.h" // System setup functions
#include "zeta.h" // Radio

volatile uint8_t exit_loop = 0;

uint8_t in_packet[16u + 4u + 7u]; ///< Array for received data.

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

    zeta_select_mode(0x1);

    // Main loop.
    while (1) {
        zeta_rx_mode(CHANNEL, 16u + 7u);
        zeta_rx_packet(in_packet);
        __delay_cycles(24e4);
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

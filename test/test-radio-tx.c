/* Radio transmission test.
 */

// Libraries.
#include "util.h" // System setup functions
#include "zeta.h" // Radio

volatile uint8_t exit_loop = 0;

uint8_t msg[16u + 7u] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0};

void main(void)
{
    // Stop watchdog timer.
    WDTCTL = WDTPW + WDTHOLD;

    // Setup peripherals.
    io_init();
    clock_init();
    spi_init();
    zeta_init();

    zeta_select_mode(0x2);

    // Main loop.
    while (1) {
        zeta_send_packet(msg, sizeof(msg));
        __delay_cycles(24e5);
        P3OUT ^= BIT7; // Toggle LED.
    }
}

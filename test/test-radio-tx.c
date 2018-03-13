/* Radio transmission test.
 */

// Libraries.
#include "util.h" // System setup functions
#include "zeta.h" // Radio

uint8_t msg[5u] = {'H', 'E', 'L', 'L', 'O'};

void main(void)
{
    // Stop watchdog timer.
    WDTCTL = WDTPW + WDTHOLD;

    // Setup peripherals.
    io_init();
    clock_init();
    spi_init();
    zeta_init();

    // Main loop.
    while (1) {
        // Send packet.
        __delay_cycles(24e5);
        P3OUT ^= BIT7; // Toggle LED to show you're transmitting.
        zeta_send_packet(msg, sizeof(msg));
    }
}

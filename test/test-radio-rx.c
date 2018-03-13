/* Radio reception test.
 */

// Libraries.
#include "util.h" // System setup functions
#include "uart.h" // Debugging
#include "zeta.h" // Radio

uint8_t in_packet[5u + 4u]; ///< Array for received data.

void main(void)
{
    // Stop watchdog timer.
    WDTCTL = WDTPW | WDTHOLD;

    // Setup peripherals.
    io_init();
    clock_init();
    spi_init();
    zeta_init();

    uint8_t len = 0;

    // Main loop.
    while (1) {
        zeta_rx_mode(CHANNEL, 5u);
        zeta_read_byte(); // #
        zeta_read_byte(); // R
        len = zeta_read_byte(); // len
        zeta_read_byte(); // rssi
        for (; len > 0; len--) {
            zeta_read_byte(); // data
        }
        P3OUT ^= BIT7; // Toggle LED to show you're in ISR.
    }
}

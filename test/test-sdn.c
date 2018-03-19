/* Testing the shutdown latching operation of the node.
 *
 * "UB20" turns on, MCU counts up 5 times then hibernates and shuts down.
 */

#include <msp430.h>

#include "util.h"
#include "hibernation.h"

void main(void)
{
    // Stop watchdog timer.
    WDTCTL = WDTPW | WDTHOLD;
    // Unlock the system.
    PM5CTL0 &= ~LOCKLPM5;

    // System initialisations.
    io_init();
    clock_init();

    // Count up a few times.
    uint8_t n = 0;
    while (n < 6) {
        led_set(n++);
        __delay_cycles(24e6);
    }

    // Shutdown the node.
    P4OUT &= ~PS_LATCH;
}

/* Hibernus test code.
 *
 * 1. System is initialised.
 * 2. Check if the processor woke from LPM4.5.
 * 3. Continue with main functionality.
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

    // Initialise the system.
    io_init();
    clock_init();
    Hibernus();
    __bis_SR_register(GIE);

#ifdef USE_LPM45
    if (SYSRSTIV == SYSRSTIV_LPM5WU) {
        // System woke-up from LPM4.5.
        // ...
    } else {
        // System woke-up from "cold start".
        // ...
    }
#endif // USE_LPM45

    uint8_t n;
    while (1) {
        for (n = 0; n < 256; n++) {
            led_set(n);
            __delay_cycles(24e5);
        }
    }
}

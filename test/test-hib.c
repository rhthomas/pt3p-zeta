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

    // System initialisations.
    io_init();
    clock_init();

    // Check reason for wake-up.
#ifdef USE_LPM5
    if (SYSRSTIV == SYSRSTIV_LPM5WU) {
        // System woke-up from LPM4.5.
        // __delay_cycles(10);
        // P1.0 ISR will now trigger.
        __bis_SR_register(GIE);
    } else {
        // System woke-up from "cold start".
        // ...
    }
#endif // USE_LPM5

    // ISR returns and continue with main.
    Hibernus();

    uint8_t n;
    while (1) {
        led_set(n++);
        __delay_cycles(24e5);
    }
}

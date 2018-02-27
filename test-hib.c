/**
 * @file test-hib.c
 * @author Rhys Thomas <rt8g15@soton.ac.uk>
 * @date 2018-02-26
 *
 * @brief Testing hibernus functionality with new pin layout and clocks.
 */

#include <msp430.h>

#include "setup.h"
#include "led.h"
#include "hibernation.h"

void main(void)
{
    // Stop watchdog timer.
    WDTCTL = WDTPW | WDTHOLD;
    // Unlock the system.
    PM5CTL0 &= ~LOCKLPM5;

#ifdef USE_LPM45
    if (SYSRSTIV == SYSRSTIV_LPM5WU) {
        // If woke-up from LPM4.5,
        __bis_SR_register(GIE);
        P1OUT |= BIT2;
    } else {
        io_init();
        // System woke-up from "cold start".
    }
#endif // USE_LPM45

    io_init();
    clock_init();
    Hibernus();

    __bis_SR_register(GIE);

    uint8_t n = 0;
    while (1) {
        for (; n < 256; n++) {
            led_set(n++);
            __delay_cycles(24e5);
        }
    }
}

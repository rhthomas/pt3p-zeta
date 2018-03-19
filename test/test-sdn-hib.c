/* Testing the shutdown latching operation of the node.
 *
 * "UB20" turns on, MCU counts up 5 times then hibernates and shuts down.
 */

#include <msp430.h>

#include "util.h"
#include "hibernation.h"

#define EXT_ON (P1IN & EXT_COMP)

uint8_t n = 0;

/* Code to execute when gated by comparator.
 */
void node_active(void)
{
    // Count up some LEDs as demonstration.
    while (1) {
        led_set(n++);
        __delay_cycles(24e5);
    }
}

/* Code to execute when gated by UB20.
 */
void node_inactive(void)
{
    uint8_t i;
    for (i = 0; i < 5; i++) {
        led_set(0xAA);
        __delay_cycles(12e6);
        led_set(0x55);
        __delay_cycles(12e6);
    }
    // Shutdown node.
    power_off();
}

void main(void)
{
    // Stop watchdog timer.
    WDTCTL = WDTPW | WDTHOLD;
    // Unlock the system.
    PM5CTL0 &= ~LOCKLPM5;

    // System initialisations.
    io_init();
    clock_init();

    if (EXT_ON) {
        __enable_interrupts(); // This will trigger hibernus ISR (hopefully!).
        Hibernus();
        node_active();
    } else {
        node_inactive();
    }

    // Should never get here!
    while (1)
        ;
}

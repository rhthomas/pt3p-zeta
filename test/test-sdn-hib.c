/* Testing the shutdown latching operation of the node.
 *
 * "UB20" turns on, MCU counts up 5 times then hibernates and shuts down.
 */

#include <msp430.h>

#include "util.h"
#include "hibernation.h"

/* Code to execute when gated by UB20.
 */
void node_inactive(void)
{
    uint8_t i = 5;
    for (; i > 0; i--) {
        led_set(0xAA);
        __delay_cycles(6e6);
        led_set(~0xAA);
        __delay_cycles(6e6);
    }
    // Check the supply hasn't come up meanwhile.
    if (!COMP_ON) {
        // Shutdown node.
        power_off();
    }
}

/* Code to execute when gated by comparator.
 */
void node_active(void)
{
    // This will trigger hibernus ISR.
    __bis_SR_register(GIE);
    Hibernus();
    // Count up some LEDs as demonstration.
    uint8_t n;
    while (1) {
        led_set(n++);
        __delay_cycles(24e5);
    }
}

void main(void)
{
    // Stop watchdog timer.
    WDTCTL = WDTPW | WDTHOLD;

    // System initialisations.
    io_init();
    clock_init();

    // 1ms delay to wait for comparator output to be set.
    __delay_cycles(24e3);
    if (!COMP_ON) {
        node_inactive();
    }

    node_active();
}

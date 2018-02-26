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

/**
 * @brief Main function.
 */
void main(void)
{
    // Stop watchdog timer.
    WDTCTL = WDTPW | WDTHOLD;

    io_init();
    clock_init();
    Hibernus();

    uint8_t i;
    while (1) {
        for (i = 0; i < 256; i++) {
            led_set(i);
            __delay_cycles(24e5);
        }
    }
}

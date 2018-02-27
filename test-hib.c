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

    io_init();
    clock_init();
    Hibernus();

    __bis_SR_register(GIE);

    uint8_t i = 0;
    while (1) {
        led_set(i++);
        __delay_cycles(24e5);
    }
}

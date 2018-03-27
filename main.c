/* Unified main file for project operation.
 *
 * When the node is powered on first setup the necessary peripherals. Then
 * check the status of the external comparator. If it is on, then the supply
 * is over the active threshold and so the "normal" node operation can
 * continue.
 *
 * If the comparator is off, then the supply was gated by the UB20. In this
 * case the node just needs to receive the incoming packet (with additional
 * timeout protection) and store the packet into NVM to handle later. Once this
 * is done the node releases the GPIO latching the power-supply and it shuts
 * down.
 */

#include "util.h"        // System setup functions
#include "uart.h"        // Debugging
#include "zeta.h"        // Radio
#include "hibernation.h" // Hibernus

uint8_t in_packet[1u + 4u] = {0};

/* Code to execute when gated by UB20.
 */
void node_inactive(void)
{
    __delay_cycles(24e5); // Allows time for start-up (required and min).
    zeta_select_mode(1);
    zeta_rx_mode(CHANNEL, sizeof(in_packet) - 4u);

    if (zeta_rx_packet(in_packet)) {
        // Handle timeout task here.
    } else {
        // Display last valid reception on the LEDs.
        led_set(in_packet[4]);
        __delay_cycles(24e5);
    }

    // Check the supply hasn't come up meanwhile.
    if (!COMP_ON) {
        power_off(); // Shutdown node.
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

    // System setup.
    io_init();
    clock_init();
    spi_init();
    zeta_init();

    // 1ms delay to wait for comparator output to be set.
    __delay_cycles(24e3);
    if (!COMP_ON) {
        node_inactive();
    }

    node_active();
}

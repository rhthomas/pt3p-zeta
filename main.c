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

#include "util.h" // System setup functions
#include "uart.h" // Debugging
#include "zeta.h" // Radio
#include "hibernation.h" // Hibernuss

uint8_t count = 0;
uint8_t in_packet[5u + 4u]; ///< Array for received data.

#define EXT_ON (P1IN & EXT_COMP)

/* Code to execute when Vc > Vactive.
 */
void node_active(void)
{
    // Count up some LEDs as demonstration.
    while (1) {
        led_set(n++);
        __delay_cycles(24e5);
    }
}

/* Code to execute when Vmin <= Vc <= Vactive.
 *
 * Receive packet and store it into NVM.
 */
void node_inactive(void)
{
    // Set Rx mode.
    zeta_select_mode(1);
    // Operating on channel 0 with packet size of 9 bytes.
    zeta_rx_mode(CHANNEL, (5u + 4u));
    // Start timeout timer.
    timer_start();
    // Get incoming packet.
    zeta_rx_packet(in_packet);
    // Start timeout timer.
    timer_stop();
    // Write to NVM.
    /// @todo Write received packet to NVM.
    // Shutdown node.
    power_off();
}

void main(void)
{
    // Stop watchdog timer.
    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;

    // System setup.
    io_init();
    clock_init();
    spi_init();
    zeta_init();

    // Check what is gating the power-supply.
    if (EXT_ON) {
        uart_init();
        // Test whether system needs a restore.
        Hibernus();
        node_active();
    } else {
        node_inactive();
    }

    // Should never get here!
    while (1)
        ;
}

/* Timeout counter.
 *
 * While the node is trying zeta_rx_packet(), if it isn't handled within 2s then
 * we assume false wake-up and return to sleep.
 */
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
    // Timer triggers every second, add delay.
    if (count++ == 2) {
        // Reset counter for next time.
        count = 0;
        // Stop timer.
        timer_stop();
        // Cut power to the node.
        power_off();
    }
}

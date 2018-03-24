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
#include "hibernation.h" // Hibernus

volatile uint8_t exit_loop = 0;

/* Code to execute when gated by UB20.
 */
void node_inactive(void)
{
    zeta_select_mode(1); // Set Rx mode.
    // Operating on channel 15 with packet size of 64 bytes.
    zeta_rx_mode(CHANNEL, 64u);
    timer_start();       // Start timeout timer.
    zeta_rx_packet();    // Get incoming packet and write to mailbox FIFO.
    timer_stop();        // Stop timeout timer.
    // Check the supply hasn't come up meanwhile.
    if (!COMP_ON) {
        power_off();     // Shutdown node.
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
    timer_init();
    spi_init();
    zeta_init();

    // 1ms delay to wait for comparator output to be set.
    /// @note Probably won't be necessary since radio setup takes a while.
    __delay_cycles(24e3);
    if (!COMP_ON) {
        node_inactive();
    }

    node_active();
}

/* Timeout counter.
 *
 * While the node is trying zeta_rx_packet(), if it isn't handled within 1s then
 * we assume false wake-up and return to sleep.
 */
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
    timer_stop(); // Stop timer.
    exit_loop = 1;
}

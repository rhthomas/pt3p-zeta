/* Unified main file for project operation.
 *
 * The operation is described as follows:
 * 1. Initialise I/O ports and clock settings.
 * 2. Initialise hibernus++ to monitor the power supply.
 * 3. Initialise the UART peripheral at 115200 8N1.
 * 4. Initialise the SPI peripheral for interfacing with the ZetaPlus.
 * 5. Initialise the radio.
 * 6. Enter LPM4.5 and wait for an external interrupt from the UB20 on
 *    pin P4.0.
 * 7. Triggering ISR clears the interrupt flag and releases the MCU from
 *    LPM4.5 (now active).
 * 8. MCU then handles the radio reception and returns to LPM4.5 (repeat
 *    indefinitely from 6).
 */

#include "util.h" // System setup functions
#include "uart.h" // Debugging
#include "zeta.h" // Radio
#include "hibernation.h" // Hibernus
//#include "RESTOP_func.h" // RESTOP add-on
//#include "Config.h" // RESTOP configuration

uint8_t rx_flag = 0;
uint8_t count = 0;
uint8_t in_packet[5u + 4u]; ///< Array for received data.

void main(void)
{
    // <- Exit from LPMx.5 starts here.
    // Stop watchdog timer.
    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;

    // System setup.
    io_init();
    clock_init();
    spi_init();
    zeta_init();
    uart_init();
    /// @todo Add RESTOP commands for ZetaPlus startup.

    if (SYSRSTIV == SYSRSTIV_LPM5WU) {
        // System woke-up from LPMx.5.
        // UB20/Hibernus ISR will now trigger.
        __bis_SR_register(GIE);
    } else {
        // System woke-up from "cold start".
        // Sleep and wait for interrupt from UB20.
        __bis_SR_register(GIE);
        enter_lpm5(4);
    }

    // Enter necessary ISR.
    // ...

    // Test whether system needs a restore.
    Hibernus();

    // If interrupted by UB20, get the incoming packet.
    if (rx_flag) {
        // Set Rx mode.
        /// @todo Do you need this?
        // zeta_select_mode(1);
        // Operating on channel 0 with packet size of 9 bytes.
        zeta_rx_mode(CHANNEL, (5u + 4u));

        // Start timeout timer.
        timer_start();
        // Get incoming packet.
        zeta_rx_packet(in_packet);
        // Start timeout timer.
        timer_stop();

        // Short delay after Rx'ing packet.
        /// @todo Is this necessary?
        // __delay_cycles(48e4); // ~0.020s
        // Put radio to sleep.
        zeta_select_mode(3);
        break;
    }

    // Go to sleep and wait for next interrupt from UB20.
    enter_lpm5(4);
}

/* Triggers when there is a packet incoming.
 */
#pragma vector=PORT4_VECTOR
__interrupt void PORT4_ISR(void)
{
    switch (__even_in_range(P4IV, P4IV_P4IFG0)) {
    case P4IV_P4IFG0:
        rx_flag = 1;
    default:
        break;
    }
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
    if (++count == 2) {
        // Reset counter for next time.
        count = 0;
        // Stop timer.
        timer_stop();
        // Go to sleep.
        enter_lpm5(4);
    }
}

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
#include "hibernation.h" // Hibernus++
//#include "RESTOP_func.h" // RESTOP add-on
//#include "Config.h" // RESTOP configuration

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
    Hibernus();
    uart_init();
    spi_init();
    zeta_init();
    /// @todo Add RESTOP commands for ZetaPlus startup.

    if (SYSRSTIV == SYSRSTIV_LPM5WU) {
        // System woke-up from LPMx.5.
        exit_lpm5();
        // RTC/UB20 ISR will now trigger.
        __bis_SR_register(GIE);
    } else {
        // System woke-up from "cold start".
        // Sleep and wait for interrupt from UB20.
        rtc_init();
        __bis_SR_register(GIE);
        enter_lpm5();
    }

    // Enter necessary ISR.
    // ...
    // Return here.

    // Go to sleep and wait for next interrupt from UB20 or RTC.
    enter_lpm5(3);
}

/* Node sleeps when not receiving or transmitting data. When the timer pops,
 * sample any connected sensors, construct a packet and transmit it.
 */
#pragma vector=RTC_VECTOR
__interrupt void RTC_ISR(void)
{
    switch (__even_in_range(RTCIV, RTCIV_RT1PSIFG)) {
    case RTCIV_RT1PSIFG:
        // Sample sensor.
        // Construct packet.
        // Transmit.
        /// @test RTC is waking CPU from sleep.
        led_set(0xFF);
        break;
    default:
        break;
    }
}

/* Triggers when there is a packet incoming.
 */
#pragma vector=PORT4_VECTOR
__interrupt void PORT4_ISR(void)
{
    switch (__even_in_range(P4IV, P4IV_P4IFG0)) {
    case P4IV_P4IFG0:
        // Set Rx mode.
        zeta_select_mode(1);
        // Operating on channel 0 with packet size of 9 bytes.
        zeta_rx_mode(CHANNEL, (5u + 4u));
        // Get incoming packet.
        zeta_rx_packet(in_packet);
        // Short delay after Rx'ing packet.
        /// @todo Is this necessary?
        __delay_cycles(48e4); // ~0.020s
        // Put radio to sleep.
        zeta_select_mode(3);
        break;
    default:
        break;
    }
}

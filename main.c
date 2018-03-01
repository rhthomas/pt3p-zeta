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
    // Stop watchdog timer.
    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;

    io_init();
    clock_init();
    Hibernus();
    uart_init();
    spi_init();
    zeta_init();
    /// @todo Add RESTOP commands for ZetaPlus startup.

    __bis_SR_register(GIE);

#ifdef USE_LPM45
    if (SYSRSTIV == SYSRSTIV_LPM5WU) {
        // System woke-up from LPM4.5.
        // Continue to while loop and receive packet.
    } else {
        // System woke-up from "cold start".
        // Sleep and wait for interrupt from UB20.
        enter_lpm45();
    }
#endif // USE_LPM45

    // Main loop.
    while (1) {
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
#ifdef USE_LPM45
        // Go to sleep and wait for next interrupt from UB20.
        enter_lpm45();
#else
        // Re-enable UB20 interrupt when finished.
        P4IE = UB20;
        // Go to sleep and wait for next interrupt from UB20.
        __bis_SR_register(LPM4_bits + GIE);
#endif // USE_LPM45
    }
}

#ifndef USE_LPM45
/*
 * Used for waking up the processor, this ISR clears the LPM4 bits which puts
 * the processor back into active mode. When leaving the ISR, the code then
 * continues from after __bis_SR_register, of which contains the radio code
 * (does the talking/listening).
 */
#pragma vector=PORT4_VECTOR
__interrupt void PORT4_ISR(void)
{
    switch (__even_in_range(P4IV, P4IV_P4IFG0)) {
    case P4IV_P4IFG0:
        // Clear P4.0 interrupt flag.
        P4IFG &= ~UB20;
        // Disable interrupts.
        P4IE &= ~UB20;
        // Clear LPM4 bits on exit from ISR.
        __bic_SR_register_on_exit(LPM4_bits);
        break;
    default:
        break;
    }
}
#endif // USE_LPM45

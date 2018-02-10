/**
 * @file main.c
 * @author Rhys Thomas <rt8g15@soton.ac.uk>
 * @date 2018-02-09
 *
 * @brief Combined code of all modules, demonstrating the project
 * solution.
 *
 * The operation is described as follows:
 * 1. Initialise I/O ports and clock settings.
 * 2. Initialise hibernus++ to monitor the power supply.
 * 3. Initialise the UART peripheral at 115200 8N1.
 * 4. Initialise the SPI peripheral for interfacing with the ZetaPlus.
 * 5. Initialise the radio.
 * 6. Enter LPM4.5 and wait for an external interrupt from the UB20 on
 *    pin P1.4.
 * 7. Triggering ISR clears the interrupt flag and releases the MCU from
 *    LPM4.5 (now active).
 * 8. MCU then handles the radio reception and returns to LPM4.5 (repeat
 *    indefinitely from 6).
 */

// Libraries.
#include "setup.h" // System setup functions
#include "hibernation.h" // Hibernus++
#include "RESTOP_func.h" // RESTOP add-on
#include "Config.h" // RESTOP configuration
#include "uart.h" // Debugging
#include "zeta.h" // Radio

uint8_t in_packet[PACKET_SIZE]; ///< Array for received data.

/**
 * Main loop.
 */
void main(void)
{
    // Stop watchdog timer.
    WDTCTL = WDTPW | WDTHOLD;

    // Setup peripherals.
    io_init();
    clock_init();
    // TODO This has its own clock/port setup code. Check this.
    Hibernus();
    uart_init();
    spi_init();
    zeta_init();

    // TODO Add RESTOP commands for ZetaPlus startup.

    // Main loop.
    while (1) {
        // Go to sleep.
        __bis_SR_register(LPM4_bits | GIE);
        // Set Rx mode.
        zeta_select_mode(1);
        // Operating on channel 0 with packet size of 12 bytes.
        zeta_rx_mode(CHANNEL, PACKET_SIZE);
        // Get incoming packet.
        zeta_rx_packet(&in_packet);
        // Short delay after Rx'ing packet.
        __delay_cycles(48e4); // ~0.020s
        // Put radio to sleep.
        zeta_select_mode(3);
        // Re-enable UB20 interrupt when finished.
        P1IE = BIT4;
    }
}

// TODO Currently interrupting on P1.4, change this.
#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{
    P1IFG &= ~BIT4; // Clear P1.4 interrupt flag.
    P1IE = 0; // Disable interrupts.
    /* This clears the LPM4 bits which puts the processor back into
     * active mode. When leaving the ISR, the code then continues from
     * after __bis_SR_register, of which contains the radio code (does
     * the talking). */
    __bic_SR_register_on_exit(LPM4_bits);
}

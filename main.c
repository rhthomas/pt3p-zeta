/**
 * @file main.c
 * @author Rhys Thomas <rt8g15@soton.ac.uk>
 * @date 2018-02-09
 *
 * @brief Combined code of all modules, demonstrating the project solution.
 */

#include "setup.h" // System setup functions
#include "uart.h" // Debugging
#include "zeta.h" // Radio
#include "hibernation.h" // Hibernus++
//#include "RESTOP_func.h" // RESTOP add-on
//#include "Config.h" // RESTOP configuration

//#define USE_LPM_4_5 ///< Define to enter LPM4.5 at best, else LPM4 only.

uint8_t in_packet[5u + 4u]; ///< Array for received data.

/**
 * @brief Call initialisation functions.
 *
 * Used when exiting LPM4.5 in the ISR.
 *
 * @see low_power_mode_4_5
 * @see PORT4_ISR
 */
void setup(void);

/**
 * @brief Sets the necessary registers to enter LPM4.5.
 *
 * @see Section 1.4 in the 57xx user guide.
 */
inline void low_power_mode_4_5(void);

/**
 * @brief Main loop.
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
void main(void)
{
    // Stop watchdog timer.
    WDTCTL = WDTPW | WDTHOLD;

    // Setup peripherals.
    setup();

    // Main loop.
    while (1) {
        // Go to sleep and wait for interrupt from UB20.
#ifdef USE_LPM_4_5
        low_power_mode_4_5();
#else
        __bis_SR_register(LPM4_bits + GIE);
#endif // USE_LPM_4_5
        // Set Rx mode.
        zeta_select_mode(1);
        // Operating on channel 0 with packet size of 9 bytes.
        zeta_rx_mode(CHANNEL, (5u + 4u));
        // Get incoming packet.
        zeta_rx_packet(in_packet);
        // Short delay after Rx'ing packet.
        __delay_cycles(48e4); // ~0.020s
        // Put radio to sleep.
        zeta_select_mode(3);
        // Re-enable UB20 interrupt when finished.
        P4IE = UB20;
    }
}

void setup(void)
{
    io_init();
    clock_init();
    Hibernus();
    uart_init();
    spi_init();
    zeta_init();
    /// @todo Add RESTOP commands for ZetaPlus startup.
}

inline void low_power_mode_4_5(void)
{
    // Enable interrupts.
    __bis_SR_register(GIE);
    // Unlock PMM registers.
    PMMCTL0_H = 0xA5;
    // Disable SVS module.
    PMMCTL0_L &= ~(SVSHE + SVSLE);
    // Clear LPM4.5 IFG.
    PMMIFG &= ~PMMLPM5IFG;
    // Enter LPM4.5
    PMMCTL0_L |= PMMREGOFF;
    __bis_SR_register(LPM4_bits);
}

/**
 * @brief ISR that is triggered by UB20.
 *
 * Used for waking up the processor, this ISR clears the LPM4 bits which puts
 * the processor back into active mode. When leaving the ISR, the code then
 * continues from after __bis_SR_register, of which contains the radio code
 * (does the talking/listening).
 *
 * @todo Exit LPM4.5 code? Reconfigure IO, system and unlock the system.
 */
#pragma vector=PORT4_VECTOR
__interrupt void PORT4_ISR(void)
{
#ifdef USE_LPM_4_5
    // Reconfigure system and IO as required for the application.
    setup();
    // Unlock the system
    PM5CTL0 &= ~LOCKLPM5;
#endif // USE_LPM_4_5
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

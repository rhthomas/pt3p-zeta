/**
 * @file test-radio-tx.c
 * @author Rhys Thomas <rt8g15@soton.ac.uk>
 * @date 2018-02-10
 *
 * @brief Testing transmission on the radio module.
 */

// Libraries.
#include "setup.h" // System setup functions
#include "zeta.h" // Radio

///< Dummy test packet to send, 12-bytes of 0b10101010.
uint8_t msg[12u] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
        0xAA, 0xAA
};

void Set_Binary_value(unsigned int v);

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
    timer_init();
    spi_init();
    zeta_init();

    zeta_select_mode(2u);

    // Main loop.
    while (1) {
        // Go to sleep. Wait for timer interrupt.
        __bis_SR_register(LPM3_bits | GIE);
        // Send packet. Returns to previous state (sleep) when done.
//        zeta_send_packet(msg, sizeof(msg));
        zeta_get_vers();
    }
}

/**
 * Timer overflow ISR.
 *
 * Triggers every ~1s and exits the processor from LPM3. The MCU then sends a
 * packet onto the ether, sleeps the radio and returns to LPM3.
 */
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_ISR(void)
{
    P3OUT ^= BIT7; // Toggle LED to show you're in ISR.
    // Exit into active mode and disable interrupts while Tx'ing.
    __bic_SR_register_on_exit(LPM3_bits | GIE);
}

/// @todo Write a better implementation than this.
void Set_Binary_value(unsigned int v)
{
    PJOUT &= ~(BIT0);
    PJOUT &= ~(BIT1);
    PJOUT &= ~(BIT2);
    PJOUT &= ~(BIT3);
    P3OUT &= ~(BIT4);
    P3OUT &= ~(BIT5);
    P3OUT &= ~(BIT6);
    P3OUT &= ~(BIT7);

    PJOUT |= (BIT0 & (v & 0x0001));
    PJOUT |= (BIT1 & (v & 0x0002));
    PJOUT |= (BIT2 & (v & 0x0004));
    PJOUT |= (BIT3 & (v & 0x0008));
    P3OUT |= (BIT4 & (v & 0x0010));
    P3OUT |= (BIT5 & (v & 0x0020));
    P3OUT |= (BIT6 & (v & 0x0040));
    P3OUT |= (BIT7 & (v & 0x0080));
}

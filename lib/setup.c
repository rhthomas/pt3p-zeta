#include "setup.h"

void io_init(void)
{
    // All ports output.
    PADIR = 0xFFFF;
    PBDIR = 0xFFFF;
    PJDIR = 0xFFFF;
    // All outputs low.
    PAOUT = 0;
    PBOUT = 0;
    PJOUT = 0;
    // Reset interrupt flags.
    PAIFG = 0;
    PBIFG = 0;
    /// @todo Setup UB20 interrupt on P1.3.

    /// @test Set PJ pins as clock outputs.
    PJSEL0 |= (BIT2 | BIT1 | BIT0);
}

void clock_init(void)
{
    // Unlock CS registers.
    CSCTL0_H = 0xA5;
    // Set DCO to 8MHz.
    CSCTL1 |= DCOFSEL0 + DCOFSEL1;
    // ACLK = VLO, SMCLK = MCLK = DCO
    CSCTL2 = SELA_1 + SELS_3 + SELM_3;
    // ACLK/1, SMCLK/8, MCLK/1
    CSCTL3 = DIVA_0 + DIVS_3 + DIVM_0; // set all dividers
    // Power down unused clocks.
    CSCTL4 = XT1OFF + XT2OFF;
}

void timer_init(void)
{
    // ACLK, upmode, clear.
    TA0CTL |= (TASSEL__ACLK | MC_1);
    TA0CCTL0 = CCIE; // CCR0 interrupt enabled.
    TA0CCR0 = 0x2711; // ~1s delay.
}

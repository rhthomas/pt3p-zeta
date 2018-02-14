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
    /// @bug DCO should be 8MHz but is 1MHz for some reason.
    CSCTL1 &= ~(DCORSEL);
    CSCTL1 |= (DCOFSEL0 | DCOFSEL1);
    // ACLK VLOCLK, SMCLK = MCLK = DCOCLK
    CSCTL2 |= (SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK);
    // No clock prescales.
    CSCTL3 |= (DIVA__1 | DIVS__1 | DIVM__1);
    // Power down unused clocks.
    CSCTL4 |= (XT1OFF | XT2OFF);
    // Lock the registers.
    CSCTL0_H = 0;
}

void timer_init(void)
{
    // ACLK, upmode, clear.
    TA0CTL |= (TASSEL__ACLK | MC_1);
    TA0CCTL0 = CCIE; // CCR0 interrupt enabled.
    TA0CCR0 = 0x2711; // ~1s delay.
}
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
    CSCTL0_H = 0xA5; // Unlock CS registers.
    CSCTL1 &= ~(DCORSEL);
    CSCTL1 |= DCOFSEL0 | DCOFSEL1; // 24MHz
    // ACLK = SMCLK = MCLK = DCOCLK
    CSCTL2 |= SELA_1 | SELS_3 | SELM_3;
    // SMCLK=DCO/2
    CSCTL3 |= DIVA_0 | DIVS_0 | DIVM_0;
    CSCTL0_H = 0; // Lock the registers.
}

void timer_init(void)
{
    // ACLK, upmode, clear.
    TA0CTL |= TASSEL__ACLK | MC_1;
    TA0CCTL0 = CCIE; // CCR0 interrupt enabled.
    TA0CCR0 = 0x2710; // ~1s delay.
}

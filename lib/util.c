#include <util.h>

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
    // Set UB20 on P4.0 as input (no pull-up) with interrupts enabled.
//    P4DIR &= ~(UB20);
//    P4REN &= ~(UB20);
//    P4IE |= UB20;
    /* Set external comparator on P1.0 as input (no pull-up) with interrupts
     * enabled.
     */
    P1IFG = 0;
    P1DIR &= ~(EXT_COMP); // Set P1.0 as an input.
    P1REN &= ~(EXT_COMP); // Disable pull resistors.
    P1IE |= EXT_COMP;
}

void clock_init(void)
{
    // Unlock CS registers.
    CSCTL0_H = 0xA5;
    // Set DCO to 24MHz.
    CSCTL1 |= DCORSEL + DCOFSEL0 + DCOFSEL1;
    // ACLK = VLO, SMCLK = MCLK = DCO
    CSCTL2 = SELA_1 + SELS_3 + SELM_3;
    // ACLK/1, SMCLK/8, MCLK/1
    CSCTL3 = DIVA_0 + DIVS_3 + DIVM_0;
    // Power down unused clocks (used for LPM4.5).
    CSCTL4 = XT1OFF + XT2OFF;
    // Lock clock registers.
    CSCTL0_H = 0;
}

void timer_init(void)
{
    // ACLK, upmode, clear.
    TA0CTL |= (TASSEL__ACLK | MC_1);
    TA0CCTL0 = CCIE; // CCR0 interrupt enabled.
//    TA0CCR0 = 0x2711; // ~1s delay.
    TA0CCR0 = 0x0400; // ~1s delay.
}

void led_set(uint8_t byte)
{
    PJOUT = (byte & 0x0F);
    P3OUT = (byte & 0xF0);
}

void led_clear(void)
{
    PJOUT = 0;
    P3OUT = 0;
}

#ifdef USE_LPM45
inline void enter_lpm45(void)
{
    // Unlock PMM registers.
    PMMCTL0_H = 0xA5;
    // Disable SVS module.
    PMMCTL0_L &= ~(SVSHE + SVSLE);
    // Enter LPM4.5
    PMMCTL0_L |= PMMREGOFF;
    // Lock PMM registers.
    PMMCTL0_H = 0;
    // Enter LPM4.5
    __bis_SR_register(LPM4_bits + GIE);
}
#endif // USE_LPM45

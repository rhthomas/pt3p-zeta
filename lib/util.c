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
    // Set power-supply latch as output, and immediately drive high.
    // P4DIR |= PS_LATCH;
    P4OUT |= PS_LATCH;
    /* Set external comparator on P1.0 as input (no pull-up) with interrupts
     * enabled.
     */
    P1DIR &= ~EXT_COMP; // Set P1.0 as an input.
    P1REN &= ~EXT_COMP; // Disable pull resistors.
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
    CSCTL3 |= DIVA_0 + DIVS_3 + DIVM_0;
    // Power down unused clocks.
    CSCTL4 = XT1OFF + XT2OFF;
    // Lock clock registers.
    CSCTL0_H = 0;
}

void timer_init(void)
{
    // ACLK, upmode, clear.
    TA0CTL |= (TASSEL__ACLK | MC_1);
    TA0CCTL0 = CCIE; // CCR0 interrupt enabled.
    TA0CCR0 = 0x2711; // ~1s delay.
}

inline void timer_start(void)
{
    TA0CTL |= MC_1;
}

inline void timer_stop(void)
{
    TA0CTL &= ~MC_1;
    timer_reset();
}

inline void timer_reset(void)
{
    TA0R = 0;
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

#ifdef USE_LPM5
inline void enter_lpm5(void)
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
#endif // USE_LPM5

#ifdef USE_SHDN
inline void power_off(void)
{
    P4OUT &= ~PS_LATCH;
}
#endif // USE_SHDN

#include <util.h>

void io_init(void)
{
    // PORT 1
    P1DIR = 0xFF & ~(EXT_COMP);
    P1OUT = 0;
    P1REN = 0; //EXT_COMP;
    P1IE |= EXT_COMP;
    // PORT 4
    P4DIR = 0xFF;
    P4OUT &= ~PS_LATCH;
    // PORT 2
    P2DIR = 0xFF;
    P2OUT = 0;
    // PORT 3
    P3DIR = 0xFF;
    P3OUT = 0;
    // PORT J
    PJDIR = 0xFF;
    PJOUT = 0;
}

void clock_init(void)
{
    // Unlock CS registers.
    CSCTL0_H = 0xA5;
    // Set DCO to 24MHz.
    CSCTL1 = DCORSEL + DCOFSEL0 + DCOFSEL1;
    // ACLK = VLO, SMCLK = MCLK = DCO
    CSCTL2 = SELA_1 + SELS_3 + SELM_3;
    // ACLK/1, SMCLK/8, MCLK/1
    CSCTL3 = DIVA_0 + DIVS_3 + DIVM_0;
    // Power down unused clocks.
    CSCTL4 = XT1OFF + XT2OFF;
    // Lock clock registers.
    CSCTL0_H = 0;
}

void timer_start(void)
{
    // ACLK, upmode, clear.
    TA0CTL |= (TASSEL__ACLK + MC_1);
    TA0CCTL0 = CCIE; // CCR0 interrupt enabled.
    TA0CCR0 = 0x4E21; // ~2s delay.
    __bis_SR_register(GIE); // Enable interrupts for timeout.
}

void timer_stop(void)
{
    TA0CTL = MC_0; // Stop counting.
    TA0R = 0;      // Reset counter.
    __bic_SR_register(GIE);
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

void led_flash(void)
{
    uint8_t i;
    for (i = 0; i < 5; i++) {
        led_set(0xFF);
        __delay_cycles(4.8e5);
        led_clear();
        __delay_cycles(4.8e5);
    }
}

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

inline void power_off(void)
{
    // Active-low shut off when driving a PMOS.
    P4OUT |= PS_LATCH;
}

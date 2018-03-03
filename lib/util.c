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
    // Set UB20 on P4.0 as input (no pull-up) with interrupts enabled.
//    P4DIR &= ~(UB20);
//    P4REN &= ~(UB20);
//    P4IE |= UB20;
    /* Set external comparator on P1.0 as input (no pull-up) with interrupts
     * enabled.
     */
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
//    do {
//        CSCTL5 &= ~XT1OFFG; // Clear XT1 fault flag
//        SFRIFG1 &= ~OFIFG;
//    } while (SFRIFG1 & OFIFG); // Test oscillator fault flag
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

void rtc_init(void)
{
    // Configure RTC_B
    // Configure BCD mode, stop the RTC, and enable RTC
    RTCCTL01 = RTCBCD | RTCHOLD | RTCTEV_0;
    // Set the RTC second stage divider to 256
    // RTC interrupts every 2 seconds
    RTCPS1CTL = RT1IP_7 | RT1PSIE;
    // Start RTC calendar mode
    RTCCTL01 &= ~RTCHOLD;
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
inline void enter_lpm5(uint8_t mode)
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
    if (mode == 3) {
        __bis_SR_register(LPM3_bits + GIE);
    } else if (mode == 4) {
        __bis_SR_register(LPM4_bits + GIE);
    }
}

inline void exit_lpm5(void)
{
    // Re-configure / start RTC
    // Configure BCD mode, stop the RTC, and enable RTC
    RTCCTL1 |= RTCBCD | RTCHOLD | RTCTEV_0;
    // Set the RTC second stage divider to 256
    // RTC interrupts every 2 seconds
    RTCPS1CTL = RT1IP_7 | RT1PSIE;
    // Start RTC calendar mode
    RTCCTL1 &= ~RTCHOLD;
}
#endif // USE_LPM45

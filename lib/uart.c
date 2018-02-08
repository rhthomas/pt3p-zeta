#include "uart.h"

void uart_init(void)
{
    // USCI_A1 UART operation.
    // P2SEL[1:0] = b10
    P2SEL1 |= TXPIN | RXPIN;
    P2SEL0 &= ~(TXPIN | RXPIN);
    // UART clock uses SMCLK.
    UCA1CTLW0 |= UCSSEL__SMCLK | UCSWRST;
    // 12MHz SMCLK, 115200 baud, UCOS16=1, UCBRx=6, UCBRFx=8, UCBRSx=0x20
    UCA1MCTLW |= 0x2081;
    UCA1BRW = 6;
    // Initialise eUSCI
    UCA1CTLW0 &= ~UCSWRST;
}

void uart_putc(char ch)
{
    // Add character to UART buffer.
    UCA0TXBUF = ch;
    // Enable UART interrupts.
    UCA0IE |= UCTXIE;
    // Wait until Tx buffer is ready.
    __bis_SR_register(LPM3_bits | GIE);
}

void uart_puts(char *string)
{
    while (*string++) {
        // TODO Is this correct?
        uart_putc(*string);
    }
}

#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
    switch (__even_in_range(UCA0IV, 4)) {
    case 0: // No interrupts
        break;
    case 2: // UCRXIFG
        // Could potentially send the Rx'd data over RF here?
        break;
    case 4: // UCTXIFG
        // Disable UART interrupts.
        UCA0IE &= ~UCTXIE;
        // Exit to active CPU.
        __bic_SR_register(LPM3_bits);
        break;
    default:
        break;
    }
}

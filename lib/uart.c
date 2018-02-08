#include "uart.h"

void uart_init(void)
{
    // USCI_A0 UART operation.
    // P2SEL[1:0] = b10
    P2SEL1 |= TXPIN | RXPIN;
    P2SEL0 &= ~(TXPIN | RXPIN);
    // UART clock uses SMCLK.
    UCA0CTL1 |= UCSWRST;
    UCA0CTL1 = UCSSEL__SMCLK;
    // 12MHz SMCLK, 115200 baud, UCOS16=1, UCBRx=6, UCBRFx=8, UCBRSx=0x20
    UCA0MCTLW |= 0x2081;
    UCA0BRW = 6;
    // Initialise eUSCI
    UCA0CTLW0 &= ~UCSWRST;
    // Enable UART interrupts.
    UCA0IE |= UCTXIE;
}

void uart_putc(char ch)
{
    // Enable UART interrupts.
    UCA0IE |= UCTXIE;
    // Add character to UART buffer.
    UCA0TXBUF = ch;
    // Wait until Tx buffer is ready.
    __bis_SR_register(GIE);
}

void uart_puts(char *string)
{
    while (*string++) {
        // TODO Is this correct?
        uart_putc(*string);
    }
}

#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
    switch (__even_in_range(UCA0IV, 4)) {
    case 0: // No interrupts
        break;
    case 2: // UCRXIFG
        break;
    case 4: // UCTXIFG
        // Disable UART interrupts.
        UCA0IE &= ~UCTXIE;
        // Exit to active CPU.
//        __bic_SR_register(LPM3_bits);
        break;
    default:
        break;
    }
}

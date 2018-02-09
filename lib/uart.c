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
}

void uart_putc(unsigned char ch)
{
    while (!(UCA0IFG & UCTXIFG))
        ;
    UCA0TXBUF = ch;
}

void uart_puts(const char *string)
{
    while (*string) {
        uart_putc(*string++);
    }
}

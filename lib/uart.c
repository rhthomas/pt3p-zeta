#include "uart.h"

void uart_init(void)
{
    // USCI_A0 UART operation.
    // P2SEL[1:0] = b10
    P2SEL1 |= TXPIN | RXPIN;
    P2SEL0 &= ~(TXPIN | RXPIN);
    // UART clock uses SMCLK.
    UCA1CTL1 |= UCSWRST;
    UCA1CTL1 = UCSSEL__SMCLK;
    // 1MHz SMCLK, 115200 baud, UCOS16=0, UCBRx=8, UCBRFx=-, UCBRSx=0xD6
    UCA1MCTLW |= 0xD600;
    UCA1BRW = 8;
    // Initialise eUSCI
    UCA1CTLW0 &= ~UCSWRST;
}

void uart_putc(unsigned char ch)
{
    while (!(UCA1IFG & UCTXIFG))
        ;
    UCA1TXBUF = ch;
}

void uart_puts(const char *string)
{
    while (*string) {
        uart_putc(*string++);
    }
}

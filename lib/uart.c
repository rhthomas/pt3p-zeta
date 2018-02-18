#include "uart.h"

void uart_init(void)
{
    // USCI_A0 UART operation.
    // P2SEL[1:0] = b10
    P2SEL1 |= TXPIN | RXPIN;
    P2SEL0 &= ~(TXPIN | RXPIN);
    // Reset UART state machine.
    UCA1CTL1 |= UCSWRST;
    // UART clock uses SMCLK.
    UCA1CTL1 = UCSSEL__SMCLK;
    // 3MHz SMCLK, 115200 baud, UCBRSx=0x01, UCBRFx=0, UCOS16=0,
    UCA1MCTLW |= 0x0100;
    // UCBRx=26
    UCA1BRW = 26;
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

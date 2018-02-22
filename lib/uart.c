#include "uart.h"

void uart_init(void)
{
    // USCI_A0 UART operation.
    P2SEL1 |= TXPIN | RXPIN;
    P2SEL0 &= ~(TXPIN | RXPIN);
    // Reset UART state machine.
    UCA0CTL1 |= UCSWRST;
    // UART clock uses SMCLK.
    UCA0CTL1 = UCSSEL__SMCLK;
    // 3MHz SMCLK, 115200 baud, UCBRSx=0x01, UCBRFx=0, UCOS16=0,
    UCA0MCTLW |= 0x0100;
    // UCBRx=26
    UCA0BRW = 26;
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

#include "uart.h"

void uart_init(void)
{
    // USCI_A0 UART operation. TODO Check pins.
    // P2SEL[1:0] = b10
    P2SEL1 |= BIT1 | BIT0;
    P2SEL0 &= ~(BIT1 | BIT0);
    // UART clock uses SMCLK. TODO These registers clash with SPI. Change one.
    UCA0CTLW0 |= UCSSEL__SMCLK | UCSWRST;
    // 12MHz SMCLK, 115200 baud, UCOS16=1, UCBRx=6, UCBRFx=8, UCBRSx=0x20
    UCA0MCTLW |= 0x2081;
    UCA0BRW = 6;
    // Initialise eUSCI
    UCA0CTLW0 &= ~UCSWRST;
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

#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
    switch (__even_in_range(UCA0IV, 18)) {
    case 0: // No interrupts
        break;
    case 2: // UCRXIFG
        // TODO What happens if Rx buffer has stuff?
        break;
    case 4: // UCTXIFG
        // TODO What happens if Tx buffer has stuff?
        break;
    case 6: // UCSTTIFG
        // TODO What is this?
        break;
    case 8: // UCTXCPTIFG
        // TODO What is this?
        break;
    default:
        break;
    }
}

#include "spi.h"

void spi_init(void)
{
    P1DIR |= SCLK;
    P1OUT |= SCLK;

    P2DIR |= MOSI;
    P2DIR &= ~MISO;
    // TODO pullups?

    // Secondary functionality of pins
    P1SEL1 |= SCLK;
    P2SEL1 |= MOSI + MISO;

    UCA0CTLW0 |= UCSWRST; // reset state-machine
    UCA0CTLW0 |= UCMST + // 3 pin, 8-bit SPI master
    UCSYNC + // sync mode
    UCCKPL + // clock polarity high
    UCMSB; // MSB first
    UCA0CTLW0 |= UCSSEL_2; // SMCLK
    UCA0BR0 |= 0x02; // /2 TODO what is SMCLK running at?
    UCA0BR1 |= 0;
    UCA0MCTLW |= 0; // no modulation
    UCA0CTLW0 &= ~UCSWRST; // initialise the state-machine
    UCA0IE |= UCRXIE; // enable USCI_A0 RX interrupt
}

uint8_t spi_xfer(uint8_t byte)
{
    while (!(UCA0IFG & UCA0TXBUF))
        ; // wait until tx buffer is ready
    UCA0TXBUF = byte;
    while (!(UCA0IFG & UCA0RXBUF))
        ; // wait until rx buffer is ready
    return UCA0RXBUF;
}

// TODO how to use this with spi_xfer()?
// #pragma vector=USCI_A0_VECTOR
// __interrupt void USCI_A0_ISR(void)
// {
//     switch (__even_in_range(UCA0IV, 0x04)) {
//     case 0: break;
//     case 2: // RXIFG
//         // data has been received
//         UCA0IFG &= ~UCRXIFG; // reset interrupt flag
//         __bic_SR_register_on_exit(LPM3_bits);
//         break;
//     case 4: // TXIFG
//         // transmit some data
//         UCA0IE &= ~UCTXIE;
//         break;
//     }
// }

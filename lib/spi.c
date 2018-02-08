#include "spi.h"

void spi_init(void)
{
    P1DIR |= SCLK;
    P1OUT |= SCLK;
    P2DIR |= MOSI;
    P2DIR &= ~MISO;
    // TODO Do inputs need pull-ups?

    // Secondary functionality of pins.
    P1SEL1 |= SCLK;
    P2SEL1 |= MOSI + MISO;

    UCA0CTLW0 |= UCSWRST; // reset state-machine
    // 3 pin, 8-bit SPI master, sync mode, clock polarity high, MSB first
    UCA0CTLW0 |= UCMST | UCSYNC | UCCKPL | UCMSB;
    UCA0CTLW0 |= UCSSEL__SMCLK; // SMCLK
    UCA0BR0 |= 0x02; // /2 TODO What is SMCLK running at?
    UCA0BR1 |= 0;
    UCA0MCTLW |= 0; // no modulation
    UCA0CTLW0 &= ~UCSWRST; // initialise the state-machine
    UCA0IE |= (UCTXIE | UCRXIE); // enable USCI_A0 Tx/Rx interrupts
}

// TODO Test blocking SPI first.
uint8_t spi_xfer(uint8_t byte)
{
    while (!(UCA0IFG & UCA0TXBUF))
        ; // Wait until Tx buffer is ready.
    UCA0TXBUF = byte;
    while (!(UCA0IFG & UCA0RXBUF))
        ; // Wait until Rx buffer is ready.
    return UCA0RXBUF;
}

// TODO Make SPI work by interrupts.
//#pragma vector=USCI_A0_VECTOR
//__interrupt void USCI_A0_ISR(void)
//{
//    switch (__even_in_range(UCA0IV, 4)) {
//    case 0: // No interrupts
//        break;
//    case 2: // RXIFG
//        // Reset interrupt flag.
//        UCA0IFG &= ~UCRXIFG;
//        // Exit to active CPU.
//        __bic_SR_register_on_exit(LPM3_bits);
//        break;
//    case 4: // TXIFG
//        // Disable interrupts.
//        UCA0IE &= ~UCTXIE;
//        // Exit to active CPU.
//        __bic_SR_register_on_exit(LPM3_bits);
//        break;
//    }
//}

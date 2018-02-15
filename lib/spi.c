#include "spi.h"

void spi_init(void)
{
    P1DIR |= (SCLK | CS);
    P1OUT |= CS;
    P2DIR |= MOSI;
    P2DIR &= ~MISO;

    // Secondary functionality of pins.
    P1SEL1 |= SCLK;
    P1SEL1 |= CS;
    P2SEL1 |= (MOSI | MISO);

    // Reset state-machine.
    UCA0CTLW0 |= UCSWRST;
    /* Master mode.
     * 3-pin mode.
     * Synchronous mode.
     * MSB first. */
    UCA0CTLW0 |= (UCMST | UCSYNC | UCMSB);// | UCCKPH);
    // Use USCA0STE pin as chip select.
    UCA0CTLW0 |= (UCMODE1 | UCSTEM);
    // SMCLK drives SPI peripheral.
    UCA0CTLW0 |= UCSSEL_2;
    // No modulation.
    UCA0MCTLW = 0;
    // Run the SPI clk at 1MHz.
    UCA0BRW = 0x0001;
    // initialise the state-machine
    UCA0CTLW0 &= ~UCSWRST;
}

uint8_t spi_xfer(uint8_t byte)
{
    while (!(UCA0IFG & UCTXIFG))
        ; // Wait until Tx buffer is ready.
    UCA0TXBUF = byte;
    while (!(UCA0IFG & UCRXIFG))
        ; // Wait until Rx buffer is ready.
    return UCA0RXBUF;
}

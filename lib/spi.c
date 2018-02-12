#include "spi.h"

void spi_init(void)
{
    P1DIR |= (SCLK | CS);
    spi_cs_high();
    P2DIR |= MOSI;
    P2DIR &= ~MISO;

    // Secondary functionality of pins.
    P1SEL1 |= SCLK;
    P2SEL1 |= (MOSI | MISO);

    UCA0CTLW0 |= UCSWRST; // reset state-machine
    /* Master mode.
     * 3-pin mode.
     * Synchronous mode.
     * MSB first. */
    UCA0CTLW0 |= UCMST | UCSYNC | UCMSB;
    // SMCLK @ 12MHz
    UCA0CTLW0 |= UCSSEL__SMCLK;
    UCA0BR0 |= 0x0b; // Run the SPI clk at 1MHz
    UCA0BR1 |= 0;
    UCA0CTLW0 &= ~UCSWRST; // initialise the state-machine
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

void spi_cs_high(void)
{
    P1OUT |= CS;
}

void spi_cs_low(void)
{
    P1OUT &= ~CS;
}

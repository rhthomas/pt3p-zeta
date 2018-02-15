#include "spi.h"

void spi_init(void)
{
    P1DIR |= (SCLK | CS);
    P1OUT |= CS;
    P2DIR |= MOSI;
    P2DIR &= ~MISO;

    // Secondary functionality of pins.
    P1SEL1 |= SCLK;
//    P1SEL1 |= CS;
    P2SEL1 |= (MOSI | MISO);

    // Reset state-machine.
    UCA0CTLW0 |= UCSWRST;
    /* SPI configuration:
     * 1. Master 3-pin mode.
     * 2. Synchronous.
     * 3. MSB first.
     * 4. Data captured on first clock edge and changed on following.
     * 5. UCA0STE (P1.4) as CS pin.
     * 6. Active low.
     * 7. SMCLK as source. */
    UCA0CTLW0 |= (UCMST /*| UCSYNC*/| UCMSB | UCCKPH);
//    UCA0CTLW0 |= (UCMODE1 | UCSTEM);
    UCA0CTLW0 |= UCSSEL_2;
    // No modulation.
    UCA0MCTLW = 0;
    // Run the SPI clk at 1MHz.
    UCA0BRW_L = 1;
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

void spi_cs_high(void)
{
    P1OUT |= CS;
}

void spi_cs_low(void)
{
    P1OUT &= ~CS;
}

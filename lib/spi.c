#include "spi.h"

void spi_init(void)
{
    P1DIR |= (MOSI + CS);
    P1DIR &= ~MISO;
    P2DIR |= SCLK;

    P1REN |= MISO; // enable pull resistor
    P1OUT |= MISO; // pull up

    // Secondary functionality of pins.
    P2SEL1 |= SCLK;
    P1SEL1 |= (MOSI | MISO);

    // Set automatic CS pin.
#ifndef MANUAL
    P1OUT |= CS;
    P1SEL1 |= CS;
#endif // MANUAL

    // Reset state-machine.
    UCB0CTLW0 |= UCSWRST;
    /* SPI configuration:
     * 1. Master 3-pin mode.
     * 2. Synchronous.
     * 3. MSB first.
     * 4. Data captured on first clock edge and changed on following.
     * x. UCB0STE (P1.3) as CS pin. (not used, now manual)
     * x. Active low. (not used, now manual)
     * 5. SMCLK as source. */
    UCB0CTLW0 |= (UCMST | UCSYNC | UCMSB | UCCKPH);
#ifndef MANUAL
    UCB0CTLW0 |= (UCMODE1 | UCSTEM);
#endif // MANUAL
    UCB0CTLW0 |= UCSSEL_2;
    // Run the SPI clk at 1MHz.
    UCB0BRW_L = 3;
    // initialise the state-machine
    UCB0CTLW0 &= ~UCSWRST;
}

uint8_t spi_xfer(uint8_t byte)
{
    while (!(UCB0IFG & UCTXIFG))
        ; // Wait until Tx buffer is ready.
    UCB0TXBUF = byte;
    while (!(UCB0IFG & UCRXIFG))
        ; // Wait until Rx buffer is ready.
    return UCB0RXBUF;
}

inline void spi_cs_high(void)
{
    P1OUT |= CS;
}

inline void spi_cs_low(void)
{
    P1OUT &= ~CS;
}

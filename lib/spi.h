/**
 * @file spi.h
 * @author Rhys Thomas <rt8g15@soton.ac.uk>
 * @date 2018-01-29
 *
 * @brief Simple SPI library for MSP430FR5739 MCU.
 *
 * @details Pin-out:
 * ```
 * MSP430FR5739
 * -------+
 *        |
 *    P2.0|-> Data Out (UCA0SIMO)
 *    P2.1|<- Data In (UCA0SOMI)
 *        |
 *    P1.4|-> CS (UCA0STE)
 *    P1.5|-> Serial Clock Out (UCA0CLK)
 *        |
 * -------+
 * ```
 */

#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include <msp430.h>

#define CS   (BIT4) ///< Chip select pin for SPI (P1.4).
#define SCLK (BIT5) ///< SPI clock (P1.5).
#define MISO (BIT1) ///< Master-in/Slave-out (P2.1).
#define MOSI (BIT0) ///< Master-out/Slave-in (P2.0).

/**
 * Initialises the SPI peripheral on eUSCI A0.
 *
 * @ingroup init
 */
void spi_init(void);

/**
 * Begin SPI transfer.
 *
 * @param byte : Data to transmit to slave.
 * @return Data received from slave.
 */
uint8_t spi_xfer(uint8_t byte);

//inline void spi_cs_high(void);
//inline void spi_cs_low(void);

#endif // SPI_H

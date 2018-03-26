/**
 * @file spi.h
 * @author Rhys Thomas <rt8g15@soton.ac.uk>
 * @date 2018-01-29
 *
 * @brief Simple SPI library for MSP430FR5739 MCU.
 *
 * Pin-out:
 * ```
 * MSP430FR5739
 * -------+
 *        |
 *    P1.3|-> CS (UCB0STE)
 *    P1.6|-> Data Out (UCB0SIMO)
 *    P1.7|<- Data In (UCB0SOMI)
 *    P2.2|-> Serial Clock Out (UCB0CLK)
 *        |
 * -------+
 * ```
 */

#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include <msp430.h>

//#define MANUAL ///< Uncomment for automatic CS toggling.

#define CS   (BIT3) ///< Chip select pin for SPI (P1.3).
#define SCLK (BIT2) ///< SPI clock (P2.2).
#define MISO (BIT7) ///< Master-in/Slave-out (P1.7).
#define MOSI (BIT6) ///< Master-out/Slave-in (P1.6).

/**
 * @brief Initialises the SPI peripheral on eUSCI A0.
 *
 * SPI configuration:
 * 1. Master 3-pin mode (CS is handled manually).
 * 2. Synchronous.
 * 3. MSB first.
 * 4. Data captured on first clock edge and changed on following.
 * 5. SMCLK @ 1MHz as source.
 *
 * @ingroup init
 * @note ZETAPLUS has a max. SPI frequency of 1.4MHz.
 */
void spi_init(void);

/**
 * @brief Begin SPI transfer.
 *
 * @param byte : Data to transmit to slave.
 * @return Data received from slave.
 */
uint8_t spi_xfer(uint8_t byte);

/**
 * @brief Set the chip select pin high.
 */
inline void spi_cs_high(void);

/**
 * @brief Set the chip select pin low.
 */
inline void spi_cs_low(void);

#endif // SPI_H

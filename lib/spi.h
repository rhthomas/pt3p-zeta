/**
 * @file spi.h
 * @author Rhys Thomas <rt8g15@soton.ac.uk>
 * @date 2018-01-29T14:16
 *
 * @brief Simple SPI library for MSP430FR5739 MCU.
 */
/*
 * MSP430FR5739
 * -------+
 *        |
 *    P2.0|-> Data Out (UCA0SIMO)
 *    P2.1|<- Data In (UCA0SOMI)
 *        |
 *    P1.3|-> CS
 *    P1.5|-> Serial Clock Out (UCA0CLK)
 *        |
 * -------+
 */

#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include <msp430.h>

#define CS   (BIT3) // P1.3 ///< Chip select pin for SPI.
#define SCLK (BIT5) // P1.5
#define MISO (BIT1) // P2.1
#define MOSI (BIT0) // P2.0

/**
 * Initialises the SPI peripheral on eUSCI A0.
 */
void spi_init(void);

/**
 * Begin SPI transfer.
 *
 * @param byte : Data to transmit to slave.
 * @return Data received from slave.
 */
uint8_t spi_xfer(uint8_t byte);

/**
 * Set CS line high.
 */
void spi_cs_high(void);

/**
 * Set CS line low.
 */
void spi_cs_low(void);

#endif // SPI_H

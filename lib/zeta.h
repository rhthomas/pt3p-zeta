/**
 * @file zeta.h
 * @author Rhys Thomas <rt8g15@soton.ac.uk>
 * @date 2018-01-2914:33
 *
 * @brief Library for interfacing with ZetaPlus radio module.
 */

#ifndef ZETA_H
#define ZETA_H

#include <stdint.h>
#include <msp430.h>
#include "spi.h"

#define CS  (BIT3) ///< Chip select pin for SPI.
#define SDN (BIT4) ///< Sleep pin (high, low) = (sleep, active).
#define IRQ (BIT5) ///< Interrupt request.

/**
 * Setup the device.
 */
void zeta_init(void);

/**
 * Wait for interrupt from CODEC (data ready).
 */
void zeta_wait_irq(void);

/**
 * Wait until device is ready for another command. Use after init command.
 */
void zeta_ready(void);

/**
 * Set mode of the device.
 *
 * @param mode : See datasheet for mode values.
 */
void zeta_select_mode(uint8_t mode);

/**
 * Enters RX mode, selecting channel and packet length.
 *
 * @param ch : Channel.
 * @param pLength : Packet length.
 */
void zeta_rx_mode(uint8_t ch, uint8_t pLength);

/**
 * Set baud-rate of RF (kbps).
 *
 * @param baud : Baud-rate, see datasheet for values.
 */
void zeta_set_baud(uint8_t baud);

/**
 * Set sync bytes and addressing after preamble.
 *
 * @param sync1 : Byte 1.
 * @param sync2 : Byte 2.
 * @param sync3 : Byte 3.
 * @param sync4 : Byte 4.
 */
void zeta_sync_byte(uint8_t sync1, uint8_t sync2, uint8_t sync3, uint8_t sync4);

/**
 * Send packet opening.
 *
 * @param ch : Channel to transmit on.
 * @param pLength : Length of packet.
 */
void zeta_send_packet(uint8_t ch, uint8_t pLength);

/**
 * Write byte to TX buffer (to send).
 *
 * @param data : Data byte to send.
 */
void zeta_write_byte(uint8_t data);

/**
 * Send packet closing.
 */
void zeta_send_close(void);

/**
 * Read one byte from FIFO only.
 *
 * @return Byte from the FIFO buffer.
 */
uint8_t zeta_rx_byte(void);

/**
 * Read packet from FIFO loop until empty.
 *
 * @param[out] packet : Array to return Rx'd packet to.
 * @todo Confirm that this works as expected.
 */
void zeta_rx_packet(uint8_t* packet);

#endif // ZETA_H

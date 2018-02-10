/**
 * @file zeta.h
 * @author Rhys Thomas <rt8g15@soton.ac.uk>
 * @date 2018-02-08
 *
 * @brief Library for interfacing with ZetaPlus radio module.
 *
 * @todo Add detailed description for groups.
 */

#ifndef ZETA_H
#define ZETA_H

#include <stdint.h>
#include <msp430.h>
#include "spi.h"

#define SDN (BIT4) ///< Sleep pin (high, low) = (sleep, active).
#define IRQ (BIT5) ///< Interrupt request.

// f = 869.50e6 + (250e3 * 15) = 873.25MHz
#define CHANNEL (15u) ///< Move nearer the 900MHz band for antenna.
#define PACKET_SIZE (12u) ///< Size of packets in network.

/**
 * @defgroup zeta_setup ZetaPlus Initialisation
 * Detailed description.
 * @{
 */

/**
 * Setup the device.
 *
 * @todo Setup up the comms registers?
 */
void zeta_init(void);

/**
 * Wait for interrupt from CODEC (data ready).
 */
void zeta_wait_irq(void);

/**
 * Wait until device is ready for another command. Use after zeta_init()
 * command.
 */
void zeta_ready(void);

/** @} */

/**
 * @defgroup zeta_config ZetaPlus Configuration
 * Detailed description.
 * @{
 */

/**
 * Set mode of the device [ATM].
 *
 * @param mode : Operating mode of the radio.
 *      | Value | Name  | Description |
 *      |-------|-------|-------------|
 *      | 1     | RX    | ZetaPlus enters RX mode using last RX configured settings.     |
 *      | 2     | Ready | Low power awake state for fast switching between RX/TX (<1ms). |
 *      | 3     | Sleep | Low power sleep mode with register retention.                  |
 */
void zeta_select_mode(uint8_t mode);

/**
 * Receive mode configuration [ATR].
 *
 * @param ch : Channel.
 * @param pLength : Packet length.
 */
void zeta_rx_mode(uint8_t ch, uint8_t pLength);

/**
 * Set sync bytes and addressing after preamble [ATA].
 *
 * @param sync1 : Byte 1.
 * @param sync2 : Byte 2.
 * @param sync3 : Byte 3.
 * @param sync4 : Byte 4.
 */
void zeta_sync_byte(uint8_t sync1, uint8_t sync2, uint8_t sync3, uint8_t sync4);

/**
 * Set baud-rate between radio and MCU [ATH].
 *
 * @param baud : Data rate between ZetaPlus and MCU.
 *      | `baud` | Data Rate |
 *      |--------|-----------|
 *      | 0      | 9.6kbps   |
 *      | 1      | 19.2kbps (default) |
 *      | 2      | 28.8kbps  |
 *      | 3      | 38.4kbps  |
 *      | 4      | 57.6kbps  |
 */
void zeta_set_baud_host(uint8_t baud);

/**
 * Set baud-rate of RF [ATB].
 *
 * @param baud : RF data rate, must be at least double host baud.
 *      | `baud` | RF Rate   |
 *      |--------|-----------|
 *      | 1      | 4.8kbps   |
 *      | 2      | 9.6kbps   |
 *      | 3      | 38.4kbps  |
 *      | 4      | 128.0kbps |
 *      | 5      | 256.0kbps |
 *      | 6      | 500.0kbps |
 * @todo Which is default?
 */
void zeta_set_baud_rf(uint8_t baud);

/**
 * Set RF output power [ATP].
 *
 * @param pwr : RF TX output power (1-127).
 * @note Not a linear function. The output power varies depending on factors
 * such as supply voltage, impedance miss-match etc. Adjustment resoulution of
 * the TX output power is very fine (step size < 0.1dB) when operating near
 * max. power setting, but becomes coarser as output level is reduced.
 */
void zeta_set_rf_power(uint8_t pwr);

/**
 * Enable data error checking [ATE].
 *
 * @param en : Enable CRC error checking.
 *      | | |
 *      |-|-|
 *      | 0    | Disable error checking (default) |
 *      | 1    | Enable error checking            |
 */
void zeta_enable_crc(uint8_t en);

/**
 * Get RSSI from receiver [ATQ].
 *
 * @return Received signal strength (0-255).
 * @todo Check this method is correct. Does nIRQ need to go high?
 */
uint8_t zeta_get_rssi(void);

/**
 * Reset the radio to its power on defaults [ATD].
 */
void zeta_reset_default(void);

/** @} */

/**
 * @defgroup zeta_tx ZetaPlus Transmission
 * Detailed description.
 * @{
 */

/**
 * Transmit mode configuration [ATS].
 *
 * @param ch : Channel to transmit on.
 * @param pLength : Length of packet.
 */
void zeta_send_open(uint8_t ch, uint8_t pLength);

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
 * Send byte packet over radio.
 *
 * @param packet : Pointer to byte packet to send.
 * @param len : Length of packet you're sending.
 */
void zeta_send_packet(uint8_t* packet, uint8_t len);

/** @} */

/**
 * @defgroup zeta_rx ZetaPlus Reception
 * Detailed description.
 * @{
 */

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

/** @} */

#endif // ZETA_H

/**
 * @file util.h
 * @author Rhys Thomas <rt8g15@soton.ac.uk>
 * @date 2018-02-09
 *
 * @brief Setup functions to initialise general peripherals.
 *
 * Pin-out:
 * ```
 * MSP430FR5739
 * -------+
 *        |
 *    P1.0|<- External comparator (INT).
 *    P4.0|<- UB20M power-gated output (INT).
 *        |
 * -------+
 * ```
 */

#ifndef UTIL_H
#define UTIL_H

#include <msp430.h>
#include <stdint.h>

// Config defines.
#define USE_LPM5 ///< When sleeping, go as low as LPM4.5.

#define UB20     (BIT0) ///< UB20 interrupt comes from P4.0.
#define EXT_COMP (BIT0) ///< External comparator inputs to P1.0.

/**
 * @defgroup init Initialisation
 * @brief Peripheral initialisation.
 * @{
 */

/**
 * @brief Initialise IO pins and buses.
 *
 * All ports are output, driven low and interrupt flags are cleared.
 *
 * @see ULP4.1
 */
void io_init(void);

/**
 * @brief Initialise system clock.
 *
 * | Clock | Module | Freq. |
 * |-------|--------|-------|
 * | MCLK  | DCO    | 24MHz |
 * | SMCLK | DCO    | 3MHz  |
 * | ACLK  | DCO    | 10kHz |
 *
 * @bug Clocks are being weird.
 */
void clock_init(void);

/**
 * @brief Setup timer for periodic transmissions.
 */
void timer_init(void);

/**
 *
 */
void rtc_init(void);

/** @} */

/**
 * @defgroup led LEDs
 * @brief Functions for controlling on-board LEDs.
 * @{
 */

/**
 * @brief Write byte to on-board LEDs.
 *
 * @param byte : Value to write.
 * @test See if this works.
 */
void led_set(uint8_t byte);

/**
 * @brief Clear all the on-board LEDs.
 *
 * @test See if this works.
 */
void led_clear(void);

/** @} */

#ifdef USE_LPM5
/**
 * @brief Sets the necessary registers to enter LPMx.5.
 *
 * @param mode : LPM3.5 or LPM4.5?
 * @see Section 1.4 in the 57xx user guide.
 */
inline void enter_lpm5(uint8_t mode);

/**
 *
 */
inline void exit_lpm5(void);
#endif // USE_LPM45

#endif // UTIL_H

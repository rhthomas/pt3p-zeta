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
// #define USE_LPM5 ///< When sleeping, go as low as LPM4.5.
#define USE_SHDN ///< When done, shutdown the processor.

#define PS_LATCH (BIT0) ///< Power-supply latch output on P4.0.
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
 * | ACLK  | VLO    | 10kHz |
 */
void clock_init(void);

/** @} */

/**
 * @defgroup timers Timers
 * @brief Control of timer module.
 * @{
 */

/**
 * @brief Setup timer for periodic transmissions.
 *
 * @ingroup init
 */
void timer_init(void);

/**
 * @brief Start running the timer.
 */
inline void timer_start(void);

/**
 * @brief Halt the timer.
 */
inline void timer_stop(void);

/**
 * @brief Reset the count of the timer.
 *
 * Used in the timeout functionality.
 */
inline void timer_reset(void);

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
 * @see Section 1.4 in the 57xx user guide.
 */
inline void enter_lpm5(void);
#endif // USE_LPM5

#ifdef USE_SHDN
/**
 * @brief Release latch to power-supply.
 *
 * @note The latch is driven high in io_init() and is the first setup function
 * that is called, regardless of what mode of operation is running.
 */
inline void power_off(void);
#endif // USE_SHDN

#endif // UTIL_H

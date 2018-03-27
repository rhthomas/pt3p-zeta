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

// Connection definitions.
#define PS_LATCH (BIT0) ///< Power-supply latch output on P4.0.
#define EXT_COMP (BIT0) ///< External comparator output connects to P1.0.

// State definitions.
#define COMP_ON (P1IN & EXT_COMP) ///< Tests the state of the comparator output.

typedef enum {
    ERROR_OK,
    ERROR_NOBUFS,
    ERROR_TIMEOUT
} error_t;

/**
 * @defgroup init Initialisation
 * @brief Peripheral initialisation.
 * @{
 */

/**
 * @brief Initialise IO pins.
 *
 * Power-supply latch is driven high to keep the supply of power to the node.
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
 * | SMCLK | DCO/8  | 3MHz  |
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
 *
 * @pre timer_init() must be called before starting the timer for it to work.
 */
inline void timer_start(void);

/**
 * @brief Halt the timer.
 *
 * @see timer_reset()
 */
inline void timer_stop(void);

/**
 * @brief Reset the count of the timer.
 *
 * @note Called at the end of timer_stop().
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
 * @param byte : Value to display.
 */
void led_set(uint8_t byte);

/**
 * @brief Clear all the on-board LEDs.
 */
void led_clear(void);

/**
 * @brief Flash the LEDs to indicate a timeout has occured.
 */
void led_flash(void);

/** @} */

/**
 * @brief Sets the necessary registers to enter LPMx.5.
 *
 * @see Section 1.4 in the 57xx user guide.
 */
inline void enter_lpm5(void);

/**
 * @brief Release latch to power-supply.
 *
 * @note The latch is driven high in io_init() and is the first setup function
 * that is called, regardless of what mode of operation is running.
 */
inline void power_off(void);

#endif // UTIL_H

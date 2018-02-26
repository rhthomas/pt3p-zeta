/**
 * @file setup.h
 * @author Rhys Thomas <rt8g15@soton.ac.uk>
 * @date 2018-02-09
 *
 * @brief Setup functions to initialise general peripherals.
 *
 * Pin-out:
 * ```
 * MSP430FR5739
 * –––––––+
 *        |
 *    P1.0|<- External comparator (INT).
 *    P1.1|<- UB20M power-gated output (INT).
 *        |
 * –––––––+
 * ```
 */

#ifndef SETUP_H
#define SETUP_H

#include <msp430.h>
#include <stdint.h>

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

/** @} */

#endif // SETUP_H

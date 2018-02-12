/**
 * @file setup.h
 * @author Rhys Thomas <rt8g15@soton.ac.uk>
 * @date 2018-02-09
 *
 * @brief Setup functions to initialise general peripherals.
 */

#ifndef SETUP_H
#define SETUP_H

#include <msp430.h>

/**
 * @defgroup init Initialisation
 * @brief Peripheral initialisation.
 * @{
 */

/**
 * Initialise IO pins and buses.
 *
 * All ports are output, driven low and interrupt flags are cleared.
 *
 * @see ULP4.1
 */
void io_init(void);

/**
 * Initialise system clock.
 *
 * | Clock | Module | Freq. |
 * |-------|--------|-------|
 * | MCLK  | DCO    | 24MHz |
 * | SMCLK | DCO    | 12MHz |
 * | ACLK  | DCO    | 10kHz |
 */
void clock_init(void);

/**
 * Setup time for periodic transmissions.
 */
void timer_init(void);

/** @} */

#endif // SETUP_H

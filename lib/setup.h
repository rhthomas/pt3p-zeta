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
 * Initialise IO pins and buses.
 *
 * @see ULP4.1
 * @todo Setup P1.4 interrupt (rising edge?).
 */
void io_init(void);

/**
 * Initialise system clock.
 *
 * MCLK  = DCO @ 24MHz, SMCLK = DCO/2 @ 12MHz, ACLK  = VLO @ 10kHz.
 */
void clock_init(void);

#endif // SETUP_H

/**
 * @file led.h
 * @author Rhys Thomas <rt8g15@soton.ac.uk>
 * @date 2018-02-26
 *
 * @brief LED control functions.
 */

#ifndef LED_H
#define LED_H

#include <stdint.h>
#include <msp430.h>

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

#endif // LED_H

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
 *    P1.0|<- External comparator      (INT)
 *    P4.0|<- UB20M power-gated output (INT)
 *        |
 * -------+
 * ```
 */

#ifndef UTIL_H
#define UTIL_H

#include <msp430.h>
#include <stdint.h>

// Connection definitions.
#define PS_LATCH (BIT0) ///< Power-supply latch output on P4.0.
#define EXT_COMP (BIT0) ///< External comparator output connects to P1.0.

// State definitions.
#define COMP_ON (P1IN & EXT_COMP) ///< Tests the state of the comparator output.

#define BUFFER_SIZE 10u ///< Number of bytes in mailbox buffer.

typedef struct {
    uint8_t buffer[BUFFER_SIZE];
    uint8_t head, tail;
} buffer_t;

typedef enum {
    ERROR_OK = 0, ERROR_NOBUFS, ERROR_TIMEOUT
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
 * @brief Control of timer module for timeout protection.
 * @{
 */

/**
 * @brief Start running the timer.
 */
inline void timer_start(void);

/**
 * @brief Stop and reset the timer.
 */
inline void timer_stop(void);

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

/**
 * @defgroup mailbox Mailbox
 * @brief Functions for reading/writing data to the mailbox.
 * @{
 */

/**
 * @brief Push new data into the box.
 *
 * @param[in] new - Data to be added.
 * @return Error status.
 * @retval ERROR_OK - No errors, packet added to buffer.
 * @retval ERROR_NOBUFS - Buffer full, packet lost.
 */
error_t mailbox_push(uint8_t new);

/**
 * @brief Pop oldest data from the box.
 *
 * @param[out] out - Address to write popped data to.
 * @return Error status.
 * @retval ERROR_OK - No errors, packet popped from buffer.
 * @retval ERROR_NOBUFS - Buffer empty, Nothing to pop.
 */
error_t mailbox_pop(uint8_t *out);

/** @} */

#endif // UTIL_H

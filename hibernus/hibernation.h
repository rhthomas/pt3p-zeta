/**
 * @file hibernation.h
 * @author Domenico Balsamo
 * @date 2016-05-25
 *
 * @brief Hibernus for MSP430FR5739.
 *
 * Hibernus: Software-based approach to intelligently hibernate and restore
 * the system's state in response to a power failure. This software exploits
 * an external comparator.
 *
 * Citation: If you are using this code for your research please cite:
 *
 * [1]    D. Balsamo, A. S. Weddell, G. V. Merrett, B. M. Al-Hashimi,
 * D. Brunelli and L. Benini, "Hibernus: Sustaining Computation During
 * Intermittent Supply for Energy-Harvesting Systems," in IEEE Embedded
 * Systems Letters, vol. 7, no. 1, pp. 15-18, March 2015.
 *
 * [2]    D. Balsamo; A. Weddell; A. Das; A. Arreola; D. Brunelli;
 * B. Al-Hashimi; G. Merrett; L. Benini, "Hibernus++: A Self-Calibrating and
 * Adaptive System for Transiently-Powered Embedded Devices," in IEEE
 * Transactions on Computer-Aided Design of Integrated Circuits and Systems,
 * vol.PP, no.99, pp.1-1
 *
 * This research has been supported by the University of Southampton and the
 * University of Bologna.
 *
 * @copyright Copyright 2016, Domenico Balsamo, All rights reserved.
 * @remarks Modifications by Rhys Thomas <rt8g15@soton.ac.uk>.
 * @test See if hibernus works with LPM4.5.
 */

#ifndef HIBERNATION_H
#define HIBERNATION_H

#include <msp430.h>
#include "util.h"

// Hibernus configuration.
//#define USE_SWITCHES ///< Use switches to change trigger point of comparator.
//#define HIB_DEBUG ///< Debug hibernus with specific I/O outputs.

// Interrupt and restoring.
#define INT   (0xEDD8) ///< Address storing the interrupt flag.
#define CHECK (0xEDDC) ///< Address storing the check flag.

// FRAM.
#define FRAM_START (0xEDF0) ///< Address of the start of FRAM.
#define FRAM_END   (0xFF40) ///< Address of the end of FRAM.

// RAM.
#define RAM_START (0x1C00) ///< Address of the start of RAM.
#define RAM_END   (0x2000) ///< Address of the end of RAM.

// PC.
#define PC (0xEDF0) ///< Address storing the program counter.

// Threshold.
#ifdef USE_SWITCHES
#define VMIN  (2270) ///< Restore threshold.
#define VMINN (2170) ///< Hibernate threshold.
#endif // USE_SWITCHES

/**
 * @defgroup hib Hibernus
 * @brief Hibernus functions by D. Balsamo.
 * @{
 */

/**
 * @brief Called when the system powers up and decides whether to restore the
 * state or not.
 */
void Hibernus(void);

/**
 * @brief Prepare the processor for hibernation (and potential power loss).
 *
 * 1. Sets `CC_Check` to 0.
 * 2. Moves R1, R2, ..., R15 into FRAM.
 * 3. Copies the state of the SP register and writes it to `FRAM_pc`.
 * 4. Copies all RAM to FRAM.
 * 5. Copies all general registers to FRAM.
 * 6. Sets `CC_Check` to 1.
 */
void Hibernate(void);

/**
 * @brief Restores the state of the processor once the power supply is
 * sufficient.
 *
 * 1.
 *
 * @todo Document this.
 */
void Restore(void);

/**
 * @brief Copies RAM into FRAM.
 */
void Save_RAM(void);

/**
 * @brief Copies general purpose registers into FRAM.
 */
void Save_Register(void);

/**
 * @brief Restores general purpose registers from FRAM.
 */
void Restore_Register(void);

/**
 * @brief Set the trigger voltage of the external comparator.
 *
 * @param v : Voltage of the new trigger point (mV).
 */
void Set_V(unsigned int v);

/**
 * @brief Set up P1.0 interrupt on the falling edge.
 */
inline void int_falling(void);

/**
 * @brief Set up P1.0 interrupt on the rising edge.
 */
inline void int_rising(void);

/**
 * @brief ISR for external comparator trigger.
 *
 * 1.
 *
 * @todo Document this.
 */
__interrupt void PORT1_ISR(void);

/** @} */

#endif // HIBERNATION_H

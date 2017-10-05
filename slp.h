/******************************************************************************
* Copyright (C) 2017 by Ben Heberlein
*
* Redistribution, modification or use of this software in source or binary
* forms is permitted as long as the files maintain this copyright. This file
* was created for the University of Colorado Boulder course Internet of Things
* Embedded Firmware. Ben Heberlein and the University of Colorado are not
* liable for any misuse of this material.
*
*******************************************************************************/
/**
 * @file slp.h
 * @brief Definitions and global variables for sleep system
 *
 * This file defines sleep functions and variables for the Managing Energy
 * Modes demonstration. See associated header file for function descriptions.
 * This file is heavily based on the Silicon Labs sleep.h file. Rights are
 * reserved for Silicon Labs.
 *
 * @author Ben Heberlein
 * @date September 11 2017
 * @version 1.0
 *
 */

#ifndef __SLP_H__
#define __SLP_H__

#include "main.h"

/**
 * @brief Number of energy states
 */
#define SLP_NUM_EM 5

/*
 * @brief Enumeration of energy mode names
 */
typedef enum slp_em_e {
	EM0=0,
	EM1=1,
	EM2=2,
	EM3=3,
	EM4=4
} slp_em_t;

/**
 * @brief Sleep function
 *
 * When called, this function makes sure the system settles into the lowest
 * energy state possible based on the current values of the shared variable
 * sleep_state.
 *
 * @return Void
 */
void slp_sleep(void);

/**
 * @brief Block sleep mode function
 *
 * When called, this function increments a global shared variable that makes
 * sure the system cannot sleep beyond the given energy mode until the mode
 * is completely unblocked.
 *
 * @param minimum The lowest energy mode that needs to be blocked
 *
 * @return Void
 */
void slp_blockSleepMode(slp_em_t minimum);

/**
 * @brief Unblock sleep mode function
 *
 * When called, this function decrements a global shared variable that makes
 * sure the system cannot sleep beyond the given energy mode until the mode
 * is completely unblocked.
 *
 * @param minimum The energy mode to be unblocked
 *
 * @return Void
 */
void slp_unblockSleepMode(slp_em_t minimum);

/**
 * @brief Initializes sleep system
 *
 * This function initializes the sleep system for the demo.
 *
 * @return Void
 */
void slp_init(void);

#endif /* __SLP_H__ */

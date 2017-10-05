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
 * @file letimer.h
 * @brief The interface for low energy timer functions
 *
 * This file contains interfaces for low energy timer functions for the
 * Managing Energy Modes demonstration.
 *
 * @author Ben Heberlein
 * @date September 11 2017
 * @version 2.0
 *
 */

#ifndef __LETIMER_H__
#define __LETIMER_H__

#include "main.h"

/*
 * @brief Lowest energy state the LETIMER should run at
 */
#define LETIMER_EM 3

/*
 * @brief Period of LETIMER0 in seconds
 */
#define LETIMER_PERIOD 1.75

/*
 * @brief On time of LED in seconds
 */
#define LETIMER_ONTIME 0.02

/*
 * @brief Frequencies for LFA clocks in Hz
 */
#define LETIMER_LFXO_FREQ 32768
#define LETIMER_ULFRCO_FREQ 1000

/*
 * @brief Maximum count for LETIMER
 */
#define LETIMER_MAX 65535

/*
 * @brief Number of commands available including none command
 */
#define LETIMER_NUMCMD 4

/*
 * @briefs Commands from ADC interrupt
 */
typedef enum letimer_cmd_e {
	LETIMER_CMD_NONE,
	LETIMER_CMD_INC,
	LETIMER_CMD_DEC,
	LETIMER_CMD_RST,
} letimer_cmd_t;

/* Command message queue */
extern uint32_t letimer_cmd_state[];

/**
 * @brief Initializes LETIMER0
 *
 * This function initializes LETIMER0 for the demo.
 *
 * @return Void
 */
void letimer_init(void);

#endif /* __LETIMER_H__ */

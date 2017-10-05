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
 * @file slp.c
 * @brief Functions to manage energy modes
 *
 * This file implements sleep functions to manage energy modes for the Managing
 * Energy Modes demonstration. This file is heavily based on the Silicon Labs
 * sleep.h file. Rights are reserved for Silicon Labs.
 *
 * @author Ben Heberlein
 * @date September 11 2017
 * @version 1.0
 *
 */

#include "slp.h"
#include "em_emu.h"

/* Global shared sleep state */
static uint32_t slp_state[SLP_NUM_EM] = {0};

void slp_sleep(void) {
	/* Lowest state is EM0 */
	if (slp_state[EM0] > 0) {
		return;

	/* Lowest state is EM1 */
	} else if (slp_state[EM1] > 0) {
		EMU_EnterEM1();

	/* Lowest state is EM2 */
	} else if (slp_state[EM2] > 0) {
		EMU_EnterEM2(true);

	/* Lowest state is EM3 */
	} else if (slp_state[EM3] > 0) {
		EMU_EnterEM3(true);

	/* Lowest state is EM4 */
	} else{
		/* Temporarily set as EM3 because EM4 is hard to debug */
		EMU_EnterEM3(true);
	}
	return;
}

void slp_blockSleepMode(slp_em_t minimum) {

	/* Atomically change sleep state */
	CORE_ATOMIC_IRQ_DISABLE();
	slp_state[minimum]++;
	CORE_ATOMIC_IRQ_ENABLE();

	return;
}

void slp_unblockSleepMode(slp_em_t minimum) {

	/* Atomically unblock sleep mode */
	CORE_ATOMIC_IRQ_DISABLE();
	if(slp_state[minimum] > 0) {
		slp_state[minimum]--;
	}
	CORE_ATOMIC_IRQ_ENABLE();

	return;
}

void slp_init(void) {

	return;
}

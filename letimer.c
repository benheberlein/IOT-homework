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
 * @file letimer.c
 * @brief The implementation for low energy timer functions
 *
 * This file implements low energy timer functions for the Managing Energy
 * Modes demonstration. See associated header file for function descriptions.
 *
 * @author Ben Heberlein
 * @date September 11 2017
 * @version 2.0
 *
 */

#include "letimer.h"
#include "em_letimer.h"
#include "slp.h"
#include "gpio.h"
#include "bma280.h"

/* Current on time in ms */
static int32_t letimer_ontime = 0;

/* Command message queue */
uint32_t letimer_cmd_state[LETIMER_NUMCMD] = {0,0,0,0};

void LETIMER0_IRQHandler(void) {

	uint32_t int_flag = 0;

	/* Check interrupt flag and clear */
	if (LETIMER_IntGet(LETIMER0) & LETIMER_IF_COMP1 ||
		LETIMER_IntGet(LETIMER0) & LETIMER_IF_UF) {
		if (LETIMER_IntGet(LETIMER0) & LETIMER_IF_UF) {
			int_flag = LETIMER_IF_UF;
		} else if (LETIMER_IntGet(LETIMER0) & LETIMER_IF_COMP1) {
			int_flag = LETIMER_IF_COMP1;
		}

		LETIMER_IntClear(LETIMER0, LETIMER_IEN_UF | LETIMER_IEN_COMP1);

		/* Only do if underflow */
		if (int_flag == LETIMER_IF_UF) {
		/* Go through state buffer and look for commands */
		for (int i = 0; i < LETIMER_NUMCMD; i++) {
			if (letimer_cmd_state[i] > 0) {

				/* Execute command the right number of times */
				for (int j = 0; j < letimer_cmd_state[i]; j++) {
					switch (i) {
					case LETIMER_CMD_NONE:
						/* Shouldn't happen under normal operation */
						break;
					case LETIMER_CMD_INC:
						letimer_ontime += 500;
						if (letimer_ontime > LETIMER_PERIOD * 1000) {
							letimer_ontime = LETIMER_PERIOD * 1000;
						}
						break;
					case LETIMER_CMD_DEC:
						//gpio_setLED0(false);
						//gpio_setLED1(true);
						letimer_ontime -= 500;
						if (letimer_ontime < 0) {
							letimer_ontime = 0;
						}
						break;
					case LETIMER_CMD_RST:
						//gpio_setLED0(false);
						//gpio_setLED1(false);
						letimer_ontime = 20;
						gpio_setLED1(false);
						bma280_disable();
						break;
					default:
						break;
					}

				}

				/* Calculate and set compare values for duty cycle with new ontime as
				 * defined by macros in associated header file */
				uint32_t comp0_val = 0;
				uint32_t comp1_val = 0;

				/* Calculate number of ticks */
				if (LETIMER_EM == 3) {
					comp0_val = LETIMER_ULFRCO_FREQ * LETIMER_PERIOD;
					comp1_val = LETIMER_ULFRCO_FREQ * letimer_ontime / 1000;
				} else {
					comp0_val = LETIMER_LFXO_FREQ * LETIMER_PERIOD;
					comp1_val = LETIMER_LFXO_FREQ * letimer_ontime / 1000;
				}

				/* Adjust ticks if greater than 16 bits */
				uint8_t div = 0;
				while (comp0_val > LETIMER_MAX) {
					comp0_val /= 2;
					comp1_val /=2;
					div++;
				}

				/* Stop timer */
				LETIMER0->CMD = LETIMER_CMD_STOP;

				/* Wait until the timer is not busy (clock crossing) */
				while(LETIMER0->SYNCBUSY) {}

				/* Clear low four bits and rewrite with value of div */
				CMU->LFAPRESC0 &= ~0xf;
				CMU->LFAPRESC0 |= div;

				/* Set new compare */
				LETIMER_CompareSet(LETIMER0, 0, comp0_val);
				LETIMER_CompareSet(LETIMER0, 1, comp1_val);

				/* Clear timer */
				LETIMER0->CMD = LETIMER_CMD_CLEAR;
				while(LETIMER0->SYNCBUSY) {}

				/* Start again */
				LETIMER0->CMD = LETIMER_CMD_START;
				while(LETIMER0->SYNCBUSY) {}
			}

			/* set command state buffer for that command to 0 */
			letimer_cmd_state[i] = 0;
		}
		}

		/* Strict toggle on match mode */
		if (int_flag == LETIMER_IF_UF) {
			gpio_setLED0(false);
		} else if (int_flag == LETIMER_IF_COMP1) {
			gpio_setLED0(true);
		}
	}

	return;
}

void letimer_init(void){
	/* Initialize LETIMER0 for COMP0 controlling period via underflow and
	 * COMP1 controlling duty cycle*/
	LETIMER_Init_TypeDef letimerInit = {
		.bufTop = false,
		.comp0Top = true,
		.debugRun = true,
		.enable = false,
		.out0Pol = 0,
		.out1Pol = 0,
		.repMode = letimerRepeatFree,
		.ufoa0 = letimerUFOANone,
		.ufoa1 = letimerUFOANone,
	};

	/* Get default on time in ms */
	letimer_ontime = LETIMER_ONTIME * 1000;

	/* Calculate and set compare values for correct period and duty cycle as
	 * defined by macros in associated header file */
	uint32_t comp0_val = 0;
	uint32_t comp1_val = 0;

	/* Calculate number of ticks */
	if (LETIMER_EM == 3) {
		comp0_val = LETIMER_ULFRCO_FREQ * LETIMER_PERIOD;
		comp1_val = LETIMER_ULFRCO_FREQ * letimer_ontime / 1000;
	} else {
		comp0_val = LETIMER_LFXO_FREQ * LETIMER_PERIOD;
		comp1_val = LETIMER_LFXO_FREQ * letimer_ontime / 1000;
	}

	/* Adjust ticks if greater than 16 bits */
	uint8_t div = 0;
	while (comp0_val > LETIMER_MAX) {
		comp0_val /= 2;
		comp1_val /=2;
		div++;
	}

	/* Clear low four bits and rewrite with value of div */
	CMU->LFAPRESC0 &= ~0xf;
	CMU->LFAPRESC0 |= div;

	LETIMER_CompareSet(LETIMER0, 0, comp0_val);
	LETIMER_CompareSet(LETIMER0, 1, comp1_val);

	/* Initialize LETIMER0 */
	LETIMER_Init(LETIMER0, &letimerInit);

	/* Wait until the timer is not busy (clock crossing) */
	while(LETIMER0->SYNCBUSY) {}

	/* Clear any interrupts */
	LETIMER0->IFC |= LETIMER_IFC_COMP0 |
		LETIMER_IFC_COMP1 |
		LETIMER_IFC_REP0 |
		LETIMER_IFC_REP1 |
		LETIMER_IFC_UF;

	/* Enable UF and COMP1 interrupts */
	LETIMER0->IEN |= LETIMER_IEN_UF | LETIMER_IEN_COMP1;

	/* Block on correct sleep level using sleep library */
	slp_blockSleepMode(LETIMER_EM);

	/* Enable interrupts in NVIC */
	NVIC_EnableIRQ(LETIMER0_IRQn);

	/* Enable LETIMER0 */
	LETIMER_Enable(LETIMER0, true);

	return;
}


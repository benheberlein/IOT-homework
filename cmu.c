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
 * @file gpio.c
 * @brief The implementation for CMU functions
 *
 * This file implements clock management unit functions for the Managing Energy
 * Modes demonstration. See associated header file for function descriptions.
 *
 * @author Ben Heberlein
 * @date September 11 2017
 * @version 1.0
 *
 */

#include "cmu.h"
#include "letimer.h"

void cmu_init(void){
	/* Set up HFRCO and disable HFXO */
	CMU_HFXOAutostartEnable(0,false,false);
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);
	CMU_HFRCOBandSet(cmuHFRCOFreq_19M0Hz);
	CMU_OscillatorEnable(cmuOsc_HFXO, false, false);

	if (LETIMER_EM == 3) {
		/* Set up ULFRCO */
		CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);
		CMU_OscillatorEnable(cmuOsc_ULFRCO, true, true);
	} else {
		/* Set up LFXO for LFA */
		CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
	}

	/* Set up LFXO and disable LFRCO */
	CMU_OscillatorEnable(cmuOsc_LFXO, true, true);
	CMU_OscillatorEnable(cmuOsc_LFRCO, false, false);

	/* Set up ADC clock */
	CMU_AUXHFRCOBandSet(cmuAUXHFRCOFreq_1M0Hz);
	CMU_OscillatorEnable(cmuOsc_AUXHFRCO, true, true);
	CMU->ADCCTRL = CMU_ADCCTRL_ADC0CLKSEL_AUXHFRCO;

	/* Set up HFPERCO */
	CMU_ClockEnable(cmuClock_HFPER, true);

	/* Enable peripheral clocks */
	CMU_ClockEnable(cmuClock_GPIO, true);
	CMU_ClockEnable(cmuClock_CORELE, true);
	CMU_ClockEnable(cmuClock_LETIMER0, true);
	CMU_ClockEnable(cmuClock_ADC0, true);
	CMU_ClockEnable(cmuClock_TIMER0, true);
	CMU_ClockEnable(cmuClock_USART1, true);

}


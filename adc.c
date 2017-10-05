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
 * @file adc.c
 * @brief The implementation for ADC functions
 *
 * This file implements ADC functions for the Low Energy ADC assignment.
 * See the associated header file for function descriptions.
 *
 * @author Ben Heberlein
 * @date September 19 2017
 * @version 2.0
 *
 */

#include "adc.h"
#include "gpio.h"
#include "letimer.h"
#include "slp.h"
#include "bma280.h"

static uint32_t adc_joystick = 0;
static bool adc_debounce_flag = false;
static int joystick_temp = 0;

void ADC0_IRQHandler(void) {

	if (ADC_IntGet(ADC0) & ADC_IF_SINGLECMP) {
		ADC_IntClear(ADC0, ADC_IF_SINGLECMP | ADC_IF_SINGLE | ADC_IF_SINGLEUF | ADC_IF_SCANUF | ADC_IF_SINGLECMP);

		/* Wait until the system sleeps again so there are no repeat commands */
		if (adc_debounce_flag == false) {

			/* Get command from joystick (read enough to clear FIFO */
			for (int i = 0; i < 10; i++) {
				adc_joystick = ADC_DataSingleGet(ADC0);
			}

			if (adc_joystick > ADC_JOY_UP_GT && adc_joystick < ADC_JOY_UP_LT) {
				/* Want this to happen immediately */
				//gpio_setLED1(true);
				bma280_enable();
			} else if (adc_joystick > ADC_JOY_RIGHT_GT && adc_joystick < ADC_JOY_RIGHT_LT) {
				letimer_cmd_state[LETIMER_CMD_INC]++;
			} else if (adc_joystick > ADC_JOY_LEFT_GT && adc_joystick < ADC_JOY_LEFT_LT) {
				letimer_cmd_state[LETIMER_CMD_DEC]++;
			} else if (adc_joystick > ADC_JOY_DOWN_GT && adc_joystick < ADC_JOY_DOWN_LT) {
				/* Want this to happen immediately */
				//gpio_setLED1(false);
				bma280_disable();
			} else if (adc_joystick > ADC_JOY_PRESS_GT && adc_joystick < ADC_JOY_PRESS_LT) {
				letimer_cmd_state[LETIMER_CMD_RST]++;
			} else {
				/* Shouldn't happen under normal operation */
				letimer_cmd_state[LETIMER_CMD_NONE]++;
			}

			/* Wait until the system sleeps again to act.
			 * This effectively just captures the rising edge or first change */
			adc_debounce_flag = true;
		}

	}

	joystick_temp++;
	return;
}

void adc_debounce(void) {
	/* Set a flag when going to sleep so we know the joystick action is valid again */
	adc_debounce_flag = false;
	return;
}

void adc_init(void) {

	/* Structures */
	ADC_Init_TypeDef adc_init = {
		.em2ClockConfig = adcEm2ClockAlwaysOn,
		.ovsRateSel = adcOvsRateSel2,
		.prescale = 111, // Calculated for 200Hz
		.tailgate = false,
		.timebase = _ADC_CTRL_TIMEBASE_DEFAULT,
		.warmUpMode = adcWarmupNormal,
	};

	ADC_InitSingle_TypeDef single_init = {
		.acqTime = adcAcqTime32,
		.diff = false,
		.fifoOverwrite = true,
		.leftAdjust = false,
		.negSel = adcNegSelVSS,
		.posSel = adcPosSelAPORT3XCH8,
		.prsEnable = false,
		.prsSel = adcPRSSELCh0,
		.reference = adcRefVDD,
		.rep = true,
		.resolution = adcRes12Bit,
		.singleDmaEm2Wu = false,
	};

	/* Initialize operation */
	ADC_Init(ADC0, &adc_init);
	ADC_InitSingle(ADC0, &single_init);

	/* Set compare */
	ADC0->SINGLECTRL |= ADC_SINGLECTRL_CMPEN;
	ADC0->CMPTHR = _ADC_CMPTHR_RESETVALUE;
	ADC0->CMPTHR = (ADC_GT << _ADC_CMPTHR_ADGT_SHIFT) |
				   (ADC_LT << _ADC_CMPTHR_ADLT_SHIFT);

	/* Clear any interrupts */
	ADC0->IFC |= ADC_IFC_PROGERR |
			ADC_IFC_VREFOV |
			ADC_IFC_SCANCMP |
			ADC_IFC_SINGLECMP |
			ADC_IFC_SCANUF |
			ADC_IFC_SINGLEUF |
			ADC_IFC_SCANOF |
			ADC_IFC_SINGLEOF;
	ADC_IntClear(ADC0, ADC_IF_SINGLECMP | ADC_IF_SINGLE | ADC_IF_SINGLEUF | ADC_IF_SCANUF | ADC_IF_SINGLECMP);

	/* Set interrupt */
	ADC0->IEN |= ADC_IEN_SINGLECMP;

	/* Block on correct sleep level using sleep library */
	slp_blockSleepMode(ADC_EM);

	/* Enable CPU interrupt */
	NVIC_EnableIRQ(ADC0_IRQn);
	NVIC_SetPriority(TIMER0_IRQn, 1);

	/* Start operation */
	ADC0->CMD |= ADC_CMD_SINGLESTART;

	return;

}

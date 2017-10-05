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
 * @file adc.h
 * @brief Definitions and interfaces for ADC functions
 *
 * This file declares ADC functions for the Low Energy ADC assignment.
 *
 * @author Ben Heberlein
 * @date September 19 2017
 * @version 2.0
 *
 */

#ifndef __ADC_H__
#define __ADC_H__

#include "main.h"
#include "em_adc.h"

/* EM level */
#define ADC_EM 3

/* Thresholds for interrupt */
#define ADC_GT 0
#define ADC_LT 3800

/* Thresholds for joystick found experimentally, tunable */
#define ADC_JOY_UP_GT    3300
#define ADC_JOY_UP_LT    3800
#define ADC_JOY_DOWN_GT  1700
#define ADC_JOY_DOWN_LT  2200
#define ADC_JOY_RIGHT_GT 2800
#define ADC_JOY_RIGHT_LT 3300
#define ADC_JOY_LEFT_GT  2200
#define ADC_JOY_LEFT_LT  2800
#define ADC_JOY_NONE_GT  3800
#define ADC_JOY_NONE_LT  4096
#define ADC_JOY_PRESS_GT 0
#define ADC_JOY_PRESS_LT 200

/**
 * @brief Debounce helper function
 *
 * This function sets a debounce flag to let the system know when to sleep.
 *
 * @return Void
 */
void adc_debounce(void);

/**
 * @brief Initializes ADC
 *
 * This function initializes the ADC to sense joystick input.
 *
 * @return Void
 */
void adc_init(void);

#endif /* __ADC_H__ */

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
 * @file gpio.h
 * @brief Definitions and interfaces for GPIO functions
 *
 * This file declares GPIO functions and macros for the Managing Energy Modes
 * demonstration. See the associated header file for function descriptions.
 *
 * @author Ben Heberlein
 * @date September 11 2017
 * @version 2.0
 *
 */

#ifndef __GPIO_H__
#define __GPIO_H__

#include "main.h"
#include "em_gpio.h"
#include "stdint.h"

/*
 * @brief General macro definitions
 */
#define GPIO_ON 1
#define GPIO_OFF 0

/*
 * @brief Macro definitions for LED0
 */
#define	GPIO_LED0_PORT		gpioPortF
#define GPIO_LED0_PIN		4
#define GPIO_LED0_DEFAULT	false

/*
 * @brief Macro definitions for LED1
 */
#define	GPIO_LED1_PORT		gpioPortF
#define GPIO_LED1_PIN		5
#define GPIO_LED1_DEFAULT	false

/*
 * @brief Macro definition for ADC
 */
#define GPIO_ADC_PORT		gpioPortA
#define GPIO_ADC_PIN		0

/**
 * @brief Toggle LED0
 *
 * This function toggles the LED connected to port F pin 4 (LED0)
 *
 * @return Void
 */
void gpio_toggleLED0(void);

/**
 * @brief Toggle LED1
 *
 * This function toggles the LED connected to port F pin 5 (LED1)
 *
 * @return Void
 */
void gpio_toggleLED1(void);

/**
 * @brief Set LED0
 *
 * This function sets the LED connected to port F pin 4 (LED0) low or high
 *
 * @return Void
 */
void gpio_setLED0(bool on);

/**
 * @brief Set LED1
 *
 * This function sets the LED connected to port F pin 4 (LED0) low or high
 *
 * @return Void
 */
void gpio_setLED1(bool on);

/**
 * @brief Initializes GPIO
 *
 * This function initializes the GPIO pins to be able to access the LEDs.
 *
 * @return Void
 */
void gpio_init(void);

#endif /* __GPIO_H__ */

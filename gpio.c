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
 * @brief The implementation for GPIO functions
 *
 * This file implements GPIO functions for the Managing Energy Modes
 * demonstration. See the associated header file for function descriptions.
 *
 * @author Ben Heberlein
 * @date September 11 2017
 * @version 3.0
 *
 */

#include "gpio.h"
#include "bma280.h"

/* LED0 state */
static bool gpio_led0_state = false;

/* LED1 state */
static bool gpio_led1_state = false;


void gpio_toggleLED0(void) {

	if (gpio_led0_state == false) {
		gpio_led0_state = true;
		GPIO_PinOutSet(GPIO_LED0_PORT, GPIO_LED0_PIN);
	} else {
		gpio_led0_state = false;
		GPIO_PinOutClear(GPIO_LED0_PORT, GPIO_LED0_PIN);
	}

	return;

}

void gpio_toggleLED1(void) {

	if (gpio_led1_state == false) {
		gpio_led1_state = true;
		GPIO_PinOutSet(GPIO_LED1_PORT, GPIO_LED1_PIN);
	} else {
		gpio_led1_state = false;
		GPIO_PinOutClear(GPIO_LED1_PORT, GPIO_LED1_PIN);
	}

	return;

}

void gpio_setLED0(bool on) {
		gpio_led0_state = on;
		if (on) {
			GPIO_PinOutSet(GPIO_LED0_PORT, GPIO_LED0_PIN);
		} else {
			GPIO_PinOutClear(GPIO_LED0_PORT, GPIO_LED0_PIN);
		}
}

void gpio_setLED1(bool on) {
		gpio_led1_state = on;
		if (on) {
			GPIO_PinOutSet(GPIO_LED1_PORT, GPIO_LED1_PIN);
		} else {
			GPIO_PinOutClear(GPIO_LED1_PORT, GPIO_LED1_PIN);
		}
}

void gpio_init(void){

	/* Set LED ports to be standard output drive with default off (cleared) */
	GPIO_DriveStrengthSet(GPIO_LED0_PORT, gpioDriveStrengthStrongAlternateStrong);
	GPIO_PinModeSet(GPIO_LED0_PORT, GPIO_LED0_PIN, gpioModePushPull, GPIO_LED0_DEFAULT);
	GPIO_DriveStrengthSet(GPIO_LED1_PORT, gpioDriveStrengthStrongAlternateStrong);
	GPIO_PinModeSet(GPIO_LED1_PORT, GPIO_LED1_PIN, gpioModePushPull, GPIO_LED1_DEFAULT);

	/* Set SPI pins */
	GPIO_PinModeSet(BMA280_SPI_PORT, BMA280_SPI_CS, gpioModePushPull, 1);
	GPIO_PinModeSet(BMA280_SPI_PORT, BMA280_SPI_SCK, gpioModePushPull, 1);
	GPIO_PinModeSet(BMA280_SPI_PORT, BMA280_SPI_MISO, gpioModeInput, 0);
	GPIO_PinModeSet(BMA280_SPI_PORT, BMA280_SPI_MOSI, gpioModePushPull, 1);

	/* Set joystick pin as analog input */
	GPIO_PinModeSet(GPIO_ADC_PORT, GPIO_ADC_PIN, gpioModeDisabled, 0);

	return;

}

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
 * @file bma280.h
 * @brief Definitions and interfaces for the BMA280 accelerometer module
 *
 * This file declares BMA280 functions and macros for the SPI Tap Sensor
 * demonstration. See the associated header file for function descriptions.
 *
 * @author Ben Heberlein
 * @date September 24 2017
 * @version 4.0
 *
 */

#ifndef __BMA280_H__
#define __BMA280_H__

#include "main.h"
#include "em_timer.h"
#include "em_usart.h"
#include "em_gpio.h"
#include "stdint.h"

/* SPI Pins (all on port C) */
#define BMA280_SPI_CS 9
#define BMA280_SPI_SCK 8
#define BMA280_SPI_MISO 7
#define BMA280_SPI_MOSI 6
#define BMA280_SPI_PORT gpioPortC

/* Interrupt pin */
#define BMA280_INT_PIN 11
#define BMA280_INT_PORT gpioPortD

/* Timer delay in ns */
#define BMA280_TMR_DELAY 2000000

/* Timer clock in MHz */
#define BMA280_TMR_CLK 19

/* Timer period in ns */
#define BMA280_TMR_PERIOD (1000/BMA280_TMR_CLK)

/* Timer clock ticks to count */
#define BMA280_TMR_TICKS BMA280_TMR_DELAY/(BMA280_TMR_PERIOD)

/* Timer sleep mode */
#define BMA280_TMR_EM 1
#define BMA280_USART_EM 1

/* Selected BMA280 register access macros */
#define BMA280_PMU_LPW 0x11
#define BMA280_PMU_LPW_SUSPEND_MASK (1<<7)
#define BMA280_PMU_LPW_DEEP_SUSPEND_MASK (1<<5)
#define BMA280_PMU_LPW_SUSPEND (0<<7)
#define BMA280_PMU_LPW_DEEP_SUSPEND (1<<5)

#define BMA280_BGW_SOFTRESET 0x14
#define BMA280_BGW_SOFTRESET_SOFTRESET_MASK 0xff
#define BMA280_BGW_SOFTRESET_SOFTRESET 0xb6

#define BMA280_PMU_RANGE 0x0f
#define BMA280_PMU_RANGE_MASK 0b1111
#define BMA280_PMU_RANGE_RANGE 0b1010

#define BMA280_PMU_BW 0x10
#define BMA280_PMU_BW_BW_MASK 0b11111
#define BMA280_PMU_BW_BW 0b01100

#define BMA280_INT_6 0x28
#define BMA280_INT_6_SLOPE_TH_MASK 0xff
#define BMA280_INT_6_SLOPE_TH 0x02

#define BMA280_INT_8 0x2a
#define BMA280_INT_8_TAP_QUIET_MASK (1<<7)
#define BMA280_INT_8_TAP_SHOCK_MASK (1<<6)
#define BMA280_INT_8_TAP_DUR_MASK 0b111
#define BMA280_INT_8_TAP_QUIET (0<<7)
#define BMA280_INT_8_TAP_SHOCK (0<<6)
#define BMA280_INT_8_TAP_DUR 0b011

#define BMA280_INT_9 0x2b
#define BMA280_INT_9_TAP_SAMP_MASK (0b11<<6)
#define BMA280_INT_9_TAP_TH_MASK 0b11111
#define BMA280_INT_9_TAP_SAMP (0b01<<6)
#define BMA280_INT_9_TAP_TH 0b00001

#define BMA280_INT_EN_0 0x16
#define BMA280_INT_EN_0_S_TAP_EN_MASK (1<<5)
#define BMA280_INT_EN_0_D_TAP_EN_MASK (1<<4)
#define BMA280_INT_EN_0_S_TAP_EN (1<<5)
#define BMA280_INT_EN_0_D_TAP_EN (1<<4)

#define BMA280_INT_STATUS_0 0x09
#define BMA280_INT_STATUS_0_S_TAP_INT_MASK (1<<5)
#define BMA280_INT_STATUS_0_D_TAP_INT_MASK (1<<4)
#define BMA280_INT_STATUS_0_S_TAP_INT (1<<5)
#define BMA280_INT_STATUS_0_D_TAP_INT (1<<4)

#define BMA280_INT_RST_LATCH 0x21
#define BMA280_INT_RST_LATCH_LATCH_INT_MASK 0b1111
#define BMA280_INT_RST_LATCH_RESET_INT_MASK (1<<7)
#define BMA280_INT_RST_LATCH_LATCH_INT 0b0011
#define BMA280_INT_RST_LATCH_RESET_INT (1<<7)

#define BMA280_INT_MAP_0 0x19
#define BMA280_INT_MAP_0_INT1_S_TAP_MASK (1<<5)
#define BMA280_INT_MAP_0_INT1_D_TAP_MASK (1<<4)
#define BMA280_INT_MAP_0_INT1_S_TAP (1<<5)
#define BMA280_INT_MAP_0_INT1_D_TAP (1<<4)

#define BMA280_INT_MAP_2 0x21
#define BMA280_INT_MAP_2_INT1_S_TAP_MASK (1<<5)
#define BMA280_INT_MAP_2_INT1_D_TAP_MASK (1<<4)
#define BMA280_INT_MAP_2_INT1_S_TAP (1<<5)
#define BMA280_INT_MAP_2_INT1_D_TAP (1<<4)

#define BMA280_RW_MASK (1<<7)
#define BMA280_READ (1<<7)
#define BMA280_WRITE (0<<7)

/**
 * @brief Initializes the USART module for SPI communication
 *
 * This function initializes the USART peripheral for SPI mode in order to
 * communicate with the BMA280.
 *
 * @return Void
 */
void bma280_usart_init(void);

/**
 * @brief Starts the delay timer
 *
 * This function enables the delay timer on TIMER0. When done, bma280_trm_flag
 * will go high.
 *
 * @return Void
 */
void bma280_tmr_delay(uint32_t ms);

/**
 * @brief Initializes the timer for BMA280 delay functions
 *
 * This function initializes TIMER0 for delay functionality.
 *
 * @param ms Time to delay in milliseconds
 * @return Void
 */
void bma280_tmr_init(void);

/**
 * @brief Handle the tap event
 *
 * This function looks at the tap flag from the GPIO interrupt and
 * toggles LED1 according to a single tap or a double tap.
 *
 * @return Void
 */
void bma280_tap_handle(void);

/**
 * @brief Configure the BMA280
 *
 * This function configures the registers in the BMA280 for single and double
 * tap interrupt modes.
 *
 * @return Void
 */
void bma280_configure(void);

/**
 * @brief Read data from BMA280
 *
 * This function reads data from BMA280
 *
 * @param address The address to read from
 *
 * @return The data read
 */
uint8_t bma280_read(uint8_t address);

/**
 * @brief Write data to BMA280
 *
 * This function writes data to BMA280
 *
 * @param address The address to write to
 * @param data The data to write
 *
 * @return Void
 */
void bma280_write(uint8_t address, uint8_t data);

/**
 * @brief Disables BMA280
 *
 * This function disables the BMA280 accelerometer, putting it to sleep.
 *
 * @return Void
 */
void bma280_disable(void);

/**
 * @brief Enables BMA280
 *
 * This function enables the BMA280 accelerometer, bringing it out of sleep.
 *
 * @return Void
 */
void bma280_enable(void);

/**
 * @brief Initializes BMA280
 *
 * This function initializes the BMA280 accelerometer module.
 *
 * @return Void
 */
void bma280_init(void);

#endif /* __BMA280_H__ */

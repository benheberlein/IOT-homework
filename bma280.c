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
 * @file bma280.c
 * @brief Functions for the BMA280 external accelerometer
 *
 * This file implements BMA280 control and energy mode functions for the SPI
 * Tap Sensor demonstration. See the associated header file for function
 * descriptions.
 *
 * @author Ben Heberlein
 * @date September 24 2017
 * @version 3.0
 *
 */

#include "bma280.h"
#include "slp.h"
#include "em_timer.h"
#include "gpio.h"

/* Timer interrupt flag */
volatile bool bma280_tmr_flag = false;

static bool bma280_check_tap = false;

/* Swap endian helper function for 2 byte transfers*/
static uint16_t _bma280_swap(uint16_t d) {
	return (d >> 8) | (d << 8);
}

/* GPIO interrupt for BMA280 */
void GPIO_ODD_IRQHandler(void) {

	if (GPIO_IntGet() & _GPIO_IF_EXT_MASK) {
		/* Clear interrupts */
		GPIO_IntClear(_GPIO_IF_EXT_MASK);

		/* Set flag to check if it is a single tap or double tap (latched for 500ms) */
		bma280_check_tap = true;

	}
}

/* TIMER0 interrupt handler */
void TIMER0_IRQHandler(void) {
	CORE_ATOMIC_IRQ_DISABLE();

	TIMER_IntClear(TIMER0, TIMER_IF_OF);

	/* Reset timer */
	TIMER_Enable(TIMER0, false);
	TIMER_CounterSet(TIMER0, 0);

	bma280_tmr_flag = true;

	CORE_ATOMIC_IRQ_ENABLE();
}

void bma280_usart_init() {
	USART_InitSync_TypeDef usart_init = {
		.autoCsEnable = true,
		.autoCsHold = 0,
		.autoCsSetup = 0,
		.autoTx = false,
		.baudrate = 100000,
		.clockMode = usartClockMode3,
		.databits = usartDatabits8,
		.enable = usartDisable,
		.master = true,
		.msbf = true,
		.prsRxCh = usartPrsRxCh0,
		.prsRxEnable = false,
		.refFreq = 0,
	};

	slp_blockSleepMode(BMA280_USART_EM);

	USART_InitSync(USART1, &usart_init);

	/* Route pins and enable*/
	USART1->ROUTELOC0 = USART_ROUTELOC0_CSLOC_LOC11 | USART_ROUTELOC0_CLKLOC_LOC11 | USART_ROUTELOC0_TXLOC_LOC11 | USART_ROUTELOC0_RXLOC_LOC11;
	USART1->ROUTEPEN = USART_ROUTEPEN_CSPEN | USART_ROUTEPEN_CLKPEN | USART_ROUTEPEN_TXPEN | USART_ROUTEPEN_RXPEN;

	USART_Enable(USART1, usartEnable);

}

void bma280_tmr_delay(uint32_t ms) {
	bma280_tmr_flag = false;

	/* Set TIMER TOP for overflow */
	TIMER_TopSet(TIMER0, ms*1000000/1024/BMA280_TMR_PERIOD);

	/* Enable */
	TIMER_Enable(TIMER0, true);

	/* Block on correct sleep level using sleep library */
	slp_blockSleepMode(BMA280_TMR_EM);

	while(bma280_tmr_flag == false) {
		/* Snooze */
		slp_sleep();
	}

	/* Unblock sleep mode */
	slp_unblockSleepMode(BMA280_TMR_EM);
}

void bma280_tmr_init() {
	/* Initialize TIMER0 */
	TIMER_Init_TypeDef timerInit = {
	  .enable     = false,
	  .debugRun   = true,
	  .prescale   = timerPrescale1024,
	  .clkSel     = timerClkSelHFPerClk,
	  .fallAction = timerInputActionNone,
	  .riseAction = timerInputActionNone,
	  .mode       = timerModeUp,
	  .dmaClrAct  = false,
	  .quadModeX4 = false,
	  .oneShot    = false,
	  .sync       = false,
	};

	/* Set TIMER TOP for overflow */
	TIMER_TopSet(TIMER0, BMA280_TMR_TICKS/1024);

	/* Overflow interrupt */
	TIMER_IntEnable(TIMER0, TIMER_IF_OF);

	/* NVIC enable and priority (nesting) */
	NVIC_EnableIRQ(TIMER0_IRQn);
	NVIC_SetPriority(TIMER0_IRQn, 0);

	/* Initialize TIMER0, don't enable yet */
	TIMER_Init(TIMER0, &timerInit);

	return;
}

void bma280_tap_handle() {
	if (bma280_check_tap == true) {

		/* Get status */
		uint8_t rd1 = bma280_read(BMA280_INT_STATUS_0);

		/* Wait for second tap max time */
		bma280_tmr_delay(280);
		uint8_t rd2 = bma280_read(BMA280_INT_STATUS_0);

		/* Reset interrupts (if using latch mode, safer just to always do it) */
		bma280_write(BMA280_INT_RST_LATCH, BMA280_INT_RST_LATCH_RESET_INT |
					BMA280_INT_RST_LATCH_LATCH_INT);

		/* Reset flag */
		bma280_check_tap = false;

		/* Toggle LED */
		if (rd1 == 32 && rd2 == 16) {
			gpio_setLED1(true);
		} else if (rd1 == 32 && rd2 == 0) {
			gpio_setLED1(false);
		}

	}
}

uint8_t bma280_read(uint8_t address) {

	uint16_t tx = 0;
	uint16_t rx = 0;
	tx = ((uint16_t) (BMA280_READ | address) << 8) | 0x00;
	tx = _bma280_swap(tx);

	USART_TxDouble(USART1, tx);
	while(!(USART1->STATUS & USART_STATUS_TXC)) {}

	rx = USART1->RXDOUBLE;
	rx = rx >> 8;

	return (uint8_t) rx;

}

void bma280_write(uint8_t address, uint8_t data) {

	uint16_t tx = 0;
	tx = ((uint16_t) (BMA280_WRITE | address)) << 8 | data;

	/* Swap for double Tx buffer order */
	tx = _bma280_swap(tx);

	USART_TxDouble(USART1, tx);
	while(!(USART1->STATUS & USART_STATUS_TXC)) {}

	/* Clear any Rx with dummy variable */
	while ((USART1->STATUS & USART_STATUS_RXDATAV)) {
		tx = USART1->RXDOUBLE;
	}

	return;
}

void bma280_config() {
	/* Range 4g */
	bma280_write(BMA280_PMU_RANGE, BMA280_PMU_RANGE_RANGE);

	/* Bandwidth 128Hz */
	bma280_write(BMA280_PMU_BW, BMA280_PMU_BW_BW);

	/* Tap quiet 30ms, tap shock 50ms, tap duration 200ms */
	bma280_write(BMA280_INT_8, BMA280_INT_8_TAP_DUR   |
							   BMA280_INT_8_TAP_SHOCK |
							   BMA280_INT_8_TAP_QUIET );

	/* Tap samples and threshold */
	bma280_write(BMA280_INT_9, BMA280_INT_9_TAP_TH   |
							   BMA280_INT_9_TAP_SAMP );

	/* Map interrupts to INT2 */
	bma280_write(BMA280_INT_MAP_0, BMA280_INT_MAP_0_INT1_S_TAP | BMA280_INT_MAP_0_INT1_D_TAP);

	/* Enable interrupts and 500ms latch */
	bma280_write(BMA280_INT_EN_0, BMA280_INT_EN_0_S_TAP_EN |
								  BMA280_INT_EN_0_D_TAP_EN );
	bma280_write(BMA280_INT_RST_LATCH, BMA280_INT_RST_LATCH_LATCH_INT);

}

void bma280_disable() {
	/* Put the device to sleep */
	uint8_t rd = bma280_read(BMA280_PMU_LPW);
	bma280_write(BMA280_PMU_LPW, BMA280_PMU_LPW_DEEP_SUSPEND);
}

void bma280_enable() {
	/* Do some stuff */
	bma280_write(BMA280_BGW_SOFTRESET, BMA280_BGW_SOFTRESET_SOFTRESET);

	/* Delay timer */
	bma280_tmr_delay(2);

	/* Clear any pending interrupts */
	uint16_t rd = bma280_read(BMA280_INT_RST_LATCH);
	bma280_write(BMA280_INT_RST_LATCH, BMA280_INT_RST_LATCH_RESET_INT |
			BMA280_INT_RST_LATCH_LATCH_INT);
	rd = bma280_read(BMA280_INT_RST_LATCH);

	/* Set interrupt pin to BMA280 */
	GPIO_PinModeSet(BMA280_INT_PORT, BMA280_INT_PIN, gpioModeInput, 0);

	/* Clear GPIO external interrupt pins */
	GPIO_IntClear(_GPIO_IF_EXT_MASK);

	/* Enable GPIO_ODD interrupt vector in NVIC */
	NVIC_EnableIRQ(GPIO_ODD_IRQn);

	/* Enable BMA280 GPIO interrupt pin for rising edge*/
	GPIO_IntConfig(BMA280_INT_PORT, BMA280_INT_PIN, true, false, true);

	/* Configure settings */
	bma280_config();
}

void bma280_init() {

	/* Initialize timer */
	bma280_tmr_init();

	/* Initialize USART */
	bma280_usart_init();

	/* Initialization to BMA280 */
	bma280_enable();

	/* Turn off BMA280 */
	bma280_disable();

	return;

}

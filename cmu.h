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
 * @file cmu.h
 * @brief The interface for CMU functions
 *
 * This file defines clock management unit functions for the Managing Energy
 * Modes demonstration. See associated header file for function descriptions.
 *
 * @author Ben Heberlein
 * @date September 11 2017
 * @version 1.0
 *
 */

#ifndef __CMU_H__
#define __CMU_H__

#include "main.h"
#include "em_cmu.h"

/**
 * @brief Initializes CMU
 *
 * This function initializes the CMU for the demo.
 *
 * @return Void
 */
void cmu_init(void);

#endif /* __CMU_H__ */

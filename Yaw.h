// *******************************************************
//
// Yaw.h
//
//
//
// Authored with by tfo49 & dta82
//
// Created 23/04/2024
//
// *******************************************************
#ifndef YAW_H_
#define YAW_H_

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "utils/ustdlib.h"
#include "stdlib.h"
#include "inc/hw_ints.h"  // Interrupts


//*****************************************************************************
// Constants
//*****************************************************************************
#define YAW_PERIPH SYSCTL_PERIPH_GPIOB
#define YAW_PORT GPIO_PORTB_BASE
#define CHA_PIN GPIO_PIN_0
#define CHB_PIN GPIO_PIN_1

#define YAW_REF_PERIPH SYSCTL_PERIPH_GPIOC
#define YAW_REF_PORT GPIO_PORTC_BASE
#define YAW_REF_PIN GPIO_PIN_4

#define LOWER_BIT_MASK 0x0F

#define NUM_PHASES 4
#define NUM_ENCODER_SLOTS 112
#define DEGREE_PER_SLOTS_X100 36000 / 448
#define SCALE_FACTOR 100

/*
 * upper 4 bits are used for previous state, lower four bits are used for current state
 * to change state, shift left 4 times, then just add phase value (00 = 0, 01 = 1, 11 = 2, 10 = 3)
 */

volatile bool isYawCalibrated;

void initYaw();
int16_t getYawDegrees();
uint8_t getYawDecimal();
void enableYawRefInt();
void disableYawRefInt();

#endif /* YAW_H_ */

// *******************************************************
//
// circBufT.c
//
// Manages interface for Display.c.
//
// Authored with by tfo49 & dta82
//
// Created 23/04/2024
//
// *******************************************************


#ifndef DISPLAY_H_
#define DISPLAY_H_

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
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "stdlib.h"
#include "inc/hw_ints.h"  // Interrupts

void initDisplay();
void clearDisplay();
void displayYaw(int16_t yawDegrees, uint8_t yawRemainder);
void displayAlt(int16_t altPercent);
void displayMainPWM(uint32_t pwmDutyCycle);
void displayTailPWM(uint32_t pwmDutyCycle);

#endif /* DISPLAY_H_ */

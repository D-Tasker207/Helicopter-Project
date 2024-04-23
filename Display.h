/*
 * Display.h
 *
 *  Created on: 23/04/2024
 *      Author: tfo49 & dta82
 */



#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "utils/ustdlib.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "stdlib.h"
#include "buttons4.h"
#include "circBufT.h"
#include "inc/hw_ints.h"  // Interrupts

void initDisplay();
void clearDisplay();
void displayValue(uint16_t ADCvalue, uint16_t minADCVal, displayMode_t mode);


#endif /* DISPLAY_H_ */

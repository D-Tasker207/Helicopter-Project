/*
 * Altitude.h
 *
 *  Created on: 23/04/2024
 *      Author: tfo49
 */

#ifndef ALTITUDE_H_
#define ALTITUDE_H_

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
#include "stdlib.h"
#include "circBufT.h"
#include "inc/hw_ints.h"  // Interrupts

#define BUF_SIZE 25
#define SAMPLE_RATE_HZ 125
#define ADC_1V_RANGE 1241

static circBuf_t g_inBuffer;        // Buffer of size BUF_SIZE integers (sample values)

void initADC();
void ADCIntHandler();
int32_t calculateMeanAltVal();
int16_t getAltPercent(int32_t currentADCVal, int32_t zeroPercentVal);
void setLandedVal();

#endif /* ALTITUDE_H_ */

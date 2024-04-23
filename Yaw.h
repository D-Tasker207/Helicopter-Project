/*
 * Yaw.h
 *
 *  Created on: 23/04/2024
 *      Author: tfo49
 */

#ifndef YAW_H_
#define YAW_H_

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

#define NIBBLE_SIZE 4

#define NUM_ENCODER_TEETH 112
#define NUM_PHASES 4
#define DEG_PER_PHASE_X100 81
#define SCALE_FACTOR 100

static bool chAState = 0;
static bool chBState = 0;

/*
 * upper 4 bits are used for previous state, lower four bits are used for current state
 * to change state, shift left 4 times, then just add phase value (00 = 0, 01 = 1, 11 = 2, 10 = 3)
 */
static uint8_t state;

static int16_t numPhaseChanges;

void initYaw();
void YawInterruptHandler();
void calculateYaw();
int16_t getYawDegrees();





#endif /* YAW_H_ */

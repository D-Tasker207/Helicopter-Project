/*
 * PID.h
 *
 *  Created on: 9/05/2024
 *      Author: tfo49
 */

#ifndef PID_H_
#define PID_H_

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
#include "inc/hw_ints.h"

#define CONTROL_SCALE_FACTOR 100

#define KP_MAIN 500
#define KI_MAIN 20
#define KD_MAIN 150

#define KP_TAIL 300
#define KI_TAIL 20
#define KD_TAIL 10

#define DELTA_T 10

#define CNTRL_MAX 98
#define CNTRL_MIN 2

int32_t controllerUpdateMain (int32_t setpoint, int32_t sensorReading);
int32_t controllerUpdateTail (int32_t setpoint, int32_t sensorReading);

#endif /* PID_H_ */

// *******************************************************
//
// PID.h
//
// Handles interface with PID module.
// Stores constants including gains for PID calculations.
//
// Authored with by tfo49 & dta82
//
// Created 23/04/2024
//
// *******************************************************

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


//*****************************************************************************
// Constants
//*****************************************************************************

#define CONTROL_SCALE_FACTOR 100

#define KP_MAIN 500
#define KI_MAIN 50
#define KD_MAIN 0
#define GRAVITY_OFFSET 33

#define KP_TAIL 100
#define KI_TAIL 20
#define KD_TAIL 0

#define DELTA_T 10

#define CNTRL_MAX 98
#define CNTRL_MIN 2

//*****************************************************************************
// Public variables
//*****************************************************************************

extern uint32_t mainControlEffort;
extern uint32_t tailControlEffort;

extern int32_t intErrMain;
extern int32_t intErrTail;

//*****************************************************************************
// Public functions
//*****************************************************************************

int32_t controllerUpdateMain (int32_t setpoint, int32_t sensorReading);
int32_t controllerUpdateTail (int32_t setpoint, int32_t sensorReading);

#endif /* PID_H_ */

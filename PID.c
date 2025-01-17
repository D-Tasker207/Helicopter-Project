// *******************************************************
//
// PID.c
//
// Dictates control effort using a PI control scheme.
// Takes data from altitude ADC and yaw Quadrature encoder to
// calculate motor duty cycles for main and tail rotors.
//
// Authored with by tfo49 & dta82
//
// Created 23/04/2024
//
// *******************************************************

#include "PID.h"

//*****************************************************************************
// Constants
//*****************************************************************************

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

//
uint32_t mainControlEffort = 0;
uint32_t tailControlEffort = 0;
int32_t intErrMain = 0;
int32_t intErrTail = 0;
int32_t sensorPrevMain = 0;
int32_t sensorPrevTail = 0;



int32_t controllerUpdateMain (int32_t setpoint, int32_t sensorReading){
    int32_t error = setpoint - sensorReading;
    int32_t P = KP_MAIN * error;
    int32_t dI = KI_MAIN * error / DELTA_T;
    int32_t D = KD_MAIN * (sensorPrevMain - sensorReading) * DELTA_T;

    int32_t controlEffort = (P + (intErrMain + dI) + D) / CONTROL_SCALE_FACTOR;
    sensorPrevMain = sensorReading;

    // add gravity offset for baseline motor duty cycle
    controlEffort += GRAVITY_OFFSET;

    if (controlEffort > CNTRL_MAX) controlEffort = CNTRL_MAX;
    else if (controlEffort < CNTRL_MIN) controlEffort = CNTRL_MIN;
    else intErrMain += dI;

    return controlEffort;
}

int32_t controllerUpdateTail(int32_t setpoint, int32_t sensorReading){
    int32_t error = setpoint - sensorReading;

    // wrap error value so it always takes the shortest difference in yaw
    error = (error < -180) ? error + 360 : error;
    error = (error > 180) ? error - 360 : error;

    int32_t P = KP_TAIL * error;
    int32_t dI = KI_TAIL * error / DELTA_T;
    int32_t D = KD_TAIL * (sensorPrevTail - sensorReading) * DELTA_T;

    int32_t controlEffort = (P + (intErrTail + dI) + D) / CONTROL_SCALE_FACTOR;
    sensorPrevMain = sensorReading;

    controlEffort += (8 * mainControlEffort) / 10;

    if (controlEffort > CNTRL_MAX) controlEffort = CNTRL_MAX;
    else if (controlEffort < CNTRL_MIN) controlEffort = CNTRL_MIN;
    else intErrTail += dI;

    return controlEffort;
}






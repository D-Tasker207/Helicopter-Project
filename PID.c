/*
 * PID.c
 *
 *  Created on: 9/05/2024
 *      Author: tfo49
 */

#include "PID.h"

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

int32_t controllerUpdateMain (int32_t setpoint, int32_t sensorReading){
    int32_t error = setpoint - sensorReading;
    int32_t P = KP_MAIN * error;
    int32_t dI = KI_MAIN * error / DELTA_T;
    int32_t D = KD_MAIN * (sensorPrevMain - sensorReading) * DELTA_T;

    int32_t controlEffort = (P + (intErrMain + dI) + D) / CONTROL_SCALE_FACTOR;
    sensorPrevMain = sensorReading;

//    controlEffort = max(min(CNTRL_MAX, controlEffort), CNTRL_MIN);
    if (controlEffort > CNTRL_MAX) controlEffort = CNTRL_MAX;
    else if (controlEffort < CNTRL_MIN) controlEffort = CNTRL_MIN;
    else intErrMain += dI;

    return controlEffort;
}

int32_t controllerUpdateTail(int32_t setpoint, int32_t sensorReading){
    int32_t error = setpoint - sensorReading;

    error = (error <= -180) ? error + 360 : error;
    error = (error >= 180) ? error - 360 : error;

    int32_t P = KP_TAIL * error;
    int32_t dI = KI_TAIL * error / DELTA_T;
    int32_t D = KD_TAIL * (sensorPrevTail - sensorReading) * DELTA_T;

    int32_t controlEffort = (P + (intErrTail + dI) + D) / CONTROL_SCALE_FACTOR;
    intErrTail += dI;
    sensorPrevMain = sensorReading;

//    controlEffort = max(min(CNTRL_MAX, controlEffort), CNTRL_MIN);
    //    controlEffort = max(min(CNTRL_MAX, controlEffort), CNTRL_MIN);
    if (controlEffort > CNTRL_MAX) controlEffort = CNTRL_MAX;
    else if (controlEffort < CNTRL_MIN) controlEffort = CNTRL_MIN;
    else intErrMain += dI;

    return controlEffort;
}






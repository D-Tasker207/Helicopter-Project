/*
 * PID.c
 *
 *  Created on: 9/05/2024
 *      Author: tfo49
 */

#include "PID.h"

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

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

    controlEffort += GRAVITY_OFFSET;

//    controlEffort = max(min(CNTRL_MAX, controlEffort), CNTRL_MIN);
    if (controlEffort > CNTRL_MAX) controlEffort = CNTRL_MAX;
    else if (controlEffort < CNTRL_MIN) controlEffort = CNTRL_MIN;
    else intErrMain += dI;

    return controlEffort;
}

int32_t controllerUpdateTail(int32_t setpoint, int32_t sensorReading){
    int32_t error = setpoint - sensorReading;

    error = (error < -180) ? error + 360 : error;
    error = (error > 180) ? error - 360 : error;

    int32_t P = KP_TAIL * error;
    int32_t dI = KI_TAIL * error / DELTA_T;
    int32_t D = KD_TAIL * (sensorPrevTail - sensorReading) * DELTA_T;

    int32_t controlEffort = (P + (intErrTail + dI) + D) / CONTROL_SCALE_FACTOR;
    sensorPrevMain = sensorReading;

    controlEffort += (8 * mainControlEffort) / 10;

//    controlEffort = max(min(CNTRL_MAX, controlEffort), CNTRL_MIN);
    if (controlEffort > CNTRL_MAX) controlEffort = CNTRL_MAX;
    else if (controlEffort < CNTRL_MIN) controlEffort = CNTRL_MIN;
    else intErrTail += dI;

//    if(sensorReading < setpoint + 1 && sensorReading > setpoint - 1) intErrTail = 0;

    return controlEffort;
}






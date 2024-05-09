/*
 * PID.c
 *
 *  Created on: 9/05/2024
 *      Author: tfo49
 */

#include "PID.h"

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

int32_t intErrMain = 0;
int32_t intErrTail = 0;
int32_t sensorPrevMain = 0;
int32_t sensorPrevTail = 0;

/*
 * blah blah blah
 */
int32_t controllerUpdate(int32_t setpoint, int32_t sensorReading, int32_t kp, int32_t ki, int32_t kd, int32_t *sensorPrev, int32_t *intErr) {
    int32_t error = setpoint - sensorReading;
    int32_t P = kp * error;
    int32_t dI = ki * error * DELTA_T;
    int32_t D = kd * (*sensorPrev - sensorReading) / DELTA_T;

    int32_t controlEffort = (P + (intErrMain + dI) + D) / CONTROL_SCALE_FACTOR;
    *intErr += dI;
    *sensorPrev = sensorReading;

    controlEffort = max(min(98, controlEffort), 2);

    return controlEffort;
}


int32_t controllerUpdateMain (int32_t setpoint, int32_t sensorReading){
    return controllerUpdate(setpoint, sensorReading, KP_MAIN, KI_MAIN, KD_MAIN, &sensorPrevMain, &intErrMain);
}

int32_t controllerUpdateTail(int32_t setpoint, int32_t sensorReading){
    return controllerUpdate (setpoint, sensorReading, KP_TAIL, KI_TAIL, KD_TAIL, &sensorPrevTail, &intErrTail);
}






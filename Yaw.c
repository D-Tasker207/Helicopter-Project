/*
 * Yaw.c
 *
 *  Created on: 23/04/2024
 *      Author: tfo49
 */

#include "Yaw.h"

void YawInterruptHandler();
void calculateState(bool chAState, bool chBState);
void calculateNumChanges();

void initYaw(){
    SysCtlPeripheralEnable(YAW_PERIPH);

    GPIOPadConfigSet(YAW_PORT, CHA_PIN | CHB_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    GPIOIntRegister(YAW_PORT, YawInterruptHandler);
    GPIOIntTypeSet(YAW_PORT, CHA_PIN | CHB_PIN, GPIO_BOTH_EDGES);
    GPIOIntEnable(YAW_PORT, CHA_PIN | CHB_PIN);

    bool chAState = GPIOPinRead(YAW_PORT, CHA_PIN);
    bool chBState = GPIOPinRead(YAW_PORT, CHB_PIN);

    calculateState(chAState, chBState);
    state = state << 4;

    IntEnable(INT_GPIOB);
}

void YawInterruptHandler(){
    // read GPIO pins into chAState & chBState
    bool chAState = GPIOPinRead(YAW_PORT, CHA_PIN);
    bool chBState = GPIOPinRead(YAW_PORT, CHB_PIN);

    //shift old state to upper four bits, setting the lower four bits as 0
    state = state << 4;

    calculateState(chAState, chBState);

    //    calculateNumChanges();
    if ((state & LOWER_BIT_MASK) == (((state >> 4) + 1) % NUM_PHASES)) //encoder is turning clockwise
        numPhaseChanges++;
    else if ((state & LOWER_BIT_MASK) == (((state >> 4) - 1) % NUM_PHASES))// encoder is turning anti-clockwise
        numPhaseChanges--;

    numPhaseChanges %= (NUM_ENCODER_SLOTS * NUM_PHASES);

    GPIOIntClear(YAW_PORT, CHA_PIN | CHB_PIN);
}

void calculateState(bool chAState, bool chBState){
    if(chAState){
        if(chBState)
           state += 2;
        else
           state += 3;
    }
    else{
        if(chBState)
            state += 1;
        else
            state += 0;
    }
}
//
//void calculateNumChanges(){
//    //  And operation masks previous state, right shift operation
//    if ((state & LOWER_BIT_MASK) == (((state >> 4) + 1) % NUM_PHASES)) //encoder is turning clockwise
//        numPhaseChanges++;
//    else if ((state & LOWER_BIT_MASK) == (((state >> 4) - 1) % NUM_PHASES))// encoder is turning anti-clockwise
//        numPhaseChanges--;
//
//    numPhaseChanges %= (NUM_ENCODER_SLOTS * NUM_PHASES);
//}

int16_t getYawDegrees(){
    int16_t angle = ((DEGREE_PER_SLOTS_X100 * numPhaseChanges) / SCALE_FACTOR) % 360;

    //Subtract or add a full rotation from the angle if it exceeds the bounds of -180<x<180
    if(angle <= -180) { angle += 360; }
    else if (angle >= 180) {angle -= 360; }

    return angle;
}

uint8_t getYawDecimal(){
    return (uint8_t) ((DEGREE_PER_SLOTS_X100 * numPhaseChanges) % SCALE_FACTOR);
}

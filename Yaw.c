// *******************************************************
//
// Yaw.c
//
// Initialises Yaw module and calculates helicopter Yaw using the quadrature encoder data.
// Handles Yaw data input interrupt.s
//
// Authored with by tfo49 & dta82
//
// Created 23/04/2024
//
// *******************************************************

#include "Yaw.h"

void YawInterruptHandler();
void YawReferenceInterruptHandler();
void calculateState(bool chAState, bool chBState);
void calculateNumChanges();


//*****************************************************************************
// Constants
//*****************************************************************************
/*
 * upper 4 bits are used for previous state, lower four bits are used for current state
 * to change state, shift left 4 times, then just add phase value (00 = 0, 01 = 1, 11 = 2, 10 = 3)
 */
static uint8_t state = 0;

static int16_t numPhaseChanges;

void initYaw(){
    // Enable the two quad encoding pins
    SysCtlPeripheralEnable(YAW_PERIPH);

    GPIOPadConfigSet(YAW_PORT, CHA_PIN | CHB_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    GPIOIntRegister(YAW_PORT, YawInterruptHandler);
    GPIOIntTypeSet(YAW_PORT, CHA_PIN | CHB_PIN, GPIO_BOTH_EDGES);
    GPIOIntEnable(YAW_PORT, CHA_PIN | CHB_PIN);

    // Enable Yaw Reference pin and set up interrupt
    SysCtlPeripheralEnable(YAW_REF_PERIPH);

    GPIOPadConfigSet(YAW_REF_PORT, YAW_REF_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    GPIOIntRegister(YAW_REF_PORT, YawReferenceInterruptHandler);
    GPIOIntTypeSet(YAW_REF_PORT, YAW_REF_PIN, GPIO_RISING_EDGE);
    GPIOIntEnable(YAW_REF_PORT, YAW_REF_PIN);
    IntEnable(INT_GPIOC);

    // Read initial state of quad pins to prepare for recording yaw
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

    if ((state & LOWER_BIT_MASK) == (((state >> 4) + 1) % NUM_PHASES)) //encoder is turning clockwise
        numPhaseChanges++;
    else if ((state & LOWER_BIT_MASK) == (((state >> 4) - 1) % NUM_PHASES))// encoder is turning anti-clockwise
        numPhaseChanges--;

    numPhaseChanges %= (NUM_ENCODER_SLOTS * NUM_PHASES);

    GPIOIntClear(YAW_PORT, CHA_PIN | CHB_PIN);
}


void YawReferenceInterruptHandler(){
    // Function is triggered when yaw reference slot is passed over,
    // indicating the helicopter is at 0 yaw so it resets the number of
    // phase changes from the 0 point back to 0
    numPhaseChanges = 0;
    isYawCalibrated = true;
    GPIOIntClear(YAW_REF_PORT, YAW_REF_PIN);
}

void calculateState(bool chAState, bool chBState){
    // adds the current state to the state variable based on the encoding of chAState and chBState
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

int16_t getYawDegrees(){
    //converts the current number of state changes to integer number of degrees (-180 < x < 180)
    int16_t angle = ((DEGREE_PER_SLOTS_X100 * numPhaseChanges) / SCALE_FACTOR) % 360;

    //Subtract or add a full rotation from the angle if it exceeds the bounds of -180<x<180
    if(angle <= -180) { angle += 360; }
    else if (angle >= 180) { angle -= 360; }

    return angle;
}

uint8_t getYawDecimal(){
    //returns the number of decimal degrees with two sf of precision
    return (uint8_t) ((DEGREE_PER_SLOTS_X100 * numPhaseChanges) % SCALE_FACTOR);
}

// functions used in testing to enable or disable the yaw reference interrupt handler
void enableYawRefInt(){
    GPIOIntEnable(YAW_REF_PORT, YAW_REF_PIN);
}

void disableYawRefInt(){
    GPIOIntDisable(YAW_REF_PORT, YAW_REF_PIN);
}

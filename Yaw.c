/*
 * Yaw.c
 *
 *  Created on: 23/04/2024
 *      Author: tfo49
 */


void initYaw(){

}

void YawInterruptHandler(){
    // read GPIO pins into chAState & chBState

    state = state << NIBBLE_SIZE;

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

    calculateYaw();
}

void calculateYaw(){
    //  And operation masks previous state, right shift operation
    if ((state && NIBBLE_SIZE) == ((state >> NIBBLE_SIZE) + 1)) //encoder is turning clockwise
        numPhaseChanges++;
    else // encoder is turning anti-clockwise
        numPhaseChanges--;

    numPhaseChanges %= (NUM_ENCODER_TEETH * NUM_PHASES);
}

int16_t getYawDegrees(){
    return (DEG_PER_PHASE_X100 * numPhaseChanges - 180) / 100;
}

uint8_t getYawDecimal(){
    return (uint8_t) ((DEG_PER_PHASE_X100 * numPhaseChanges - 180) % 100;
}

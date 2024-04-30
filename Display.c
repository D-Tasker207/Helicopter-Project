/*
 * Display.h
 *
 *  Created on: 23/04/2024
 *      Author: tfo49 & dta82
 */

#include "Display.h"


void initDisplay() {
    // intialise the Orbit OLED display
    OLEDInitialise();
}

void clearDisplay(){
    // Function to clear the display by writing empty spaces to each line
    uint8_t i;
    for(i = 0; i < 4; i++){
        OLEDStringDraw ("                ", 0, i);
    }
}

void displayAlt(int16_t altPercent){
    char string[17];

    OLEDStringDraw ("Altitude:", 0, 0);
    usnprintf (string, sizeof(string),  "%3d %%", altPercent);
    OLEDStringDraw (string, 0, 1);
}

void displayYaw(int16_t yawDegrees, uint8_t yawRemainder){
    char string[17];

    OLEDStringDraw ("Yaw:", 0, 2);
    usnprintf(string, sizeof(string),  "%4d.%02d", yawDegrees, yawRemainder);
    OLEDStringDraw (string, 0, 3);
}





// *******************************************************
//
// Display.c
//
// Displays characters on the Orbit OLED display.
// Contains functions for displaying each parameter.
//
// Authored with by tfo49 & dta82
//
// Created 23/04/2024
//
// *******************************************************

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

    usnprintf(string, sizeof(string),  "Alt: %3d %%   ", altPercent);
    OLEDStringDraw(string, 0, 0);
}

void displayYaw(int16_t yawDegrees, uint8_t yawRemainder){
    char string[17];

    usnprintf(string, sizeof(string),  "Yaw: %4d.%02d Deg  ", yawDegrees, yawRemainder);
    OLEDStringDraw(string, 0, 1);
}

void displayMainPWM(uint32_t pwmDutyCycle){
    char string[17];

    usnprintf(string, sizeof(string),  "Main Duty%%: %02d %%   ", pwmDutyCycle);
    OLEDStringDraw(string, 0, 2);
}

void displayTailPWM(uint32_t pwmDutyCycle){
    char string[17];

    usnprintf(string, sizeof(string),  "Tail Duty%%: %02d %%    ", pwmDutyCycle);
    OLEDStringDraw(string, 0, 3);
}





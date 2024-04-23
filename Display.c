/*
 * Display.h
 *
 *  Created on: 23/04/2024
 *      Author: tfo49 & dta82
 */

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

void displayValue(uint16_t ADCvalue, uint16_t minADCVal, displayMode_t mode){
    // ADCValue is the current mean value in the adc buffer
    // minADCVal is the "landed" value of the ADC
    // mode is the current display mode set by the displayMode_t enum
    int16_t percent;
    char string[17];
    switch(mode){
    case PERCENT:
        // Calculate the percentage value
        percent = (minADCVal - ADCvalue) * 100 / ADC_1V_RANGE;
        // Display the percentage value on the OLED display
        OLEDStringDraw ("Percent Value:", 0, 0);
        usnprintf (string, sizeof(string),  "%4d %%", percent);
        OLEDStringDraw (string, 0, 1);
        break;
    case ADC_VALUE:
        // Display the ADC value on the OLED display
        OLEDStringDraw ("Mean ADC Value:", 0, 0);
        usnprintf (string, sizeof(string), "%4d", ADCvalue);
        OLEDStringDraw (string, 0, 1);
        break;
    case OFF:
        // Clear the OLED display
        clearDisplay();
    }
}





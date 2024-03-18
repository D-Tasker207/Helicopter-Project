/*
 * milestone1.c
 *
 *  Created on: 18/03/2024
 *      Author: tfo49 & dta82
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/interrupt.h"
#include "utils/ustdlib.h"
#include "stdio.h"
#include "stdlib.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "buttons4.h"
#include "circBufT.h"

//*****************************************************************************
// Constants
//*****************************************************************************
#define BUF_SIZE 25
#define SAMPLE_RATE_HZ 100

#define SYSTICK_RATE_HZ 100
#define SLOWTICK_RATE_HZ 4
#define ADC_1V_RANGE 1241

//*****************************************************************************
// Type Definitions / State Machine
//*****************************************************************************
typedef enum {PERCENT=0,
              ADC_VALUE,
              OFF,
              ENUM_SIZE
} displayMode_t;

//*****************************************************************************
// Global variables
//*****************************************************************************
static circBuf_t g_inBuffer;        // Buffer of size BUF_SIZE integers (sample values)
//static uint32_t zeroHeightVal = 0;
volatile uint8_t slowTick = false;

//********************************************************
// Function Prototypes
//********************************************************
void SysTickIntHandler();
void ADCIntHandler();
void initClock();
void initDisplay();
void initADC();
void displayPercent(uint8_t percent);
void displayADCValue(uint16_t value);
void clearDisplay();


//********************************************************
// Function Definitions
//********************************************************

void SysTickIntHandler() {
//    static uint8_t tickCount = 0;
//    const uint8_t ticksPerSlow = SYSTICK_RATE_HZ / SLOWTICK_RATE_HZ

//    updateButtons ();       // Poll the buttons
//    if (++tickCount >= ticksPerSlow){
//        tickCount = 0; // Signal a slow tick
//        slowTick = true;
//    }

    // Initiate a conversion
    ADCProcessorTrigger(ADC0_BASE, 3);
}

void ADCIntHandler() {
    uint32_t ulValue;

    //
    // Get the single sample from ADC0.  ADC_BASE is defined in
    // inc/hw_memmap.h
    ADCSequenceDataGet(ADC0_BASE, 3, &ulValue);
    //
    // Place it in the circular buffer (advancing write index)
    writeCircBuf (&g_inBuffer, ulValue);
    //
    // Clean up, clearing the interrupt
    ADCIntClear(ADC0_BASE, 3);
}

void initClock() {
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
    //
    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet(SysCtlClockGet() / SAMPLE_RATE_HZ);
    //
    // Register the interrupt handler
    SysTickIntRegister(SysTickIntHandler);
    //
    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();
}

void initDisplay() {
    // intialise the Orbit OLED display
    OLEDInitialise ();
}

void initADC() {
    initCircBuf(&g_inBuffer, BUF_SIZE);

    // The ADC0 peripheral must be enabled for configuration and use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    // Enable sample sequence 3 with a processor signal trigger.  Sequence 3
    // will do a single sample when the processor sends a signal to start the
    // conversion.
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

    //
    // Configure step 0 on sequence 3.  Sample channel 0 (ADC_CTL_CH0) in
    // single-ended mode (default) and configure the interrupt flag
    // (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
    // that this is the last conversion on sequence 3 (ADC_CTL_END).  Sequence
    // 3 has only one programmable step.  Sequence 1 and 2 have 4 steps, and
    // sequence 0 has 8 programmable steps.  Since we are only doing a single
    // conversion using sequence 3 we will only configure step 0.  For more
    // on the ADC sequences and steps, refer to the LM3S1968 datasheet.
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE |
                             ADC_CTL_END);

    //
    // Since sample sequence 3 is now configured, it must be enabled.
    ADCSequenceEnable(ADC0_BASE, 3);

    //
    // Register the interrupt handler
    ADCIntRegister (ADC0_BASE, 3, ADCIntHandler);

    //
    // Enable interrupts for ADC0 sequence 3 (clears any outstanding interrupts)
    ADCIntEnable(ADC0_BASE, 3);
}

void displayValue(uint16_t value, displayMode_t mode){
    uint8_t percent = 0;
    char string[17];
    switch(mode){
    case PERCENT:
        //calculate percentage for value

        OLEDStringDraw ("Percent Value:", 0, 0);
        usnprintf (string, sizeof(string),  "%4d %%", percent);
        OLEDStringDraw (string, 0, 1);
        break;
    case ADC_VALUE:
        OLEDStringDraw ("Mean ADC Value:", 0, 0);
        usnprintf (string, sizeof(string), "%4d", value);
        OLEDStringDraw (string, 0, 1);
        break;
    case OFF:
        OLEDStringDraw ("", 0, 0);
        OLEDStringDraw ("", 0, 1);
    }
}

int32_t calculateMeanVal(){
    uint16_t i;
    int32_t sum = 0;
    for (i = 0; i < BUF_SIZE; i++)
        sum = sum + readCircBuf(&g_inBuffer);
    // Calculate and display the rounded mean of the buffer contents
    return ((2 * sum + BUF_SIZE) / 2 / BUF_SIZE);
}

int main(){
    uint16_t i;
    int32_t sum;
    int32_t meanVal = 0;
    displayMode_t displayMode = ADC_VALUE;

    initClock();
    initADC();
    initDisplay();
    initButtons();


//    zeroHeightVal = calculateMeanVal();

    IntMasterEnable();

    while(1){
//        meanVal = calculateMeanVal();
        sum = 0;
        for (i = 0; i < BUF_SIZE; i++)
            sum = sum + readCircBuf (&g_inBuffer);
        // Calculate and display the rounded mean of the buffer contents
        meanVal = (2 * sum + BUF_SIZE) / 2 / BUF_SIZE;


        //perform button state functions
        switch(checkButton(LEFT)){
        case PUSHED:
//            zeroHeightVal = meanVal;
            break;
        }
        switch(checkButton(UP)){
        case PUSHED:
//            displayMode = (displayMode + 1) % ENUM_SIZE;
            break;
        }

//        if(slowTick){
//
//            slowTick = false;
//        }
        displayValue(meanVal, displayMode);
        SysCtlDelay (SysCtlClockGet() / 6);  // Update display at ~ 2 Hz
    }
}

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
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "utils/ustdlib.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "stdlib.h"
#include "buttons4.h"
#include "circBufT.h"
#include "inc/hw_ints.h"  // Interrupts

//*****************************************************************************
// Constants
//*****************************************************************************
#define BUF_SIZE 25
#define SAMPLE_RATE_HZ 100
#define ADC_1V_RANGE 1241

#define SYSTICK_RATE_HZ 100
#define SLOWTICK_RATE_HZ 4

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

volatile uint8_t slowTick = false;

//********************************************************
// Function Definitions
//********************************************************

void SysTickIntHandler() {
    static uint8_t tickCount = 0;
    const uint8_t ticksPerSlow = SYSTICK_RATE_HZ / SLOWTICK_RATE_HZ;

    updateButtons ();       // Poll the buttons
    if (++tickCount >= ticksPerSlow){
        tickCount = 0; // Signal a slow tick
        slowTick = true;
    }

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

//*******************************************************************
void
initSysTick (void)
{
    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet (SysCtlClockGet () / SYSTICK_RATE_HZ);
    //
    // Register the interrupt handler
    SysTickIntRegister (SysTickIntHandler);
    //
    // Enable interrupt and device
    SysTickIntEnable ();
    SysTickEnable ();
}

void initClock() {
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
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
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH9 | ADC_CTL_IE |
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


void initDisplay() {
    // intialise the Orbit OLED display
    OLEDInitialise();
}

void clearDisplay(){
    uint8_t i;
    for(i = 0; i < 4; i++){
        OLEDStringDraw ("                ", 0, i);
    }
}

void displayValue(uint16_t ADCvalue, uint16_t minADCVal, displayMode_t mode){
    int16_t percent = 0;
    char string[17];
    switch(mode){
    case PERCENT:
        percent = (minADCVal - ADCvalue) * 100 / ADC_1V_RANGE;
        OLEDStringDraw ("Percent Value:", 0, 0);
        usnprintf (string, sizeof(string),  "%4d %%", percent);
        OLEDStringDraw (string, 0, 1);
        break;
    case ADC_VALUE:
        OLEDStringDraw ("Mean ADC Value:", 0, 0);
        usnprintf (string, sizeof(string), "%4d", ADCvalue);
        OLEDStringDraw (string, 0, 1);
        break;
    case OFF:
        clearDisplay();
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
    initClock ();
    initADC ();
    initButtons ();
    initDisplay ();
    initSysTick();
    IntMasterEnable(); // Enable interrupts to the processor.

    int32_t minADCVal = 0;
    int32_t meanADCVal = 0;
    displayMode_t displayMode = ADC_VALUE;

    minADCVal = calculateMeanVal();

    while(1){
        meanADCVal = calculateMeanVal();

        //perform button state functions
        if(checkButton(LEFT) == RELEASED){
            minADCVal = meanADCVal;
        }
        if(checkButton(UP) == RELEASED){
            clearDisplay();
            displayMode = (displayMode + 1) % ENUM_SIZE;
        }

        if(slowTick){
            displayValue(meanADCVal, minADCVal, displayMode);
            slowTick = false;
        }
    }
}

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
#include "Display.h"
#include "Altitude.h"
#include "Yaw.h"
#include "PID.h"
#include "PWM.h"
#include "UART.h"
#include "inc/hw_ints.h"  // Interrupts

//*****************************************************************************
// Constants
//*****************************************************************************

#define SYSTICK_RATE_HZ 100
#define SLOWTICK_RATE_HZ 25

//*****************************************************************************
// Global variables
//*****************************************************************************

volatile uint8_t slowTick = false; // Flag used to refresh the OLED display

//********************************************************
// Function Definitions
//********************************************************

void SysTickIntHandler() {
    static uint8_t tickCount = 0;
    const uint8_t ticksPerSlow = SYSTICK_RATE_HZ / SLOWTICK_RATE_HZ;

    updateButtons();       // Poll the buttons
    if (++tickCount >= ticksPerSlow){
        tickCount = 0; // Signal a slow tick
        slowTick = true;
    }

    // Initiate a conversion
    ADCProcessorTrigger(ADC0_BASE, 3);
}

//*******************************************************************
void
initSysTick (void)
{
    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet(SysCtlClockGet () / SYSTICK_RATE_HZ);
    //
    // Register the interrupt handler
    SysTickIntRegister(SysTickIntHandler);
    //
    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();
}

void initClock() {
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
}

int main(){
    // Initialize systems
    initClock();
    initADC();
    initButtons();
    initDisplay();
    initYaw();
    initMotorPWM();
    initUART();
    initSysTick();
    IntMasterEnable(); // Enable interrupts to the processor.

    int32_t landedAlt = 0;
    int32_t currentAlt = 0;
    int32_t currentYaw = 0;
    int32_t currentYawDec = 0;

    int32_t altSetpoint = 0;
    int32_t yawSetpoint = 0;
    int32_t yawDec = 0;

    // Delay for ADC buffer to fill
    SysCtlDelay(SysCtlClockGet() / 4); 

    // Set the initial minADCVal to the initial meanADCVal (starting "landed" value)
    landedAlt = calculateMeanAltVal(),  landedAlt;

    while(1){
        // Calculate the mean value of the ADC buffer
        currentAlt = getAltPercent(calculateMeanAltVal(), landedAlt);
        currentYaw = getYawDegrees();
        currentYawDec = getYawDecimal();

        if(checkButton(RESET) == RELEASED) SysCtlReset();

//        if(checkButton(LEFT) == RELEASED) UARTSend("Left Button Pressed\n\r");
//        if(checkButton(RIGHT) == RELEASED) UARTSend("Right Button Pressed\n\r");
//        if(checkButton(UP) == RELEASED) UARTSend("Up Button Pressed\n\r");
//        if(checkButton(DOWN) == RELEASED) UARTSend("Down Button Pressed\n\r");
        if(checkButton(SWITCH1) == PUSHED) UARTSend("Switch Button has been pushed\n\r");
        if(checkButton(SWITCH1) == RELEASED) UARTSend("Switch Button has been released\n\r");


        // Refresh the OLED display on slow ticks
        if(slowTick){
            displayAlt(currentAlt);
            displayYaw(currentYaw, currentYawDec);

        }
    }
}

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

#define MIN_ALT 0
#define MAX_ALT 100

#define TAKEOFF_SETPOINT 20
#define YAW_REF_INCREMENT 1

#define ALT_INCREMENT 10
#define YAW_INCREMENT 15

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

//*****************************************************************************
// Global variables
//*****************************************************************************
volatile uint8_t slowTick = false; // Flag used to refresh the OLED display
volatile uint8_t oneMSFlag = false;

enum HelicopterStates{
    LANDED = 0,
    TAKEOFF,
    FLYING,
    LANDING
};

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

    enum HelicopterStates currentState;
    currentState = LANDED;
    bool stateShift = false;

    int32_t landedAlt = 0;

    int32_t currentAlt = 0;
    int32_t currentYaw = 0;
    int32_t currentYawDec = 0;

    int32_t altSetpoint = 0;
    int32_t yawSetpoint = 0;


    // Delay for ADC buffer to fill
    SysCtlDelay(SysCtlClockGet() / 4); 

    // Set the initial minADCVal to the initial meanADCVal (starting "landed" value)
    landedAlt = calculateMeanAltVal();

    while(1){
        if(checkButton(RESET) == PUSHED) SysCtlReset();

        switch(currentState){
        case(LANDED):
            if(stateShift){
                toggleRotors();
                stateShift = false;
            }

            if(checkButton(SWITCH1) == PUSHED){
                toggleRotors();
                stateShift = true;
                currentState = TAKEOFF;
            }

            break;
        case(TAKEOFF):
            if(stateShift){
                stateShift = false;
                isYawCalibrated = false;
                enableYawRefInt();
            }

//            // ease setpoint up to TAKEOFF_ALT
            if(altSetpoint < TAKEOFF_SETPOINT){
                altSetpoint++;
            }
            else if(!isYawCalibrated){ // wait until altitude is calibrated before calibrating yaw
                // spin until yaw is calibrated
                yawSetpoint = currentYaw + YAW_REF_INCREMENT;
                yawSetpoint = (yawSetpoint >= 180) ? yawSetpoint - 360 : yawSetpoint;
            }
            else{
                yawSetpoint = 0; //when yaw is calibrated adjust the setpoint to be zeroed as well
//                disableYawRefInt();
                stateShift = true;
                currentState = FLYING;
            }

            mainControlEffort = controllerUpdateMain(altSetpoint, currentAlt);
            tailControlEffort = controllerUpdateTail(yawSetpoint, currentYaw);

            SetMainPWM(mainControlEffort);
            SetTailPWM(tailControlEffort);

            break;
        case(FLYING):
            if(yawSetpoint == currentYaw) intErrTail = 0; //zero integral error when we hit the setpoint to reduce overshoot


            if(checkButton(LEFT) == RELEASED){
                yawSetpoint -= YAW_INCREMENT;
                yawSetpoint = (yawSetpoint <= -180) ? yawSetpoint + 360 : yawSetpoint;
            }
            if(checkButton(RIGHT) == RELEASED){
                yawSetpoint += YAW_INCREMENT;
                yawSetpoint = (yawSetpoint >= 180) ? yawSetpoint - 360 : yawSetpoint;
            }
            if(checkButton(UP) == RELEASED)
                altSetpoint = min(MAX_ALT, altSetpoint + ALT_INCREMENT);
            if(checkButton(DOWN) == RELEASED)
                altSetpoint = max(MIN_ALT, altSetpoint - ALT_INCREMENT);

            mainControlEffort = controllerUpdateMain(altSetpoint, currentAlt);
            tailControlEffort = controllerUpdateTail(yawSetpoint, currentYaw);

            SetMainPWM(mainControlEffort);
            SetTailPWM(tailControlEffort);

            if(checkButton(SWITCH1) == PUSHED){
                stateShift = true;
                currentState = LANDING;
            }

            break;
        case(LANDING):
            if(yawSetpoint == currentYaw) intErrTail = 0; //zero integral error when we hit the setpoint to reduce overshoot

            if(stateShift){
                stateShift = false;
                altSetpoint = TAKEOFF_SETPOINT;
            }

            // if yaw is zeroed and helicopter is at the minimum altitude
            //  transition to LANDED state
            if((currentAlt == MIN_ALT) && currentYaw == 0){
                stateShift = true;
                currentState = LANDED;
            }

            // zero yaw before descending
            if(currentAlt == TAKEOFF_SETPOINT && currentYaw != 0){
                yawSetpoint = 0;
            }
            else if(currentAlt > MIN_ALT && currentYaw == 0){
                altSetpoint = max(MIN_ALT, altSetpoint - ALT_INCREMENT);
            }

            mainControlEffort = controllerUpdateMain(altSetpoint, currentAlt);
            tailControlEffort = controllerUpdateTail(yawSetpoint, currentYaw);

            SetMainPWM(mainControlEffort);
            SetTailPWM(tailControlEffort);
            break;
        }

        // Calculate the mean value of the ADC buffer
        currentAlt = getAltPercent(calculateMeanAltVal(), landedAlt);
        currentYaw = getYawDegrees();
        currentYawDec = getYawDecimal();

        // Refresh the OLED display on slow ticks
        if(slowTick){
            displayAlt(currentAlt);
            displayYaw(currentYaw, currentYawDec);
            displayMainPWM(mainControlEffort);
            displayTailPWM(tailControlEffort);

            //write code to send data over UART
            char string[88];
            usnprintf(string, sizeof(string),  "\r%d, YawSet: %d, YawCur: %d, AltSet: %d, AltCur:%d, MainIErr: %d, TailIErr: %d\n", (int)currentState, yawSetpoint, currentYaw, altSetpoint, currentAlt, intErrMain, intErrTail);
//            usnprintf(string, sizeof(string),  "\rState: %d, YawSet: %d, AltSet: %d\n, isYawCalibated = %d", (int)currentState, yawSetpoint, altSetpoint, isYawCalibrated);
            UARTSend(string);
        }
    }
}

/*
 * PWM.c
 *
 *  Created on: 9/05/2024
 *      Author: tfo49
 */

#include "PWM.h"

void initPWM(uint32_t PERIPH_PWM, uint32_t PERIPH_GPIO, uint32_t GPIO_CONFIG, uint32_t GPIO_BASE, uint32_t GPIO_PIN, uint32_t PWM_BASE, uint32_t PWM_GEN, uint32_t PWM_OUTBIT){
    SysCtlPeripheralEnable(PERIPH_PWM);
    SysCtlPeripheralEnable(PERIPH_GPIO);

    GPIOPinConfigure(GPIO_CONFIG);
    GPIOPinTypePWM(GPIO_BASE, GPIO_PIN);

    PWMGenConfigure(PWM_BASE, PWM_GEN, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenEnable(PWM_BASE, PWM_GEN);
}

void initMotorPWM(){
    initPWM(MAIN_PERIPH_PWM, MAIN_PERIPH_GPIO, MAIN_GPIO_CONFIG, MAIN_GPIO_BASE, MAIN_GPIO_PIN, MAIN_BASE, MAIN_GEN, MAIN_OUTBIT);
    initPWM(TAIL_PERIPH_PWM, TAIL_PERIPH_GPIO, TAIL_GPIO_CONFIG, TAIL_GPIO_BASE, TAIL_GPIO_PIN, TAIL_BASE, TAIL_GEN, TAIL_OUTBIT);
}

void SetPWM(uint32_t controlEffort, uint32_t PWM_BASE, uint32_t PWM_GEN, uint32_t PWM_OUTNUM){
    uint32_t pwmPeriod = SysCtlClockGet() / PWM_DIVIDER / PWM_RATE_HZ;

    PWMGenPeriodSet(PWM_BASE, PWM_GEN, pwmPeriod);
    PWMPulseWidthSet(PWM_BASE, PWM_OUTNUM, pwmPeriod * controlEffort / PWM_DIVISOR);
}

void SetMainPWM(uint32_t controlEffort){
    SetPWM(controlEffort, MAIN_BASE, MAIN_GEN, MAIN_OUTNUM);
}

void SetTailPWM(uint32_t controlEffort){
    SetPWM(controlEffort, TAIL_BASE, TAIL_GEN, TAIL_OUTNUM);
}

void enablePWM(uint32_t pwmBase, uint32_t pwmOutbit){
    PWMOutputState(pwmBase, pwmOutbit, true);
}

void disablePWM(uint32_t pwmBase, uint32_t pwmOutbit){
    PWMOutputState(pwmBase, pwmOutbit, false);
}

void toggleRotors(){
    if(!isRotorEnabled){
        enablePWM(MAIN_BASE, MAIN_OUTBIT);
        enablePWM(TAIL_BASE, TAIL_OUTBIT);
    }
    else{
        disablePWM(MAIN_BASE, MAIN_OUTBIT);
        disablePWM(TAIL_BASE, TAIL_OUTBIT);
    }

}

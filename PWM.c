// *******************************************************
//
// PWM.c
//
// Initialises PWM outputs.
// Manages the PWM output used to drive the motors.
// Handles rotor enabling and disabling.
//
// Authored with by tfo49 & dta82
//
// Created 23/04/2024
//
// *******************************************************
#include "PWM.h"


// Static variables
static bool isRotorEnabled = false;

void initPWM(uint32_t periph_pwm, uint32_t periph_gpio, uint32_t gpio_config, uint32_t gpio_base, uint32_t gpio_pin, uint32_t pwm_base, uint32_t pwm_gen){
    //Generic Function to enable any pwm module on the board with passed parameters
    SysCtlPeripheralEnable(periph_pwm);
    SysCtlPeripheralEnable(periph_gpio);

    GPIOPinConfigure(gpio_config);
    GPIOPinTypePWM(gpio_base, gpio_pin);

    PWMGenConfigure(pwm_base, pwm_gen, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenEnable(pwm_base, pwm_gen);
}

void initMotorPWM(){
    //One public function which can initialize both pwm signals for main and tail rotors
    initPWM(MAIN_PERIPH_PWM, MAIN_PERIPH_GPIO, MAIN_GPIO_CONFIG, MAIN_GPIO_BASE, MAIN_GPIO_PIN, MAIN_BASE, MAIN_GEN);
    initPWM(TAIL_PERIPH_PWM, TAIL_PERIPH_GPIO, TAIL_GPIO_CONFIG, TAIL_GPIO_BASE, TAIL_GPIO_PIN, TAIL_BASE, TAIL_GEN);
}

void SetPWM(uint32_t controlEffort, uint32_t pwm_base, uint32_t pwm_gen, uint32_t pwm_outnum){
    //generic function which can set either main or tail rotor to duty cycle determined by controlEffort variable
    uint32_t pwmPeriod = SysCtlClockGet() / PWM_RATE_HZ;

    PWMGenPeriodSet(pwm_base, pwm_gen, pwmPeriod);
    PWMPulseWidthSet(pwm_base, pwm_outnum, pwmPeriod * controlEffort / PWM_DIVISOR);
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
    // function which will enable or disable PWM modules depending on the current state of the motors
    if(!isRotorEnabled){
        enablePWM(MAIN_BASE, MAIN_OUTBIT);
        enablePWM(TAIL_BASE, TAIL_OUTBIT);
        isRotorEnabled = true;
    }
    else{
        disablePWM(MAIN_BASE, MAIN_OUTBIT);
        disablePWM(TAIL_BASE, TAIL_OUTBIT);
        isRotorEnabled = false;
    }

}

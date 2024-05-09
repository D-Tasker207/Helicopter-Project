/*
 * PWM.h
 *
 *  Created on: 9/05/2024
 *      Author: tfo49
 */

#ifndef PWM_H
#define PWM_H

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/debug.h"
#include "utils/ustdlib.h"
#include "stdlib.h"
#include "inc/hw_ints.h"

#define SYSTICK_RATE_HZ 100
#define PWM_RATE_HZ 100
#define PWM_DIVIDER 4

#define PWM_DIVISOR 100

// Main Rotor Constants
#define MAIN_BASE PWM0_BASE
#define MAIN_GEN PWM_GEN_3
#define MAIN_OUTNUM PWM_OUT_7
#define MAIN_OUTBIT PWM_OUT_7_BIT
#define MAIN_PERIPH_PWM SYSCTL_PERIPH_PWM0
#define MAIN_PERIPH_GPIO SYSCTL_PERIPH_GPIOC
#define MAIN_GPIO_BASE GPIO_PORTC_BASE
#define MAIN_GPIO_CONFIG GPIO_PC5_M0PWM7
#define MAIN_GPIO_PIN GPIO_PIN_5

// Tail Rotor Contstants
#define TAIL_BASE PWM1_BASE
#define TAIL_GEN PWM_GEN_2
#define TAIL_OUTNUM PWM_OUT_5
#define TAIL_OUTBIT PWM_OUT_5_BIT
#define TAIL_PERIPH_PWM SYSCTL_PERIPH_PWM1
#define TAIL_PERIPH_GPIO SYSCTL_PERIPH_GPIOC
#define TAIL_GPIO_BASE GPIO_PORTF_BASE
#define TAIL_GPIO_CONFIG GPIO_PF1_M1PWM5
#define TAIL_GPIO_PIN GPIO_PIN_1

void initMotorPWM();
void SetMainPWM(uint32_t controlEffort);
void SetTailPWM(uint32_t controlEffort);

#endif

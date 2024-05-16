// *******************************************************
//
// UART.h
//
// Handles UART module interface and defines necessary constants
//
// Authored with by tfo49 & dta82
//
// Created 23/04/2024
//
// *******************************************************

#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
#include "utils/ustdlib.h"
#include "stdlib.h"
#include "stdio.h"


//*****************************************************************************
// Constants
//*****************************************************************************
// UART configuration constants
#define BAUD_RATE 9600
#define UART_BASE UART0_BASE
#define UART_PERIPH_UART SYSCTL_PERIPH_UART0
#define UART_PERIPH_GPIO SYSCTL_PERIPH_GPIOA
#define UART_GPIO_BASE GPIO_PORTA_BASE
#define UART_GPIO_PIN_REC GPIO_PIN_0
#define UART_GPIO_PIN_TRX GPIO_PIN_1
#define UART_GPIO_PINS UART_GPIO_PIN_REC | UART_GPIO_PIN_TRX

void initUART();
void UARTSend(char* dataBuffer);

#endif /* UART_H_ */

// *******************************************************
//
// UART.c
//
// Initialises & manages UART communication
//
// Authored with by tfo49 & dta82
//
// Created 23/04/2024
//
// *******************************************************

#include "UART.h"

// initialise uart communication protocol
// must call before attempting to send data over serial connection
void initUART(){
    SysCtlPeripheralEnable(UART_PERIPH_UART);
    SysCtlPeripheralEnable(UART_PERIPH_GPIO);

    GPIOPinTypeUART(UART_GPIO_BASE, UART_GPIO_PINS);
    GPIOPinConfigure(UART_GPIO_PIN_REC);
    GPIOPinConfigure(UART_GPIO_PIN_TRX);

    UARTConfigSetExpClk(UART_BASE, SysCtlClockGet(), BAUD_RATE,
                        UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                        UART_CONFIG_PAR_NONE);

    UARTFIFOEnable(UART_BASE);
    UARTEnable(UART_BASE);
}


// dataBuffer contains string to be sent over UART connection
void UARTSend(char* dataBuffer){
    // loop over dataBuffer and transmit on char at a time
    while(*dataBuffer){
        UARTCharPut(UART_BASE, *dataBuffer);
        dataBuffer++;
    }
}

/*
 * UART.c
 *
 *  Created on: 9/05/2024
 *      Author: tfo49
 */

#include "UART.h"

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


void UARTSend(char* dataBuffer){
    while(dataBuffer){
        UARTCharPut(UART_BASE, *dataBuffer);
        dataBuffer++;
    }
}

// *******************************************************
//
// Altitude.c
//
// Module to handle monitoring helicopter altitude.
// This includes initialising the ADC, registering the ADC read interrupt
// and performing calculations to find the mean ADC val and the % altitude.
// The ADC passes input data to the ci
//
// Authored by tfo49 & dta82
// With sample code by P.J. Bones UCECE from ENCE361 labs
//
// Created 23/04/2024
//
// *******************************************************

#include "Altitude.h"

void initADC() {
    // Initialse the buffer used to store the ADC data
    initCircBuf(&g_inBuffer, BUF_SIZE);

    // The ADC0 peripheral must be enabled for configuration and use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    // Enable sample sequence 3 with a processor signal trigger.  Sequence 3
    // will do a single sample when the processor sends a signal to start the
    // conversion.
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

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

    // Since sample sequence 3 is now configured, it must be enabled.
    ADCSequenceEnable(ADC0_BASE, 3);

    // Register the interrupt handler
    ADCIntRegister (ADC0_BASE, 3, ADCIntHandler);

    // Enable interrupts for ADC0 sequence 3 (clears any outstanding interrupts)
    ADCIntEnable(ADC0_BASE, 3);
}

void ADCIntHandler() {
    uint32_t ulValue;

    // Get the single sample from ADC0.  ADC_BASE is defined in
    // inc/hw_memmap.h
    ADCSequenceDataGet(ADC0_BASE, 3, &ulValue);
    //
    // Place it in the circular buffer (advancing write index)
    writeCircBuf(&g_inBuffer, ulValue);
    //
    // Clean up, clearing the interrupt
    ADCIntClear(ADC0_BASE, 3);
}

int32_t calculateMeanAltVal(){
    // sum over contents of buffer then return rounded mean of contents
    uint16_t i;
    int32_t sum = 0;
    for (i = 0; i < BUF_SIZE; i++)
        sum = sum + readCircBuf(&g_inBuffer);
    return ((2 * sum + BUF_SIZE) / 2 / BUF_SIZE);
}

int16_t getAltPercent(int32_t currentADCVal, int32_t zeroPercentVal){
    //return an percentage altitude value ranging from 0-100 when properly calibrated
    return (int16_t) (zeroPercentVal - currentADCVal) * 100 / ADC_1V_RANGE;
}

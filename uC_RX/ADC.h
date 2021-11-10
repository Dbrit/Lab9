#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include "../inc/CortexM.h"
#include "../inc/tm4c123gh6pm.h"

#define FIFOSIZE	4
#define SAMPLES		256
/*
 * Initializes the ADC at ADC0 for Ain4 (PD3)
 * will trigger the handler w/ Timer0A
 */
int ADC_Init(uint32_t fs);

/*
 * To be called by the decoder; returns an array of size Sample of the full sample set from ADCFifo
 * Increments the ReadSizeInd and mods as necessary
 */
void ADCFifo_Read(uint32_t* SampleSet);

#endif

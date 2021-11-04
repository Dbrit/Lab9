#include <stdint.h>
#include "../inc/CortexM.h"
#include "../inc/tm4c123gh6pm.h"

/*
 * FIFO for holding ADC reads + necessary variables
 */
#define FIFOSIZE	4
#define SAMPLES		50
int ADCFifo[FIFOSIZE][SAMPLES];
int ADCFifoPutSizeInd;
int ADCFifoPutSampleInd;
int ADCFifoReadSizeInd;

/*
 * Initializes the ADC at ADC0 to some port dependent on what channel we decide to use
 * will trigger the handler w/ Timer0A
 */
void ADC_Init(uint32_t period);

/*
 * Handler whenever Timer0A ISR happens
 *  - will put in an ADC read into ADCFifo at PutSizeInd and PutSampleInd
 *	- increment the inds and mod as necessary
 */
void ADC0Seq0_Handler(void);

/*
 * To be called by the decoder; returns an array of size Sample of the full sample set from ADCFifo
 * Increments the ReadSizeInd and mods as necessary
 */
int* ADCFifo_Read(void);

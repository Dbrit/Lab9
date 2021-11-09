#ifndef DECODER_H
#define DECODER_H

#include <stdint.h>
#include <math.h>
#include "../inc/CortexM.h"
#include "../inc/tm4c123gh6pm.h"
#include "ADC.h"
#include "Notes.h"

#define highFreq A2 // 1760.000 Hz
#define lowFreq A // 440.000 Hz

void Decoder_Init(uint32_t maxFreq);

/*
 * will read a sample set from ADC to determine freq from
 */
void Decoder_ReadSample(void);

/*
 * Determines a frequency from the current sample and returns that value;
 * Should approximate to either XHz or YHz where X and Y are the freq of 0 and 1 button presses
 */
int Decoder_Decode(void);

void Timer1_Init(void);

#endif

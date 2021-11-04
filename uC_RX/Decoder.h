#include <stdint.h>
#include "../inc/CortexM.h"
#include "../inc/tm4c123gh6pm.h"

int* sampleSet;

/*
 * will read a sample set from ADC to determine freq from
 */
void Decoder_ReadSample(void);

/*
 * Determines a frequency from the current sample and returns that value;
 * Should approximate to either XHz or YHz where X and Y are the freq of 0 and 1 button presses
 */
int Decoder_Decode(void) 
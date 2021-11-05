#ifndef ENCODER_H
#define ENCODER_H

#include "../inc/CortexM.h"
#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>
#include "Notes.h"
#include "DAC.h"



#define PF2       (*((volatile uint32_t *)0x40025010))


#define PATTERN_LENGTH 4

#define highFreq A2
#define lowFreq A


struct Pattern {
	uint8_t patternLength;
	uint32_t index;                  // pattern index
	const unsigned short* voice;               // waveform selected
	const uint16_t* notes;
	const uint16_t* noteLength; // how long Timer1 runs until next note
};



//Initialize the encoder
void Encoder_Init(void);

//Send encoded 0
//Plays Start, Low, then High Frequency
void Send0(void);

//Send encoded 1
//Plays Start, High, then Low Frequency
void Send1(void);


#endif

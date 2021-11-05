#include "encoder.h"
#include "../inc/CortexM.h"
#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>


// wave index
uint8_t I1; //melody

// globals for traversing song
struct Pattern* pattern;
uint16_t DAC_patternTimer;

uint16_t VolumeMultiplier;
uint16_t PatternLength;

uint8_t currentSignal;
uint8_t previousSignal;
uint8_t newSignal;


// 11-bit 64-element sine wave
const unsigned short sine64[64] = {  
  1024,1122,1219,1314,1407,1495,1580,1658,1731,1797,1855,
  1906,1948,1981,2005,2019,2024,2019,2005,1981,1948,1906,
  1855,1797,1731,1658,1580,1495,1407,1314,1219,1122,1024,
  926,829,734,641,553,468,390,317,251,193,142,
  100,67,43,29,24,29,43,67,100,142,193,
  251,317,390,468,553,641,734,829,926
}; 

const uint16_t sig0[PATTERN_LENGTH] = {highFreq,lowFreq,lowFreq,highFreq};
const uint16_t sig0Lengths[PATTERN_LENGTH] = {1,1,1,1};
const uint16_t sig1[PATTERN_LENGTH] = {highFreq,lowFreq,highFreq,lowFreq};
const uint16_t sig1Lengths[PATTERN_LENGTH] = {1,1,1,1};

struct Pattern pattern0 = {
	PATTERN_LENGTH,
	0,
	sine64,
	sig0,
	sig0Lengths
};

struct Pattern pattern1 = {
	PATTERN_LENGTH,
	0,
	sine64,
	sig1,
	sig1Lengths
};





//Initialize the encoder
void Encoder_Init(void){
	Timer2_Init();               // init melody player
	Timer1_Init();               // init music
	
	pattern = &pattern0;
	pattern->index = 0;
	pattern->voice = sine64;
	
	I1 = 0;
	DAC_patternTimer = 1;
	PatternLength = 0;
	VolumeMultiplier = 50;
	newSignal = 0;
}

//Send encoded 0
//Plays Start, Low, then High Frequency
void Send0(void){
	pattern->index = 0;
	DAC_patternTimer = 100;
	newSignal = 1;
	pattern = &pattern0;
	previousSignal = currentSignal;
	currentSignal = 0;
	TIMER2_IMR_R = 0x00000001; // play
}

//Send encoded 1
//Plays Start, High, then Low Frequency
void Send1(void){
	pattern->index = 0;
	DAC_patternTimer = 100;
	newSignal = 1;
	pattern = &pattern1;
	previousSignal = currentSignal;
	currentSignal = 1;
	TIMER2_IMR_R = 0x00000001; // play
}


#ifndef MUSIC_H
#define MUSIC_H

#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>

#define Harmony

struct Song {
	uint8_t melodyLength;
	uint8_t harmonyLength;
	uint32_t index1;                  // melody index
	uint32_t index2;                   // harmony index
	const unsigned short* voice;               // waveform selected
	const uint16_t* notes1;
	const uint16_t* notes2;
	const uint16_t* noteLength1; // how long Timer1 runs until next note
	const uint16_t* noteLength2; // how long Timer1 runs until next note
//	const uint16_t noteVolume[SONG_LENGTH]; // fraction to multiply DAC value by
	
};


#endif

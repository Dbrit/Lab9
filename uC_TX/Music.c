#include "Music.h"
#include "Notes.h"

#define MELODY_LENGTH 208
#define HARMONY_LENGTH 126

// 11-bit 64-element sine wave
const unsigned short sine64[64] = {  
  1024,1122,1219,1314,1407,1495,1580,1658,1731,1797,1855,
  1906,1948,1981,2005,2019,2024,2019,2005,1981,1948,1906,
  1855,1797,1731,1658,1580,1495,1407,1314,1219,1122,1024,
  926,829,734,641,553,468,390,317,251,193,142,
  100,67,43,29,24,29,43,67,100,142,193,
  251,317,390,468,553,641,734,829,926
}; 


const uint16_t e0 = E0;
const uint16_t f0s = GF0;
const uint16_t g0s = AF0;
const uint16_t a1 = A0;
const uint16_t b1 = B0;
const uint16_t c1s = DF;
const uint16_t d1 = D;
const uint16_t e1 = E;
const uint16_t f1s = GF;
const uint16_t g1s = AF;
const uint16_t a2 = A;
const uint16_t b2 = B;
const uint16_t c2s = DF1;
const uint16_t d2 = D1;
const uint16_t e2 = E1;
const uint16_t f2s = GF1;
const uint16_t g2s = AF1;
const uint16_t a3 = A1;
const uint16_t b3 = B1;
const uint16_t c3s = DF2;
const uint16_t d3 = D2;
const uint16_t e3 = E2;



//32+30+28+30+30+30+28 = 208
const uint16_t RiverMelody[MELODY_LENGTH] = {a3,g2s,a3,g2s,a3,e2,a3,d2,a2,c2s,a3,g2s,a3,g2s,a3,e2,a3,d2,a2,c2s,a3,g2s,a3,a2,g2s,a3,a2,e2,a3,a2,d2,a2,
												c2s,d2,e2,c2s,b2,a2,g1s,a2,e1,a2,b2,c2s,c2s,d2,e2,d2,c2s,b2,a3,g2s,a3,a2,g2s,a3,a2,e2,a3,a2,d2,a2,
												c2s,d2,e2,c3s,b3,e2,c3s,b3,a3,g2s,a3,a2,b2,c2s,e1,a2,c2s,d2,e2,e1,c2s,d2,c2s,b2,a3,b3,a3,g2s,
												a3,a2,e2,a2,a3,b3,a3,g2s,a3,a2,e2,a2,a3,b3,a3,g2s,a3,b3,c3s,d3,e3,c3s,b3,a3,g2s,b2,a3,b3,a3,g2s,
												a3,a2,e2,a2,a3,b3,a3,g2s,a3,a2,e2,a2,a3,b3,a3,g2s,a3,b3,c3s,d3,e3,c3s,b3,a3,g2s,b2,a3,b3,a3,g2s,
												a3,a2,e2,a2,a3,b3,a3,g2s,a3,a2,e2,a2,a3,b3,a3,g2s,a3,b3,c3s,d3,e3,c3s,b3,a3,g2s,b2,a3,b3,a3,g2s,
												a3,a2,e2,a2,a3,b3,a3,g2s,a3,a2,e2,a2,a3,b3,a3,g2s,a3,b3,c3s,d3,e3,c3s,b3,a3,g2s,b2,g1s,e1};

//19+25+19+16+16+16+15 = 126
const uint16_t RiverHarmony[HARMONY_LENGTH] = {f1s,c2s,f2s,d1,a2,e2,f1s,c2s,f2s,d1,a2,e2,f1s,c2s,f2s,d1,a2,e2,d1,
												a1,e1,c2s,e1,b2,e2,f1s,c2s,f2s,d1,a2,e2,a1,e1,c2s,e1,b2,g2s,f1s,c2s,f2s,d1,a2,e2,d1,
												a1,e1,c2s,a1,e1,b2,e2,f1s,c2s,f2s,d1,b2,e2,a1,e1,c2s,e1,b2,e2,
												f1s,c2s,f2s,f1s,d1,a2,e2,d1,a1,e1,c2s,a1,e1,b2,e2,e1,
												f1s,c2s,f2s,f1s,d1,a2,e2,d1,a1,e1,c2s,a1,e1,b2,e2,e1,
												f0s,c1s,f1s,f0s,d1,a2,c2s,d1,a1,e1,c2s,a1,e0,b1,g1s,e0,
												f0s,c1s,f1s,f0s,d1,a2,e2,d1,a1,e1,c2s,a1,e0,b1,g1s};

const uint16_t RiverHarmony2[20] = {f2s, e2, f2s, e2, f2s, e2,
																	a2, e2, f2s, d2, a2, e2, f2s, d2,
																	a2, e2, f2s, d2, a2, e2};										
												
const uint16_t RiverMelodyLength[MELODY_LENGTH] = {2,2,2,2,2,2,2,16,1,1,2,2,2,2,2,2,2,16,1,1,2,1,2,1,1,2,1,1,2,1,1,1,
														2,2,2,2,6,1,1,5,1,1,1,6,1,1,6,1,1,8,2,1,2,1,1,2,1,1,2,1,1,1,
														2,2,2,2,2,2,1,1,1,1,6,1,1,2,2,2,1,1,2,2,2,1,1,4,1,1,1,1,
														1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,1,1,1,1,
														1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,1,1,1,1,
														1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,1,1,1,1,
														1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,20};

const uint16_t RiverHarmonyLength[HARMONY_LENGTH] = {2,2,4,2,2,20,2,2,4,2,2,20,2,2,4,2,2,2,2,
														2,2,4,2,2,4,2,2,4,2,2,4,2,2,4,2,2,4,2,2,4,2,2,2,2,
														2,2,2,2,2,2,4,2,2,4,2,2,4,2,2,4,2,2,4,
														2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
														2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
														2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
														2,2,2,2,2,2,2,2,2,2,2,2,2,2,22};

const uint16_t RiverHarmonyLength2[20] = {8,24,8,24,8,8,
																					8,8,8,8,8,8,8,8,
																					8,8,8,8,8,8};

struct Song river_Song = {
	MELODY_LENGTH,
	HARMONY_LENGTH,
	0,
	0,
	sine64,
	RiverMelody,
	RiverHarmony,
	RiverMelodyLength,
	RiverHarmonyLength
};

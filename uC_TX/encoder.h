#ifndef ENCODER_H
#define ENCODER_H

#include "../inc/CortexM.h"
#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>

#define PF2       (*((volatile uint32_t *)0x40025010))


//Initialize the encoder
void Encoder_Init(void);

//Send encoded 0
//Plays Start, Low, then High Frequency
void Send0(void);

//Send encoded 1
//Plays Start, High, then Low Frequency
void Send1(void);



// Initialize the DAC
// Port D is the SSI1 interface
// 16 bit data, Freescale mode
void DAC_Init(uint16_t data);

// Initialize Timer2 for sound
void Timer2_Init(void);


// Initialize Timer1 for sound length
void Timer1_Init(void);

// send the 16-bit code to the SSI
void DAC_Out(uint16_t code);



#endif
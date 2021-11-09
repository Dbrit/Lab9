// filename ******** Switch.h ************** 
// Lisong Sun and Kerry Tu
// Created: September 30, 2021
// Header file for switch functions
// Lab 5
// TA: Hassan Iqbal
// Last edited: September 30, 2021

#ifndef SWITCH_H
#define SWITCH_H

#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>
#include "encoder.h"

#define PF0       (*((volatile uint32_t *)0x40025004))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF4       (*((volatile uint32_t *)0x40025040))



// Initialize buttons
// Port F for PLAY/PAUSE and REWIND
void Switch_Init(void);

// Initialize PortF
// PF0, PF4 are inputs. PF2 is heartbeat output
// PF0 PLAY/PAUSE, PF4 REWIND
void PortF_Init(void);

// Disable button interrupts
void Buttons_Disable(void);

// Enable button interrupts
void Buttons_Enable(void);

// PLAY/PAUSE + REWIND input ISR
void GPIOPortF_Handler(void);

#endif

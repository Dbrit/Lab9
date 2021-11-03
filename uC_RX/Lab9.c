// filename ******** Lab9.c ************** 
// Lisong Sun, Kerry Tu, Dakota Britton, Casey Kim
// Created: November 3, 2021
// C file with debugging functions as well as main functions for music player
// Lab 9
// TA: Hassan Iqbal
// Last edited: November 3, 2021

// ST7735 Connections
// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected 
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO)
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

//#include <stdio.h>
#include <stdint.h>
//#include "string.h"
#include "../inc/PLL.h"
#include "../inc/tm4c123gh6pm.h"
#include "../inc/LaunchPad.h"
#include "../inc/CortexM.h"
#include "ST7735.h"
#include "../inc/FIFOsimple.h"

int main(void){
	PLL_Init(Bus80MHz);
	ST7735_InitR(INITR_REDTAB);
	ST7735_FillScreen(ST7735_BLACK);
	RxFifo_Init(); // 8 element FIFO
	EnableInterrupts();
	
	char message;
	
	ST7735_SetCursor(0,0);
	ST7735_OutString("Received Message:");
	ST7735_SetCursor(0,1);
	// LCD will output the bits as they arrive
	while (1) {
		if (RxFifo_Get(&message) == RXFIFOSUCCESS) { // new data available
			ST7735_OutChar(message);
		}
	}
} 


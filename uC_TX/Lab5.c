// filename ******** Lab9_uC1.c ************** 
// Lisong Sun, Kerry Tu, Casey Kim, Dakota Britton
// Created: September 30, 2021
// C file for the transmission uC
// Lab 9
// TA: Hassan Iqbal
// Last edited: September 20, 2021

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

// Button Connections
// PC4 up, PC5 right, PC6 left, PC7 down
// PF0, PF4 are inputs. PF2 is heartbeat output

// DAC Connections
// 


//#include <stdio.h>
#include <stdint.h>
//#include "string.h"
#include "../inc/PLL.h"
#include "../inc/tm4c123gh6pm.h"
#include "../inc/LaunchPad.h"
#include "../inc/CortexM.h"
#include "Switch.h"
#include "DAC.h"
#include "ST7735.h"

extern uint8_t currentSignal;
extern uint8_t previousSignal;

int test_switch(void) {
	PLL_Init(Bus80MHz);
	Switch_Init();
	while (1) {
	}
}

int test_DAC(void) {
	PLL_Init(Bus80MHz);
	DAC_Init(0); // DAC should output 1.49V
	while (1) {
		DAC_Out(400);
		DAC_Out(1000);
		DAC_Out(1400);
		DAC_Out(2047);
		DAC_Out(2700);
		DAC_Out(3000);
		DAC_Out(4095); // DAC should output 2.99V
	}
}

int test_speaker(void) {
	PLL_Init(Bus80MHz);
	Switch_Init();
	DAC_Init(2047);
	EnableInterrupts();
	while(1) {
	}
}

int main(void){
	PLL_Init(Bus80MHz);
	Switch_Init();
	DAC_Init(0);
	ST7735_InitR(INITR_REDTAB);
	ST7735_FillScreen(ST7735_BLACK);
	EnableInterrupts();

	while (1) {
		// PF2 ^= 0x04;
		ST7735_SetCursor(0,0);
		ST7735_OutString("Current Signal:");
		ST7735_SetCursor(0,1);
		if(currentSignal == 3){
			ST7735_OutString("None");
		}
		else if(currentSignal == 0){
			ST7735_OutString("0");
		}
		else if(currentSignal == 1){
			ST7735_OutString("1");
		}
		
		ST7735_SetCursor(0,3);
		ST7735_OutString("Previous Signal:");
		ST7735_SetCursor(0,4);
		if(previousSignal == 3){
			ST7735_OutString("None");
		}
		else if(previousSignal == 0){
			ST7735_OutString("0");
		}
		else if(previousSignal == 1){
			ST7735_OutString("1");
		}
	}
} 


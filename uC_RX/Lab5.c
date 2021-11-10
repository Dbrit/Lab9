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
#include "ADC.h"
#include "Decoder.h"
#include "Switch.h"

extern uint8_t LastFreq; //0 if low, 1 if high, 2 if none
extern uint8_t CurrFreq;
extern uint32_t* mag;
extern const unsigned short* SampleSet;

extern uint8_t Start;
extern uint8_t Bit;
extern uint8_t Parity;

int test_ADC(void){
	PLL_Init(Bus80MHz);
	ST7735_InitR(INITR_REDTAB);
	ST7735_FillScreen(ST7735_BLACK);
	ADC_Init(440*10); //440Hz * 10
	EnableInterrupts();
	
	int* message;
	
	ST7735_SetCursor(0,0);
	ST7735_OutString("Received Message:");
	ST7735_SetCursor(0,1);
	// LCD will output the bits as they arrive
	while (1) {
		ST7735_SetCursor(0,1);
		//message = ADCFifo_Read();
		ST7735_OutUDec(*message);
	}
} 

int main1(void){
	PLL_Init(Bus80MHz);
	ST7735_InitR(INITR_REDTAB);
	ST7735_FillScreen(ST7735_BLACK);
	Switch_Init();
	Decoder_Init(1760); //max freq
	EnableInterrupts();
	
	ST7735_SetCursor(0,0);
	ST7735_OutString("Received Message:");
	ST7735_SetCursor(0,1);
	// LCD will output the bits as they arrive
	while (1) {
		ST7735_SetCursor(0,1);
		ST7735_OutString("LowHz Mag:");
		ST7735_OutUDec(LastFreq);
		ST7735_OutString("  ");
		ST7735_SetCursor(0,2);
		ST7735_OutString("HighHz Mag:");
		ST7735_OutUDec(CurrFreq);
		ST7735_OutString("  ");
		
	}
} 

int main(void){
	PLL_Init(Bus80MHz);
	ST7735_InitR(INITR_REDTAB);
	ST7735_FillScreen(ST7735_BLACK);
	Switch_Init();
	Decoder_Init(1760); //max freq
	EnableInterrupts();
	
	int message = 0;
	
	ST7735_SetCursor(0,0);
	ST7735_OutString("Received Message:");
	ST7735_SetCursor(0,1);
	// LCD will output the bits as they arrive
	while (1) {
		ST7735_SetCursor(0,1);
		message = Decoder_Decode();
		switch(message) {
			case 0:
				ST7735_OutUDec(message);
				ST7735_OutString("           ");
			break;
			case 1:
				ST7735_OutUDec(message);
				ST7735_OutString("           ");
			break;
			case 2: // no freq detected
				break;
			case 3:
				ST7735_OutString("Parity Error");
				break;
			default:
				ST7735_OutUDec(message);
				ST7735_OutString("           ");
		}
		
		ST7735_SetCursor(0,2);
		ST7735_OutString("Last Freq:");
		ST7735_OutUDec(LastFreq);
		
		ST7735_SetCursor(0,3);
		ST7735_OutString("Curr Freq:");
		ST7735_OutUDec(CurrFreq);
		
		ST7735_SetCursor(0,4);
		ST7735_OutString("Start:");
		ST7735_OutUDec(Start);
	}
} 

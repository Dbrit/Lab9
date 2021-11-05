#include "DAC.h"
#include "encoder.h"

#define TEMPO 19999999 // Timer1 reload value; 240bpm
#define TEMPO5x (((TEMPO+1)/5)+1) // Tempo but 5x speed


// wave index
extern uint8_t I1; //melody

// globals for traversing song
extern struct Pattern* pattern;
extern uint16_t DAC_patternTimer;

extern uint16_t VolumeMultiplier;
extern uint16_t PatternLength;

extern uint8_t newSignal;

// Initialize the DAC
// Port D is the SSI1 interface
// 16 bit data, Freescale mode
void DAC_Init(uint16_t data) {
	SYSCTL_RCGCSSI_R |= 0x02;    // activate SSI1
	SYSCTL_RCGCGPIO_R |= 0x08;   // activate Port D
	while ((SYSCTL_PRGPIO_R&0x08) == 0) {}; // ready?
	SSI1_CR1_R = 0;              // disable SSI, master mode
	GPIO_PORTD_AFSEL_R |= 0x0B;  // enable alt func on PD3,1,0
	GPIO_PORTD_DEN_R |= 0x0B;    // enable dig I/O on PD3,1,0
	GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R & 0xFFFF0F00) + 0x00002022; // set SSI functions
	GPIO_PORTD_AMSEL_R &= ~0x0B; // disable analog func on PD3,1,0
	
	SSI1_CPSR_R = 0x08;          // SSI Clk prescale; 80MHz/8 = 10MHz SSIClk (should work up to 20MHz)
	SSI1_CR0_R &= ~(0x0000FFF0); // SSI Control 0; SCR(serial clock rate, changes clock speed) = 0 (no more prescaling past CPSR) 
		                           // SPH (serial clock phase) = 0 (data captured on first clock edge)
	SSI1_CR0_R += 0x40;          // SPO (serial clock polarity) = 1 (high when no data transfered)
	                             // FRF (frame format) = 0 (Freescale SPI frame format)
	SSI1_CR0_R |= 0x0F;          // DSS (data size select) = 16-bit data
	SSI1_DR_R = data;            // load 'data' into transmit FIFO
	SSI1_CR1_R |= 0x02;          // enable SSI1
		
	Encoder_Init();
}


// melody timer
void Timer2_Init(void) {
	SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
  TIMER2_CTL_R = 0x00000000;    // 1) disable timer2A during setup
  TIMER2_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER2_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER2_TAILR_R = 2841;    // 4) reload value
  TIMER2_TAPR_R = 0;            // 5) bus clock resolution
  TIMER2_ICR_R = 0x00000001;    // 6) clear timer2A timeout flag
  TIMER2_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 39, interrupt number 23
  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 23 in NVIC
  TIMER2_CTL_R = 0x00000001;    // 10) enable timer2A
}


// tempo timer
void Timer1_Init(void) {
	volatile uint32_t delay;
  DisableInterrupts();
  // **** general initialization ****
  SYSCTL_RCGCTIMER_R |= 0x02;      // activate timer1
  delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
  TIMER1_CTL_R &= ~TIMER_CTL_TAEN; // disable timer1A during setup
  TIMER1_CFG_R = 0;                // configure for 32-bit timer mode
  // **** timer1A initialization ****
                                   // configure for periodic mode
  TIMER1_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
  //TIMER1_TAILR_R = TEMPO;         // start value for 2Hz interrupts (120bpm)
	TIMER1_TAILR_R = TEMPO5x;
  TIMER1_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;// clear timer1A timeout flag
  TIMER1_CTL_R |= TIMER_CTL_TAEN;  // enable timer1A 32-b, periodic, interrupts
  // **** interrupt initialization ****
                                   // Timer1A=priority 1
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF0FFF)|0x00002000; // bits 15-13
  NVIC_EN0_R = 1<<21;              // disable interrupt 21 in NVIC
}

// send the 16-bit code to the SSI
void DAC_Out(uint16_t code) {
	while((SSI1_SR_R&0x02)==0){};// wait until room in FIFO
	SSI1_DR_R = code;            // data out
}

void Timer2A_Handler(void) {
	TIMER2_ICR_R = TIMER_ICR_TATOCINT; // acknowledge
	I1 = (I1+1) & 0x3F;                  // 0 to 63
	DAC_Out((VolumeMultiplier*(pattern->voice[I1]))/128
	);
	//DAC_Out((sine64[I1]+sine64[I2])/2);
}

void Timer1A_Handler(void) {
	TIMER1_ICR_R = TIMER_ICR_TATOCINT; // acknowledge
	PF2 ^= 0x04;
	// PF2 ^= 0x04;
	
	if (!newSignal||pattern->index > pattern->patternLength) { // Stop/pause music
		if(newSignal) {newSignal=0;}
		TIMER2_IMR_R = 0x00000000;    // 7) arm timeout interrupt
	} else {                                                                                      // Next note
			if (DAC_patternTimer >= PatternLength) {
				if (pattern->index == pattern->patternLength) {
					pattern->index++;
				} else {
					VolumeMultiplier = 50;
					TIMER2_TAILR_R = pattern->notes[pattern->index];
					PatternLength = 5*(pattern->noteLength[pattern->index]);
					DAC_patternTimer = 1;
					pattern->index++;
				}
			}
			else {
				DAC_patternTimer++;
			}
	}
	
	//PF2 ^= 0x04;
}

#include "Decoder.h"
#include "ADC.h"

#define TEMPO 19999999 // Timer1 reload value; 240 bpm
#define BAUDRATE 39999999// 2Hz

#define PF2       (*((volatile uint32_t *)0x40025010))

extern void cr4_fft_64_stm32(int* pssOUT, int* pssIN, int Nbin);

int* SampleSet;
int* FFT;

// static globals
uint8_t LastFreq; //0 if low, 1 if high, 2 if none
uint8_t CurrFreq;
uint32_t* mag;
static uint8_t Start;
static uint8_t Bit;
static uint8_t Parity;

void Decoder_Init(uint32_t maxFreq) {
	ADC_Init(maxFreq*2); // fs
	Timer1_Init();
}

/*
 * will read a sample set from ADC to determine freq from
 */
void Decoder_ReadSample(void) {
	SampleSet = ADCFifo_Read();
	uint32_t real;
	uint32_t imag;
	uint32_t magHigh;
	uint32_t magLow;
	// read sample set and apply FFT
	cr4_fft_64_stm32(FFT, SampleSet, 64);
	//calculate wanted freqs (440Hz and 1760Hz)
//	for(int t = 0; t < 32; t++){ // first half
//		real = FFT[t]&0xFFFF; // bottom 16 bits
//		imag = FFT[t]>>16; // top 16 bits
//		mag[t] = sqrt(real*real+imag*imag);
//	}
	
//	real = FFT[31]&0xFFFF; // bottom 16 bits
//	imag = FFT[31]>>16; // top 16 bits
//	magHigh = sqrt(real*real+imag*imag);
//	
	// 440Hz
	real = FFT[7]&0xFFFF; // bottom 16 bits
	imag = FFT[7]>>16; // top 16 bits
	magLow = sqrt(real*real+imag*imag);
	
	
	
	LastFreq = magLow;
	CurrFreq = magHigh;
}

/*
 * Determines a frequency from the current sample and returns that value;
 * Should approximate to either XHz or YHz where X and Y are the freq of 0 and 1 button presses
 * Output values: 0 = low, 1 = high, 2 = N/A, 3 = parity error
 */
int Decoder_Decode(void) {
	if (CurrFreq == 2) { //no freq detected
		Start = 0;
		Parity = 0;
	} else if (Parity) { // check parity
		Parity = 0;
		if (CurrFreq == (Bit^0x01)) {
			return Bit;
		}
		return 3; // parity error
	}	else if (Start) {
		Start = 0;
		if (CurrFreq == 0) { // low
			Bit = 0;
			Parity = 1;
		} else { // high
			Bit = 1;
			Parity = 1;
		}
	} else if (CurrFreq == 0 && LastFreq == 1) { // start seq
		Start = 1;
	}

	return 2;
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
	TIMER1_TAILR_R = BAUDRATE;
  TIMER1_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;// clear timer1A timeout flag
  TIMER1_CTL_R |= TIMER_CTL_TAEN;  // enable timer1A 32-b, periodic, interrupts
  // **** interrupt initialization ****
                                   // Timer1A=priority 1
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF0FFF)|0x00002000; // bits 15-13
  NVIC_EN0_R = 1<<21;              // disable interrupt 21 in NVIC
}

void Timer1A_Handler(void) { // read @ BAUDRATE
	TIMER1_ICR_R = TIMER_ICR_TATOCINT; // acknowledge
	PF2 ^= 0x04;
	
	Decoder_ReadSample();
	Decoder_Decode();
}

#include "Decoder.h"
#include "ADC.h"

#define TEMPO 19999999 // Timer1 reload value; 240 bpm
#define BAUDRATE 39999999// 2Hz

#define PF2       (*((volatile uint32_t *)0x40025010))

extern void cr4_fft_256_stm32(uint32_t* pssOUT, uint32_t* pssIN, uint16_t Nbin);

uint32_t SampleSet[SAMPLES] = {  
  1024,1122,1219,1314,1407,1495,1580,1658,1731,1797,1855,
  1906,1948,1981,2005,2019,2024,2019,2005,1981,1948,1906,
  1855,1797,1731,1658,1580,1495,1407,1314,1219,1122,1024,
  926,829,734,641,553,468,390,317,251,193,142,
  100,67,43,29,24,29,43,67,100,142,193,
  251,317,390,468,553,641,734,829,926,
	1024,1122,1219,1314,1407,1495,1580,1658,1731,1797,1855,
  1906,1948,1981,2005,2019,2024,2019,2005,1981,1948,1906,
  1855,1797,1731,1658,1580,1495,1407,1314,1219,1122,1024,
  926,829,734,641,553,468,390,317,251,193,142,
  100,67,43,29,24,29,43,67,100,142,193,
  251,317,390,468,553,641,734,829,926,
	1024,1122,1219,1314,1407,1495,1580,1658,1731,1797,1855,
  1906,1948,1981,2005,2019,2024,2019,2005,1981,1948,1906,
  1855,1797,1731,1658,1580,1495,1407,1314,1219,1122,1024,
  926,829,734,641,553,468,390,317,251,193,142,
  100,67,43,29,24,29,43,67,100,142,193,
  251,317,390,468,553,641,734,829,926,
	1024,1122,1219,1314,1407,1495,1580,1658,1731,1797,1855,
  1906,1948,1981,2005,2019,2024,2019,2005,1981,1948,1906,
  1855,1797,1731,1658,1580,1495,1407,1314,1219,1122,1024,
  926,829,734,641,553,468,390,317,251,193,142,
  100,67,43,29,24,29,43,67,100,142,193,
  251,317,390,468,553,641,734,829,926
};

uint32_t FFT[SAMPLES] = {0,};

// static globals
uint8_t LastFreq; //0 if low, 1 if high, 2 if none
uint8_t CurrFreq;
uint32_t mag[SAMPLES] = {0,};
uint8_t Start;
uint8_t Bit;
uint8_t Parity;

void Decoder_Init(uint32_t maxFreq) {
	ADC_Init(maxFreq*2); // fs
	Timer1_Init();
}

/*
 * will read a sample set from ADC to determine freq from
 */
void Decoder_ReadSample(void) {
	ADCFifo_Read(SampleSet);
	int16_t real;
	int16_t imag;
	uint32_t magHigh;
	uint32_t magLow;
	// read sample set and apply FFT
	cr4_fft_256_stm32(FFT, SampleSet, 256);
	//calculate wanted freqs (440Hz and 1760Hz)
//	for(int t = 0; t < 128; t++){ // first half
//		real = (int16_t) FFT[t]&0xFFFF; // bottom 16 bits
//		imag = (int16_t) FFT[t]>>16; // top 16 bits
//		mag[t] = sqrt(real*real+imag*imag);
//	}
	 // 1760 Hz
	real = (int16_t) FFT[128]&0xFFFF; // bottom 16 bits
	imag = (int16_t) FFT[128]>>16; // top 16 bits
	magHigh = sqrt(real*real+imag*imag);
	
	// 440Hz
	real = (int16_t) FFT[32]&0xFFFF; // bottom 16 bits
	imag = (int16_t) FFT[32]>>16; // top 16 bits
	magLow = sqrt(real*real+imag*imag);

	LastFreq = CurrFreq;
	if (magLow < 10 && magHigh < 10) {
		CurrFreq = 2; 
	} else if (magLow > magHigh) {
		CurrFreq = 0; 
	} else {
		CurrFreq = 1; 
	}
	
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
//	} else if (Parity) { // check parity
//		Parity = 0;
//		Start = 0;
//		if (CurrFreq == (Bit^0x01)) {
//			return Bit;
//		}
//		return 3; // parity error
//		return Bit;
	}	else if (Start) {
		if (CurrFreq == 0) { // low
			return 0;
//			Bit = 0;
//			Parity = 1;
		} else { // high
			return 1;
//			Bit = 1;
//			Parity = 1;
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
	TIMER1_TAILR_R = TEMPO;
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

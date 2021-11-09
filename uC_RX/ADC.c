#include <stdint.h>
#include "../inc/CortexM.h"
#include "../inc/tm4c123gh6pm.h"
#include "ADC.h"

#define NVIC_EN0_INT17          0x00020000  // Interrupt 17 enable

#define TIMER_CFG_16_BIT        0x00000004  // 16-bit timer configuration,
                                            // function is controlled by bits
                                            // 1:0 of GPTMTAMR and GPTMTBMR
#define TIMER_TAMR_TACDIR       0x00000010  // GPTM Timer A Count Direction
#define TIMER_TAMR_TAMR_PERIOD  0x00000002  // Periodic Timer mode
#define TIMER_CTL_TAOTE         0x00000020  // GPTM TimerA Output Trigger
                                            // Enable
#define TIMER_CTL_TAEN          0x00000001  // GPTM TimerA Enable
#define TIMER_IMR_TATOIM        0x00000001  // GPTM TimerA Time-Out Interrupt
                                            // Mask
#define TIMER_TAILR_TAILRL_M    0x0000FFFF  // GPTM TimerA Interval Load
                                            // Register Low

#define ADC_ACTSS_ASEN3         0x00000008  // ADC SS3 Enable
#define ADC_RIS_INR3            0x00000008  // SS3 Raw Interrupt Status
#define ADC_IM_MASK3            0x00000008  // SS3 Interrupt Mask
#define ADC_ISC_IN3             0x00000008  // SS3 Interrupt Status and Clear
#define ADC_EMUX_EM3_M          0x0000F000  // SS3 Trigger Select mask
#define ADC_EMUX_EM3_TIMER      0x00005000  // Timer
#define ADC_SSPRI_SS3_4TH       0x00003000  // fourth priority
#define ADC_SSPRI_SS2_3RD       0x00000200  // third priority
#define ADC_SSPRI_SS1_2ND       0x00000010  // second priority
#define ADC_SSPRI_SS0_1ST       0x00000000  // first priority
#define ADC_PSSI_SS3            0x00000008  // SS3 Initiate
#define ADC_SSCTL3_TS0          0x00000008  // 1st Sample Temp Sensor Select
#define ADC_SSCTL3_IE0          0x00000004  // 1st Sample Interrupt Enable
#define ADC_SSCTL3_END0         0x00000002  // 1st Sample is End of Sequence
#define ADC_SSCTL3_D0           0x00000001  // 1st Sample Diff Input Select
#define ADC_SSFIFO3_DATA_M      0x00000FFF  // Conversion Result Data mask
#define ADC_PC_SR_M             0x0000000F  // ADC Sample Rate
#define ADC_PC_SR_125K          0x00000001  // 125 ksps
#define SYSCTL_RCGCGPIO_R4      0x00000010  // GPIO Port E Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCGPIO_R3      0x00000008  // GPIO Port D Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCGPIO_R1      0x00000002  // GPIO Port B Run Mode Clock
                                            // Gating Control
/*
 * FIFO for holding ADC reads + necessary variables
 */

int ADCFifo[FIFOSIZE][SAMPLES];
int ADCFifoPutSizeInd = 0;
int ADCFifoPutSampleInd = 0;
int ADCFifoReadSizeInd = 0;


// Sample the ADC using Timer0 hardware triggering using SS0
// this function initiates the sampling, does not wait for completion
// task is user function to process or send data
int ADC_Init(uint32_t fs){
  volatile uint32_t delay;
	uint32_t period;  
  // channelNum must be 0-11 (inclusive) corresponding to channel 0 through 11
  period = 80000000/fs;  // timer ticks per sample, 80MHz

  // **** GPIO pin initialization ****
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R3; 
  delay = SYSCTL_RCGCGPIO_R;      // 2) allow time for clock to stabilize
  delay = SYSCTL_RCGCGPIO_R;
																	//      Ain4 is on PD3
  GPIO_PORTD_DIR_R &= ~0x08;  // 3.4) make PD3 input
  GPIO_PORTD_AFSEL_R |= 0x08; // 4.4) enable alternate function on PD3
  GPIO_PORTD_DEN_R &= ~0x08;  // 5.4) disable digital I/O on PD3
  GPIO_PORTD_AMSEL_R |= 0x08; // 6.4) enable analog functionality on PD3

// DisableInterrupts();
  SYSCTL_RCGCADC_R |= 0x01;     // activate ADC0 
  SYSCTL_RCGCTIMER_R |= 0x01;   // activate timer0 
  delay = SYSCTL_RCGCTIMER_R;   // allow time to finish activating
  TIMER0_CTL_R = 0x00000000;    // disable timer0A during setup
  TIMER0_CTL_R |= 0x00000020;   // enable timer0A trigger to ADC
  TIMER0_CFG_R = 0;             // configure for 32-bit timer mode
  TIMER0_TAMR_R = 0x00000002;   // configure for periodic mode, default down-count settings
  TIMER0_TAPR_R = 0;            // prescale value for trigger
  TIMER0_TAILR_R = period-1;    // start value for trigger
  TIMER0_IMR_R = 0x00000000;    // disable all interrupts
  TIMER0_CTL_R |= 0x00000001;   // enable timer0A 32-b, periodic, no interrupts
  ADC0_PC_R = 0x01;         // configure for 125K samples/sec
  ADC0_SSPRI_R = 0x3210;    // sequencer 0 is highest, sequencer 3 is lowest
  ADC0_ACTSS_R &= ~0x01;    // disable sample sequencer 0
  ADC0_EMUX_R = (ADC0_EMUX_R&0xFFFFFFF0)+0x0005; // timer trigger event
  ADC0_SSMUX0_R = 4;
  ADC0_SSCTL0_R = 0x06;          // set flag and end                       
  ADC0_IM_R |= 0x01;             // enable SS0 interrupts
  ADC0_ACTSS_R |= 0x01;          // enable sample sequencer 0
  NVIC_PRI3_R = (NVIC_PRI3_R&0xFF00FFFF)|0x00400000; // bits 21-23
  NVIC_EN0_R |= 1<<14;           // enable interrupt 14 in NVIC, ADC sequence 0
  return 1;
}

/*
 * Handler whenever Timer0A ISR happens
 *  - will put in an ADC read into ADCFifo at PutSizeInd and PutSampleInd
 *	- increment the inds and mod as necessary
 */
void ADC0Seq0_Handler(void){ uint32_t data;
  ADC0_ISC_R = 0x01;     // acknowledge ADC sequence 0 completion
  
	ADCFifo[ADCFifoPutSizeInd][ADCFifoPutSampleInd] = ADC0_SSFIFO0_R;
	
  ADCFifoPutSampleInd++;
	if(ADCFifoPutSampleInd >= SAMPLES)
	{
		ADCFifoPutSampleInd = 0;
		ADCFifoPutSizeInd++;
		if(ADCFifoPutSizeInd >= FIFOSIZE)
		{
			ADCFifoPutSizeInd = 0;
		}
	}
}

int* ADCFifo_Read()
{
	int* sampleSet = ADCFifo[ADCFifoReadSizeInd];
	if(ADCFifoReadSizeInd == ADCFifoPutSizeInd)
	{}
	else if(ADCFifoReadSizeInd < FIFOSIZE - 1)
	{
		ADCFifoReadSizeInd = 0;
	}
	else
	{
		ADCFifoReadSizeInd++;
	}
	
	return sampleSet;
}


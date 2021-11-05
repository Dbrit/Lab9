// filename ******** Switch.c ************** 
// Lisong Sun and Kerry Tu
// Created: September 30, 2021
// Header file for switch functions
// Lab 5
// TA: Hassan Iqbal
// Last edited: September 30, 2021

#include "Switch.h"

void Debounce_Init(void) {
	volatile uint32_t delay;
	SYSCTL_RCGCTIMER_R |= 0x10;   // 0) activate TIMER4
	delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
}

static void GPIOArm(void) {
	// arm PortF
	GPIO_PORTF_ICR_R = 0x11;      // (e) clear flag
  GPIO_PORTF_IM_R |= 0x11;      // (f) arm interrupt on PF4,0
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00600000; // (g) priority 3
  NVIC_EN0_R = 1<<30;               // enable interrupt 30 in NVIC
}

static void Timer4Arm(void) {
	TIMER4_CTL_R &= ~TIMER_CTL_TAEN;  // disable timer4A 32-b, one-shot, interrupts
  TIMER4_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER4_TAMR_R = 0x00000001;   // 3) configure for periodic mode, default down-count settings
  TIMER4_TAILR_R = 15999999;         // 200 ms reload
  TIMER4_TAPR_R = 0;            // 5) bus clock resolution
  TIMER4_ICR_R = 0x00000001;    // 6) clear TIMER3A timeout flag
  TIMER4_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI17_R = (NVIC_PRI17_R&0xFF00FFFF); // priority 0
// interrupts enabled in the main program after all devices initialized
// vector number 86, interrupt number 70
  NVIC_EN2_R = 0x00000040;      // 9) enable interrupt 70 in NVIC
  TIMER4_CTL_R |= TIMER_CTL_TAEN;    // 10) enable TIMER4A
}


// Initialize PortF
// PF0, PF4 are inputs. PF2 is heartbeat output
void PortF_Init(void) {
	SYSCTL_RCGCGPIO_R |= 0x20;        // 1) activate clock for Port F
  while((SYSCTL_PRGPIO_R&0x20)==0){}; // allow time for clock to start
                                    
	GPIO_PORTF_LOCK_R  = 0x4C4F434B;  // 2) unlock GPIO Port F
  GPIO_PORTF_CR_R    = 0x1F;        // allow changes to PF4-0
  GPIO_PORTF_PCTL_R &= ~0x000F0F0F; // 3) regular GPIO
  GPIO_PORTF_AMSEL_R &= ~0x15;      // 4) disable analog function on PF2, PF4, PF0
  GPIO_PORTF_PUR_R |= 0x11;         // 5) pullup for PF4, PF0
  GPIO_PORTF_DIR_R |= 0x04;         // 5) set direction to output
  GPIO_PORTF_AFSEL_R &= ~0x15;      // 6) regular port function
  GPIO_PORTF_DEN_R |= 0x15;         // 7) enable digital port
		
	// enable interrupts
  GPIO_PORTF_IS_R &= ~0x11;         // PF4,0 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x11;        // PF4,0 is not both edges
	GPIO_PORTF_IEV_R &= ~0x11;         // PF4,0 is rising edge event
		
		// arm PortF
	GPIO_PORTF_ICR_R = 0x11;      // (e) clear flag
  GPIO_PORTF_IM_R |= 0x11;      // (f) arm interrupt on PF4,0
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00200000; // (g) priority 1
  NVIC_EN0_R = 1<<30;               // enable interrupt 30 in NVIC
	
};

// Disable button interrupts
void Buttons_Disable(void) {
	GPIO_PORTF_IM_R &= ~0x11; //disable interrupts on PF4,0
}

// Enable button interrupts
void Buttons_Enable(void) {
	GPIO_PORTF_IM_R |= 0x11; //enable interrupts on PF4,0
}

// Initialize buttons
// Port F for Send 1/0
void Switch_Init(void) {
	PortF_Init();
	GPIOArm();
	Debounce_Init();
};

// Send 1/0 ISR
void GPIOPortF_Handler(void) {
	//GPIO_PORTF_ICR_R = 0x11;      // (e) clear flag
	Buttons_Disable();

	Timer4Arm();
};

void Timer4A_Handler(void) {
	TIMER4_IMR_R = 0x00000000;// disarm interrupts
	TIMER4_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER4A timeout
	
	// process PortF inputs
	if (GPIO_PORTF_RIS_R & 0x10) { //PF4, transmit 1
		Send1();
	}
	if (GPIO_PORTF_RIS_R & 0x01) { //PF0, transmit 0
		Send0();
	}
	
	GPIOArm();
}

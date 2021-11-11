#include "MKL25Z4.h"                    // Device header

#define TRIGGER 12 				//PortA Pin 12
#define ECHO 6 						//PortD Pin 6

#define MASK(x) (1 << (x))

uint32_t threshold = 3500;
volatile uint32_t hasStarted = 0; 
uint32_t stage = 0; 
volatile uint32_t timerStart = 0;
volatile uint32_t timerEnd = 0;

void initUltraSonic(void) {
	
	/***********************   TRIGGER   **********************************************************/
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK; 							//Enable Clock to PORTA 
	PORTA->PCR[TRIGGER] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[TRIGGER] |= PORT_PCR_MUX(1); 					//Configure MUX settings to make TRIGGER TPM
	PTA->PDDR |= MASK(TRIGGER); 											//Set PTA12 to GPIO output
		
	/*********************   ECHO   ****************************************************************/
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK; 							//Enable Clock to PORTD
	PORTD->PCR[ECHO] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[ECHO] |= PORT_PCR_MUX(1);							//Configure MUX setting to GPIO
	
	PORTD->PCR[ECHO] |= PORT_PCR_IRQC(0x0b);					//enable PortD 6 interrupt
	PORTD->PCR[ECHO] &= ~PORT_PCR_PS_MASK;							//enable pull select
	PORTD->PCR[ECHO] |= PORT_PCR_PE_MASK;							//enable pull enable
	
	NVIC_EnableIRQ(PORTD_IRQn);
}


#include "MKL25Z4.h"

#define RED_LED 18 				//PortB Pin 18
#define GREEN_LED 19 			//PortB Pin 19
#define BLUE_LED 5 				//PortD Pin 1

#define TRIGGER 12 				//PortA Pin 12
#define ECHO 6 						//PortD Pin 6

#define MASK(x) (1 << (x)) 

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
	PORTD->PCR[ECHO] |= PORT_PCR_PS_MASK;							//enable pull select
	PORTD->PCR[ECHO] |= PORT_PCR_PE_MASK;							//enable pull enable
	
	NVIC_EnableIRQ(PORTD_IRQn);
}


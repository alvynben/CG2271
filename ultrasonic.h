#include "MKL25Z4.h"                    // Device header

// Define pins to control HC-SR04 Ultrasonic Sensor.
#define TRIGGER 12 				// PortA Pin 12
#define ECHO 6 						// PortD Pin 6

#define MASK(x) (1 << (x))

// If timeDiff between rising and falling edge < threshold, 
// the BOT is close to an obstacle.
uint32_t threshold = 30000;

// Initialises hasStarted = 0 to denote that next 
// edge received by ECHO pin is a rising edge.
volatile uint32_t hasStarted = 0; 

// Initialises stage = 0 to denote SELF-DRIVING has not started.
uint32_t stage = 0; 

// Initialise values to keep track of time when
// rising edge is received in timerStart and when
// falling edge is received in timerEnd.
volatile uint32_t timerStart = 0;
volatile uint32_t timerEnd = 0;

// Initialises the HC-SR04 Ultrasonic Sensor.
void initUltraSonic(void) {
	
	/***********************   TRIGGER   **********************************************************/
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK; 							// Enable Clock to PORTA.
	PORTA->PCR[TRIGGER] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[TRIGGER] |= PORT_PCR_MUX(1); 					// Configure MUX settings to make TRIGGER TPM.
	PTA->PDDR |= MASK(TRIGGER); 											// Set PTA12 to GPIO output.
		
	/*********************   ECHO   ****************************************************************/
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK; 							// Enable Clock to PORTD.
	PORTD->PCR[ECHO] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[ECHO] |= PORT_PCR_MUX(1);							// Configure MUX setting to GPIO.
	
	PORTD->PCR[ECHO] |= PORT_PCR_IRQC(0x0b);					// Enable PortD 6 Interrupt.
	PORTD->PCR[ECHO] &= ~PORT_PCR_PS_MASK;						// Enable Pull Select.
	PORTD->PCR[ECHO] |= PORT_PCR_PE_MASK;							// Enable Pull Enable.
	
	NVIC_EnableIRQ(PORTD_IRQn);												// Enable PORTD_IRQ.
}



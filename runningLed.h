#include "MKL25Z4.h"

// Define pins for Green LEDs.
#define PTE_GREEN_LED_1 20
#define PTE_GREEN_LED_2 21
#define PTE_GREEN_LED_3 22
#define PTC_GREEN_LED_4 5
#define PTE_GREEN_LED_5 29
#define PTE_GREEN_LED_6 30
#define PTC_GREEN_LED_7 4
#define PTC_GREEN_LED_8 3
#define PTC_GREEN_LED_9 0
#define PTC_GREEN_LED_10 7

#define MASK(x) (1 << (x))

// Enum to denote if LED should be ON or OFF.
typedef enum {ON, OFF} ledStatus;

// Set the first LED to turn on when BOT starts running.
volatile int ledNum = 1;

void InitGreenLED(void) {
	// Enable Clock to Port E&C.
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
	
	// Set all pins to GPIO.
	PORTE->PCR[PTE_GREEN_LED_1] &= ~PORT_PCR_MUX_MASK; // Reset/Clear
	PORTE->PCR[PTE_GREEN_LED_1] |= PORT_PCR_MUX(1); // Set to Alternative 1
	PORTE->PCR[PTE_GREEN_LED_2] &= ~PORT_PCR_MUX_MASK; // Reset/Clear
	PORTE->PCR[PTE_GREEN_LED_2] |= PORT_PCR_MUX(1); // Set to Alternative 1
	PORTE->PCR[PTE_GREEN_LED_3] &= ~PORT_PCR_MUX_MASK; // Reset/Clear
	PORTE->PCR[PTE_GREEN_LED_3] |= PORT_PCR_MUX(1); // Set to Alternative 1
	PORTC->PCR[PTC_GREEN_LED_4] &= ~PORT_PCR_MUX_MASK; // Reset/Clear
	PORTC->PCR[PTC_GREEN_LED_4] |= PORT_PCR_MUX(1); // Set to Alternative 1
	PORTE->PCR[PTE_GREEN_LED_5] &= ~PORT_PCR_MUX_MASK; // Reset/Clear
	PORTE->PCR[PTE_GREEN_LED_5] |= PORT_PCR_MUX(1); // Set to Alternative 1
	PORTE->PCR[PTE_GREEN_LED_6] &= ~PORT_PCR_MUX_MASK; // Reset/Clear
	PORTE->PCR[PTE_GREEN_LED_6] |= PORT_PCR_MUX(1); // Set to Alternative 1
	PORTC->PCR[PTC_GREEN_LED_7] &= ~PORT_PCR_MUX_MASK; // Reset/Clear
	PORTC->PCR[PTC_GREEN_LED_7] |= PORT_PCR_MUX(1); // Set to Alternative 1
	PORTC->PCR[PTC_GREEN_LED_8] &= ~PORT_PCR_MUX_MASK; // Reset/Clear
	PORTC->PCR[PTC_GREEN_LED_8] |= PORT_PCR_MUX(1); // Set to Alternative 1
	PORTC->PCR[PTC_GREEN_LED_9] &= ~PORT_PCR_MUX_MASK; // Reset/Clear
	PORTC->PCR[PTC_GREEN_LED_9] |= PORT_PCR_MUX(1); // Set to Alternative 1
	PORTC->PCR[PTC_GREEN_LED_10] &= ~PORT_PCR_MUX_MASK; // Reset/Clear
	PORTC->PCR[PTC_GREEN_LED_10] |= PORT_PCR_MUX(1); // Set to Alternative 1

	// Set pins to output.
	PTE->PDDR |= MASK(PTE_GREEN_LED_1) | MASK(PTE_GREEN_LED_2) | MASK(PTE_GREEN_LED_3) | MASK(PTE_GREEN_LED_5) | MASK(PTE_GREEN_LED_6);
	PTC->PDDR |= MASK(PTC_GREEN_LED_7) | MASK(PTC_GREEN_LED_8) | MASK(PTC_GREEN_LED_9) | MASK(PTC_GREEN_LED_10) | MASK(PTC_GREEN_LED_4) ;
}

// Control a single LED.
// Choose if the LED is ON or OFF with status.
// Choose which LED to affect based on ledNum {1 - 10}.
void greenLedControlSingle(ledStatus status, int ledNum) {
	switch(ledNum) {
		case 1:
			if (status) {
				PTE->PCOR |= MASK(PTE_GREEN_LED_1);
			} else {
				PTE->PSOR |= MASK(PTE_GREEN_LED_1);
			}
			break;
		case 2:
			if (status) {
				PTE->PCOR |= MASK(PTE_GREEN_LED_2);
			} else {
				PTE->PSOR |= MASK(PTE_GREEN_LED_2);
			}
			break;
		case 3:
			if (status) {
				PTE->PCOR |= MASK(PTE_GREEN_LED_3);
			} else {
				PTE->PSOR |= MASK(PTE_GREEN_LED_3);
			}
			break;
		case 4:
			if (status) {
				PTC->PCOR |= MASK(PTC_GREEN_LED_4);
			} else {
				PTC->PSOR |= MASK(PTC_GREEN_LED_4);
			}
			break;
		case 5:
			if (status) {
				PTE->PCOR |= MASK(PTE_GREEN_LED_5);
			} else {
				PTE->PSOR |= MASK(PTE_GREEN_LED_5);
			}
			break;
		case 6:
			if (status) {
				PTE->PCOR |= MASK(PTE_GREEN_LED_6);
			} else {
				PTE->PSOR |= MASK(PTE_GREEN_LED_6);
			}
			break;
		case 7:
			if (status) {
				PTC->PCOR |= MASK(PTC_GREEN_LED_7);
			} else {
				PTC->PSOR |= MASK(PTC_GREEN_LED_7);
			}
			break;
		case 8:
			if (status) {
				PTC->PCOR |= MASK(PTC_GREEN_LED_8);
			} else {
				PTC->PSOR |= MASK(PTC_GREEN_LED_8);
			}
			break;
		case 9:
			if (status) {
				PTC->PCOR |= MASK(PTC_GREEN_LED_9);
			} else {
				PTC->PSOR |= MASK(PTC_GREEN_LED_9);
			}
			break;
		case 10:
			if (status) {
				PTC->PCOR |= MASK(PTC_GREEN_LED_10);
			} else {
				PTC->PSOR |= MASK(PTC_GREEN_LED_10);
			}
			break;
		default:
			break;
	}
}

// Sets all green LEDs to ON or OFF.
void greenLedControlAll(ledStatus status) {
	if (status) {
		PTE->PCOR |= MASK(PTE_GREEN_LED_1);
		PTE->PCOR |= MASK(PTE_GREEN_LED_2);
		PTE->PCOR |= MASK(PTE_GREEN_LED_3);
		PTC->PCOR |= MASK(PTC_GREEN_LED_4);
		PTE->PCOR |= MASK(PTE_GREEN_LED_5);
		PTE->PCOR |= MASK(PTE_GREEN_LED_6);
		PTC->PCOR |= MASK(PTC_GREEN_LED_7);
		PTC->PCOR |= MASK(PTC_GREEN_LED_8);
		PTC->PCOR |= MASK(PTC_GREEN_LED_9);
		PTC->PCOR |= MASK(PTC_GREEN_LED_10);
	} else {
		PTE->PSOR |= MASK(PTE_GREEN_LED_1);
		PTE->PSOR |= MASK(PTE_GREEN_LED_2);
		PTE->PSOR |= MASK(PTE_GREEN_LED_3);
		PTC->PSOR |= MASK(PTC_GREEN_LED_4);
		PTE->PSOR |= MASK(PTE_GREEN_LED_5);
		PTE->PSOR |= MASK(PTE_GREEN_LED_6);
		PTC->PSOR |= MASK(PTC_GREEN_LED_7);
		PTC->PSOR |= MASK(PTC_GREEN_LED_8);
		PTC->PSOR |= MASK(PTC_GREEN_LED_9);
		PTC->PSOR |= MASK(PTC_GREEN_LED_10);
	}
}

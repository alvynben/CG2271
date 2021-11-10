//blink led strip

/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"

#define STRIP_1 5 // Pin 5
#define MASK(x) (1 << (x))
#define allLow 000000000000000000000

enum bool{true = 1, false = 0};

void InitGPIOLedBlink(void)
{
	// Enable Clock to PORTE
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	// Configure MUX settings to make STRIP_1 GPIO
	PORTE->PCR[STRIP_1] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[STRIP_1] |= PORT_PCR_MUX(1);
	// Set Data Direction Registers for Port E
	PTE->PDDR |= MASK(STRIP_1);
}

void led_control(enum bool isOn) {
	if(isOn == true) {//red
		PTE->PDOR |= MASK(STRIP_1);
	} else {
		PTE->PDOR &= ~MASK(STRIP_1);
	}	
}
 



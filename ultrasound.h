#include "MKL25Z4.h"

#define PTE31_Pin 31 // pwm trigger
#define PTC9_Pin 9 // echo
#define MASK(x) (1 << (x))

void InitUltrasound() {
	//enable clock
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;

	//configuring ports
	PORTE->PCR[PTE31_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[PTE31_Pin] |= PORT_PCR_MUX(3);
	
	PORTC->PCR[PTC9_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[PTC9_Pin] |= PORT_PCR_MUX(3);
	
	// enable TPM0 clock
	 SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;

	// Select Clock for TPM module
  // TPMSRC = Clk source [using MCGFLLCLK]
  SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
  SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
	
	
}
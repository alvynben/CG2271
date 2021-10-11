#include "MKL25Z4.h"

#define PTB0_Pin 0
#define PTB1_Pin 1

#define MASK(x) (1 << (x))

/* Initialize PWM */
void initPWM(void) {
	// Enable Clock to PORTB
	// Need to enable port power because need to configure the registers for the MUX
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
	
	// Configure MUX settings to make pins GPIO
	// Configure Mode 3 for the new PWM pin operation
	// Pin 0
	PORTB->PCR[PTB0_Pin] &= ~PORT_PCR_MUX_MASK;
	// Alternative 3 (chip specific) Timer Module Channel 0
	PORTB->PCR[PTB0_Pin] |= PORT_PCR_MUX(3);
	
	// Pin 1
	PORTB->PCR[PTB1_Pin] &= ~PORT_PCR_MUX_MASK;
	// Alternative 3 (chip specific) Timer Module Channel 1
	PORTB->PCR[PTB1_Pin] |= PORT_PCR_MUX(3);
	
	// Enable Clock for Timer 1
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
	
	// Select Clock for TPM module
	// TPMSRC = Clk source [using MCGFLLCLK]
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);

	// Set Modulo Value 20971520 / 128 = 163840
	// 163840 / 3276 = 50 Hz
	// TPM1->MOD = 3276;
	
	// Set Modulo value to 48000000 (48 Mhz) / 128 (Prescalaer) = 375000 
	// 375000 / 7500 = 50 Hz [basically when reach 7500 (max count) will flip over to 0 and start again]
	TPM1->MOD = 7500;

	// Edge-Aligned PWM
	// Update SnC register: CMOD = 01, PS = 111 (128)
	TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	// Set CMOD to 1 == LPTPM counter increments on every LPTPM counter clock
	// PS 7 is 128 prescaler
	TPM1->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
	TPM1->SC &= ~(TPM_SC_CPWMS_MASK);
	
	// Enable PWM on TPM1 Channel 0 -> PTB0
	TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM1_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
	// Enable PWM on TPM1 Channel 1 -> PTB1
	TPM1_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM1_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
	// Set C0V and C1V
	TPM1_C0V = 0x0EA6; // 0xEA6 = 3750, basically half od 7500 for 50% duty cycle
	TPM1_C1V = 0x753; // 0x0EA6
}

void setFreq(int freq) {
	int newMod = 375000 / freq;
	TPM1->MOD = newMod;
	
	// Edge-Aligned PWM
	// Update SnC register: CMOD = 01, PS = 111 (128)
	TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	// Set CMOD to 1 == LPTPM counter increments on every LPTPM counter clock
	// PS 7 is 128 prescaler
	TPM1->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
	
	TPM1_C0V = newMod/2;
	
}


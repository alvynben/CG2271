#include "MKL25Z4.h"
#include "cmsis_os2.h"

// Define pins for motors.
#define PTA1_Pin 1 // Front Left Forward
#define PTA2_Pin 2 // Front Left Backward
#define PTD0_Pin 0 // Front Right Forward
#define PTD1_Pin 1 // Front Right Backward
#define PTD2_Pin 2 // Rear Left Forward
#define PTD3_Pin 3 // Rear Left Backward
#define PTD4_Pin 4 // Rear Right Forward
#define PTD5_Pin 5 // Rear Right Backward

#define MASK(x) (1 << (x))

// Define CONSTANTS for motor movement.
#define TURN_DIAGONAL_VAL 0x00AA
#define TURN_DELAY 500
#define MOVE_DELAY 400
#define STOP_DELAY 800

//eg. LFFOR is read as left side, front wheel, forward direction while 
//RBBACK is right side, behind wheel, backward direction
typedef enum {
  LFFOR, 
  LFBACK,
  LBFOR,
  LBBACK,
  RFFOR, 
  RFBACK,
  RBFOR,
  RBBACK
} wheel_t;

// Stops all motors.
void motorStopAll() {
  TPM2_C0V = 0x0000;
  TPM2_C1V = 0x0000;
  TPM0_C0V = 0x0000;
  TPM0_C1V = 0x0000;
  TPM0_C2V = 0x0000;
  TPM0_C3V = 0x0000;
  TPM0_C4V = 0x0000;
  TPM0_C5V = 0x0000;
}

/* Initialize PWM */
void InitMotor(void) {
  
  // Need to enable port power because need to configure the registers for the MUX
  SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
  SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;

  // Configure MUX settings to make pins GPIO
  // Port A Pin 1
  PORTA->PCR[PTA1_Pin] &= ~PORT_PCR_MUX_MASK;
  // Alternative 3 (chip specific) Timer Module Channel 2
  PORTA->PCR[PTA1_Pin] |= PORT_PCR_MUX(3);
  
  // Port A Pin 2
  PORTA->PCR[PTA2_Pin] &= ~PORT_PCR_MUX_MASK;
  // Alternative 3 (chip specific) Timer Module Channel 2
  PORTA->PCR[PTA2_Pin] |= PORT_PCR_MUX(3);
  
  
  //All Port D pins are using Alternative 4
  //Port D Pin 0
  PORTD->PCR[PTD0_Pin] &= ~PORT_PCR_MUX_MASK;
  PORTD->PCR[PTD0_Pin] |= PORT_PCR_MUX(4);
  
  //Port D Pin 1
  PORTD->PCR[PTD1_Pin] &= ~PORT_PCR_MUX_MASK;
  PORTD->PCR[PTD1_Pin] |= PORT_PCR_MUX(4);

  //Port D Pin 2
  PORTD->PCR[PTD2_Pin] &= ~PORT_PCR_MUX_MASK;
  PORTD->PCR[PTD2_Pin] |= PORT_PCR_MUX(4);

  //Port D Pin 3
  PORTD->PCR[PTD3_Pin] &= ~PORT_PCR_MUX_MASK;
  PORTD->PCR[PTD3_Pin] |= PORT_PCR_MUX(4);

  //Port D Pin 4
  PORTD->PCR[PTD4_Pin] &= ~PORT_PCR_MUX_MASK;
  PORTD->PCR[PTD4_Pin] |= PORT_PCR_MUX(4);
  
  //Port D Pin 5
  PORTD->PCR[PTD5_Pin] &= ~PORT_PCR_MUX_MASK;
  PORTD->PCR[PTD5_Pin] |= PORT_PCR_MUX(4);

  // Enable Clock for Timer 2 and 0
  SIM->SCGC6 |= SIM_SCGC6_TPM2_MASK;
  SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;

  // Select Clock for TPM module
  // TPMSRC = Clk source [using MCGFLLCLK]
  SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
  SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);

  // Set Modulo value to 48000000 (48 Mhz) / 128 (Prescalaer) = 375000 
  // 375000 / 7500 = 50 Hz [basically when reach 7500 (max count) will flip over to 0 and start again]
  TPM2->MOD = 7500;
  TPM0->MOD = 7500;

  // Edge-Aligned PWM
  // Update SnC register: CMOD = 01, PS = 111 (128)
  TPM2->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
  TPM0->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
  
  // Set CMOD to 1 == LPTPM counter increments on every LPTPM counter clock
  // PS 7 is 128 prescaler
  TPM2->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7)); //LPTPM counter increments on every LPTPM counter clock
  TPM2->SC &= ~(TPM_SC_CPWMS_MASK); // up-down counting mode
  TPM0->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
  TPM0->SC &= ~(TPM_SC_CPWMS_MASK);
  
  //Edge-aligned PWM High-true pulses (clear Output on match, set Output on reload)

  TPM2_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
  TPM2_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
  TPM2_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
  TPM2_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	TPM0_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
   TPM0_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
  TPM0_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
   TPM0_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
  TPM0_C2SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
  TPM0_C2SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
  TPM0_C3SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
   TPM0_C3SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
  TPM0_C4SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
  TPM0_C4SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
  TPM0_C5SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
   TPM0_C5SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));

  motorStopAll();
}

// Function takes in a specific wheel type and direction,
// along with the channel value to be supplied to the 
// TPMx_CnV register.
void moveSpecificWheel(wheel_t type, uint16_t pwm_val) {
  switch(type) {
  case LFFOR:
    TPM2_C0V = pwm_val;
    break;
  case LFBACK:
    TPM2_C1V = pwm_val;
    break;
  case RFFOR:
    TPM0_C0V = pwm_val;
    break;
  case RFBACK:
    TPM0_C1V = pwm_val;
    break;
  case LBFOR:
    TPM0_C4V = pwm_val;
    break;
  case LBBACK:
    TPM0_C5V = pwm_val;
    break;
  case RBFOR:
    TPM0_C2V = pwm_val;
    break;
  case RBBACK:
    TPM0_C3V = pwm_val;
    break;
  }
  //0x1D4C for 100%;
}

// Moves the BOT forward.
void motorForward() {
  moveSpecificWheel(LFFOR, 0x1D4C);
  moveSpecificWheel(LFBACK, 0x0000);

  moveSpecificWheel(RFFOR, 0x1D4C);
  moveSpecificWheel(RFBACK, 0x0000);

  moveSpecificWheel(LBFOR, 0x1D4C);
  moveSpecificWheel(LBBACK, 0x0000);

  moveSpecificWheel(RBFOR, 0x1D4C);
  moveSpecificWheel(RBBACK, 0x0000);
}

// Moves the BOT forward slowly.
void motorForwardSlow() {
  moveSpecificWheel(LFFOR, 0x0F53);
  moveSpecificWheel(LFBACK, 0x0000);

  moveSpecificWheel(RFFOR, 0x0F53);
  moveSpecificWheel(RFBACK, 0x0000);

  moveSpecificWheel(LBFOR, 0x0F53);
  moveSpecificWheel(LBBACK, 0x0000);

  moveSpecificWheel(RBFOR, 0x0F53);
  moveSpecificWheel(RBBACK, 0x0000);
}

// Moves the BOT backwards.
void motorBackward() {
  moveSpecificWheel(LFFOR, 0x0000);
  moveSpecificWheel(LFBACK, 0x1D4C);

  moveSpecificWheel(RFFOR, 0x0000);
  moveSpecificWheel(RFBACK, 0x1D4C);

  moveSpecificWheel(LBFOR, 0x0000);
  moveSpecificWheel(LBBACK, 0x1D4C);

  moveSpecificWheel(RBFOR, 0x0000);
  moveSpecificWheel(RBBACK, 0x1D4C);
}

// Moves the BOT to the right.
void motorRight() {
  moveSpecificWheel(LFFOR, 0x1D4C);
  moveSpecificWheel(LFBACK, 0x0000);

  moveSpecificWheel(RFFOR, 0x0000);
  moveSpecificWheel(RFBACK, 0x1D4C);

  moveSpecificWheel(LBFOR, 0x1D4C);
  moveSpecificWheel(LBBACK, 0x0000);

  moveSpecificWheel(RBFOR, 0x0000);
  moveSpecificWheel(RBBACK, 0x1D4C);
}

// Moves the BOT to the diagonal right.
void motorDiagonalRight() {
  moveSpecificWheel(LFFOR, 0x1D4C);
  moveSpecificWheel(LFBACK, 0x0000);

  moveSpecificWheel(RFFOR, 0x0000);
  moveSpecificWheel(RFBACK, 0x0000);

  moveSpecificWheel(LBFOR, 0x1D4C);
  moveSpecificWheel(LBBACK, 0x0000);

  moveSpecificWheel(RBFOR, 0x0000);
  moveSpecificWheel(RBBACK, TURN_DIAGONAL_VAL);
}

// Moves the BOT to the left.
void motorLeft() {
  moveSpecificWheel(LFFOR, 0x0000);
  moveSpecificWheel(LFBACK, 0x1D4C);

  moveSpecificWheel(RFFOR, 0x1D4C);
  moveSpecificWheel(RFBACK, 0x0000);

  moveSpecificWheel(LBFOR, 0x0000);
  moveSpecificWheel(LBBACK, 0x1D4C);

  moveSpecificWheel(RBFOR, 0x1D4C);
  moveSpecificWheel(RBBACK, 0x0000);
}

// Moves the BOT to the diagonal left.
void motorDiagonalLeft() {
  moveSpecificWheel(LFFOR, 0x0000);
  moveSpecificWheel(LFBACK, 0x0000);

  moveSpecificWheel(RFFOR, 0x1D4C);
  moveSpecificWheel(RFBACK, 0x0000);

  moveSpecificWheel(LBFOR, 0x0000);
  moveSpecificWheel(LBBACK, TURN_DIAGONAL_VAL);

  moveSpecificWheel(RBFOR, 0x1D4C);
  moveSpecificWheel(RBBACK, 0x0000);
}

// Moves the BOT around the cone in self-driving mode.
void motorRotateCone() {
	motorStopAll();
	osDelay(STOP_DELAY);
	motorLeft();
	osDelay(TURN_DELAY / 2);
	
	// Move Forward.
	motorForward();
	osDelay(MOVE_DELAY);
		motorStopAll();
	osDelay(STOP_DELAY);
	
	// Turn 90 Degrees Right.
	motorRight();
	osDelay(TURN_DELAY);
	motorStopAll();
	osDelay(STOP_DELAY);

	// Move Forward.
	motorForward();
	osDelay(MOVE_DELAY);
		motorStopAll();
	osDelay(STOP_DELAY);
	
	// Turn 90 Degrees Right.
	motorRight();
	osDelay(TURN_DELAY);
		motorStopAll();
	osDelay(STOP_DELAY);

  // Move Forward.
	motorForward();
	osDelay(MOVE_DELAY);
		motorStopAll();
	osDelay(STOP_DELAY);
	
	// Turn 90 Degrees Right.
	motorRight();
	osDelay(TURN_DELAY);
		motorStopAll();
	osDelay(STOP_DELAY);

	// Move Forward.
	motorForward();
	osDelay(MOVE_DELAY);
		motorStopAll();
	osDelay(STOP_DELAY);
	
	// Turn 90 Degrees Right.
	motorLeft();
	osDelay(TURN_DELAY / 2);
	motorStopAll();
}

// Sets the frequency for a PWM pin.
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
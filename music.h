#include <MKL25Z4.h>
#include "RTE_Components.h"
#include "cmsis_os2.h"
#include "system_MKL25Z4.h"

// Define pins for buzzer.
#define PTB0_Pin 0
#define PTB1_Pin 1

// Define length of each song.
#define canonInD_len 122
#define checkwifi_len 4
#define victory_len 17

// Converts a given frequency to a MOD value for PWM.
#define FREQ_2_MOD(x) (375000 / x)

// Defines the delay for different types of beats.
#define FULL_BEAT 2400
#define HALF_BEAT 1200
#define THIRD_BEAT 800
#define QUARTER_BEAT 600
#define EIGHTH_BEAT 300
#define SIXTEENTH_BEAT 150
#define REST_BEAT 240
#define SKIP_BEAT 25

// Create a struct to store a note, and the duration it should run.
struct noteWithPeriod {
	int noteFreq;
	int duration;
};

// Define the name for each note in enum note and their respective frequency in musical_notes.
enum note{B0,C1,CS1,D1,DS1,E1,F1,FS1,G1,GS1,A1,AS1,B1,C2,CS2,D2,DS2,E2,F2,FS2,G2,GS2,A2,AS2,B2,C3,CS3,D3,DS3,E3,F3,FS3,G3,GS3,A3,AS3,B3,C4,CS4,D4,DS4,E4,F4,FS4,G4,GS4,A4,AS4,B4,C5,CS5,D5,DS5,E5,F5,FS5,G5,GS5,A5,AS5,B5,C6,CS6,D6,DS6,E6,F6,FS6,G6,GS6,A6,AS6,B6,C7,CS7,D7,DS7,E7,F7,FS7,G7,GS7,A7,AS7,B7,C8,CS8,D8,DS8,REST};
int musical_notes[90] = {31,33,35,37,39,41,44,46,49,52,55,58,62,65,69,73,78,82,87,93,98,104,110,117,123,131,139,147,156,165,175,185,196,208,220,233,247,262,277,294,311,330,349,370,392,415,440,466,494,523,554,587,622,659,698,740,784,831,880,932,988,1047,1109,1175,1245,1319,1397,1480,1568,1661,1760,1865,1976,2093,2217,2349,2489,2637,2794,2960,3136,3322,3520,3729,3951,4186,4435,4699,4978,0};

// Define notes for running music [CANON IN D].
enum note canon_in_d[canonInD_len] = {
	FS5,REST,E5,REST,
	D5,REST,CS5,REST,
	B4,REST,A4,REST,
	B4,REST,CS5,REST,
	FS5,REST,E5,REST,
	D5,REST,CS5,REST,
	B4,REST,A4,REST,
	B4,REST,CS5,REST,
	D5,REST,CS5,REST,
	B4,REST,A4,REST,
	G4,REST,FS4,REST,
	G4,REST,A4,REST,
	
	D5,REST,FS5,REST,
	G5,REST,A5,REST,
	FS5,REST,G5,REST,
  A5,REST,B4,REST,
	CS5,REST,D5,REST,
	E5,REST,FS5,REST,
	G5,REST,FS5,REST,
	D5,REST,E5,REST,
	FS5,FS4,REST,
	G4,REST,A4,REST,
	G4,REST,FS4,REST,
	G4,REST,A4,REST,
  G4,REST,B4,REST,
	A4,REST,G4,REST,
	FS4,REST,E4,REST, 
  FS4,REST,D4,REST,
	E4,REST,FS4,REST,
	G4,REST,A4,REST,
	B4,REST
};

// Define periods for notes for running music [CANON IN D].
int canon_in_d_period[canonInD_len] = {
	HALF_BEAT,REST_BEAT,HALF_BEAT,REST_BEAT,
	HALF_BEAT,REST_BEAT,HALF_BEAT,REST_BEAT,
	HALF_BEAT,REST_BEAT,HALF_BEAT,REST_BEAT,
	HALF_BEAT,REST_BEAT,HALF_BEAT,REST_BEAT,
	HALF_BEAT,REST_BEAT,HALF_BEAT,REST_BEAT,
	HALF_BEAT,REST_BEAT,HALF_BEAT,REST_BEAT,
	HALF_BEAT,REST_BEAT,HALF_BEAT,REST_BEAT,
	HALF_BEAT,REST_BEAT,HALF_BEAT,REST_BEAT,
	HALF_BEAT,REST_BEAT,HALF_BEAT,REST_BEAT,
	HALF_BEAT,REST_BEAT,HALF_BEAT,REST_BEAT,
	HALF_BEAT,REST_BEAT,HALF_BEAT,REST_BEAT,
	HALF_BEAT,REST_BEAT,HALF_BEAT,REST_BEAT,
	
	QUARTER_BEAT,SKIP_BEAT,EIGHTH_BEAT,SKIP_BEAT,
	EIGHTH_BEAT,SKIP_BEAT,QUARTER_BEAT,SKIP_BEAT,
	EIGHTH_BEAT,SKIP_BEAT,EIGHTH_BEAT,SKIP_BEAT,
	QUARTER_BEAT,SKIP_BEAT,EIGHTH_BEAT,SKIP_BEAT,
	EIGHTH_BEAT,SKIP_BEAT,EIGHTH_BEAT,SKIP_BEAT,
	EIGHTH_BEAT,SKIP_BEAT,EIGHTH_BEAT,SKIP_BEAT,
	EIGHTH_BEAT,SKIP_BEAT,QUARTER_BEAT,SKIP_BEAT,
	EIGHTH_BEAT,SKIP_BEAT,EIGHTH_BEAT,SKIP_BEAT,
	QUARTER_BEAT,SKIP_BEAT,EIGHTH_BEAT,SKIP_BEAT,
	EIGHTH_BEAT,SKIP_BEAT,EIGHTH_BEAT,SKIP_BEAT,
	EIGHTH_BEAT,SKIP_BEAT,EIGHTH_BEAT,SKIP_BEAT,
	EIGHTH_BEAT,SKIP_BEAT,HALF_BEAT,SKIP_BEAT,
	QUARTER_BEAT,SKIP_BEAT,EIGHTH_BEAT,SKIP_BEAT,
	EIGHTH_BEAT,SKIP_BEAT,EIGHTH_BEAT,SKIP_BEAT,
	QUARTER_BEAT,SKIP_BEAT,EIGHTH_BEAT,SKIP_BEAT,
	EIGHTH_BEAT,SKIP_BEAT,EIGHTH_BEAT,SKIP_BEAT,
	EIGHTH_BEAT,SKIP_BEAT,EIGHTH_BEAT,SKIP_BEAT,
	EIGHTH_BEAT,SKIP_BEAT
};

// Define notes for check WIFI tune.
enum note check_wifi[checkwifi_len] = {G5,A5,B5,REST};

// Define periods for notes for check WIFI tune.
int check_wifi_period[checkwifi_len] = {EIGHTH_BEAT,EIGHTH_BEAT,EIGHTH_BEAT,REST_BEAT};

// Define notes for victory music [Final Fantasy Fanfare].
enum note victory[victory_len] = {A5,REST,A5,REST,A5,REST,A5,REST,F5,REST,G5,REST,A5,REST,G5,REST,A5};

// Define periods for notes for victory music [Final Fantasy Fanfare].
int victory_period[victory_len] = {SIXTEENTH_BEAT,SKIP_BEAT,SIXTEENTH_BEAT,SKIP_BEAT,SIXTEENTH_BEAT,SKIP_BEAT,EIGHTH_BEAT,REST_BEAT,EIGHTH_BEAT,REST_BEAT,QUARTER_BEAT,SKIP_BEAT,EIGHTH_BEAT,SKIP_BEAT,EIGHTH_BEAT,SKIP_BEAT,QUARTER_BEAT};

/* 
		The section below stores all the above notes and periods
		in an array of structs. One array is created for each song.
*/
struct noteWithPeriod canonInD[canonInD_len];
struct noteWithPeriod checkWifi[checkwifi_len];
struct noteWithPeriod Victory[victory_len];

void InitSongs(void) {
  // Initialise Canon In D struct array.
	for (int i = 0; i < canonInD_len; i++) {
		canonInD[i].noteFreq = musical_notes[canon_in_d[i]];
		canonInD[i].duration = canon_in_d_period[i];
	}
	
  // Initialise Check Wifi struct array.
	for (int i = 0; i < checkwifi_len; i++) {
		checkWifi[i].noteFreq = musical_notes[check_wifi[i]];
		checkWifi[i].duration = check_wifi_period[i];
	}
	
  // Initialise Victory Music struct array.
	for (int i = 0; i < victory_len; i++) {
		Victory[i].noteFreq = musical_notes[victory[i]];
		Victory[i].duration = victory_period[i];
	}
}

// Initialises the PWM used to control the buzzer.
void InitPWM(void) {
	// Enable Clock in PORTB
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	
	// Clearing PCR and Setting to MUX Alt3 -- TPM1_CH0 (pg 163)
	PORTB->PCR[PTB0_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[PTB0_Pin] |= PORT_PCR_MUX(3);
	
	// Enable TPM1 Clock
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
	
	// Clearing SOPT2 Register and Setting TPMSRC to Clock 1 -- MCGFLLCLK clock or MCGPLLCLK/2 (pg 196)
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
	
	// Setting MOD for TPM1 -- (Clock Speed/PS) / MOD = Desired Freq in Hz  (pg 554)
	TPM1->MOD = TPM_MOD_MOD(7500); 
	
	//Set Duty Cycle
//	TPM1_C0V = TPM_CnV_VAL(3750);
	
	/* Config for TPM1 Status and Control (SC) (pg 553) */

	// Clear CMOD and PS 
	TPM1->SC &= ~(TPM_SC_CMOD_MASK | TPM_SC_PS_MASK);
	// Set Clock Mode to 1 -- Increment every counter clock
	// Set Prescale Factor to Divide by 128
	TPM1->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
	// Set PWM to Up Counting Mode
	TPM1->SC &= ~TPM_SC_CPWMS_MASK;
	
	/* Config for TPM1_CH0 Status and Control (SC) (pg 555) */
	
	// Clear TPM1 Channel 0 SC Register
	TPM1_C0SC &= ~(TPM_CnSC_ELSA_MASK | TPM_CnSC_ELSB_MASK | TPM_CnSC_MSB_MASK);
	
//	// Set SC to MSnB:MSnA == 10  ELSnB:ELSnA == 10 --> High True pulses PWM
  TPM1_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
}

// Plays a note from the buzzer
// Choose the note to play.
// Choose the duration the note should play for.
void playNote(int note, int duration) {
	TPM1->MOD = FREQ_2_MOD(note);
	TPM1_C0V = (FREQ_2_MOD(note)) / 2;
	osDelay(duration);
}

// Turn off all PWM to the buzzer.
void offPwmFreq() {
  TPM1_C0SC &= (TPM_CnSC_ELSB(0) | TPM_CnSC_MSB(0));
}

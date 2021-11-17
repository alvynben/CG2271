#include "MKL25Z4.h"
#include "cmsis_os2.h"

// Define UART2 Pin Values.
#define BAUD_RATE 9600
#define PTE22_UART2_TX 22
#define PTE23_UART2_RX 23
#define UART2_INT_PRIO 128

// Initialise Message Queue to store rx_data.
osMessageQueueId_t mqid_rx;
#define MQ_SIZE 10
uint8_t rx_data;

// Initialise registers for UART2.
void InitUART2(uint32_t baud_rate) {
	uint32_t divisor, bus_clock;
	
	// Activate clocks to UART2 and PORTE
	SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	
	// Switch Port E Pin 22 to Alt 4 - "UART2_TX"
	PORTE->PCR[PTE22_UART2_TX] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[PTE22_UART2_TX] |= PORT_PCR_MUX(4);
	
	// Switch Port E Pin 23 to Alt 4 - "UART2_RX"
	PORTE->PCR[PTE23_UART2_RX] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[PTE23_UART2_RX] |= PORT_PCR_MUX(4);
	
	// Disable UART TX & RX so we can edit them
	UART2->C2 &= ~UART_C2_RE_MASK;
	
	// SBR[12:0] = bus_clock / (baud_rate * 16)
	// Bus clock: CPU clock / 2
	// BDH is SBR [12:9]
	// BDL is SBD [8:0]
	bus_clock = (DEFAULT_SYSTEM_CLOCK) / 2;
	divisor = (bus_clock) / (baud_rate * 16);
	UART2->BDH = UART_BDH_SBR(divisor >> 8);
	UART2->BDL = UART_BDL_SBR(divisor);
	
	UART2->C1 = 0;
	UART2->S2 = 0;
	UART2->C3 = 0;
	
	// Enable UART TX & RX
	UART2->C2 |= UART_C2_RE_MASK;
	
	// Set up interrupts
	NVIC_SetPriority(UART2_IRQn, UART2_INT_PRIO);
	NVIC_ClearPendingIRQ(UART2_IRQn);
	NVIC_EnableIRQ(UART2_IRQn);
	
	// Enable TX & RX Interrupts
	UART2->C2 |= UART_C2_RIE_MASK;
}

// IRQHandler activates whenever user sends
// an input from the app.
void UART2_IRQHandler() {
	NVIC_ClearPendingIRQ(UART2_IRQn);
	
	if (UART2->S1 & UART_S1_RDRF_MASK) {
		// If input received, read value and
		// store in MessageQueue for tBrain to receive.
		rx_data = UART2->D;
		osMessageQueuePut(mqid_rx, &rx_data, 0, 0);
	}
	PORTE->ISFR = 0xffffffff;
}

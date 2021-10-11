 #include "MKL25Z4.h"
typedef enum {RED, GREEN, BLUE} colour_t;
typedef enum {LED_OFF, LED_ON} led_status;

#define RED_LED 18 // PortB Pin 18
#define GREEN_LED 19 // PortB Pin 19
#define BLUE_LED 1 // PortD Pin 1
#define MASK(x) (1 << (x))

volatile int count = 0;

static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
  }
}

void InitGPIO(void)
{
// Enable Clock to PORTB and PORTD
SIM->SCGC5 |= ((SIM_SCGC5_PORTB_MASK) | (SIM_SCGC5_PORTD_MASK));
// Configure MUX settings to make all 3 pins GPIO
PORTB->PCR[RED_LED] &= ~PORT_PCR_MUX_MASK;
PORTB->PCR[RED_LED] |= PORT_PCR_MUX(1);
PORTB->PCR[GREEN_LED] &= ~PORT_PCR_MUX_MASK;
PORTB->PCR[GREEN_LED] |= PORT_PCR_MUX(1);
PORTD->PCR[BLUE_LED] &= ~PORT_PCR_MUX_MASK;
PORTD->PCR[BLUE_LED] |= PORT_PCR_MUX(1);
// Make RED_LED, GREEN_LED, BLUE_LED to be output
PTB->PDDR |= (MASK(RED_LED) | MASK(GREEN_LED));
PTD->PDDR |= MASK(BLUE_LED);
}

void stopAllRGB() {
	PTB->PSOR = MASK(RED_LED) | MASK(GREEN_LED);
	PTD->PSOR = MASK(BLUE_LED);
}

void led_control(colour_t colour, led_status status) {
    switch(colour) {
		case RED:
			if (status) {
				PTB->PCOR |= MASK(RED_LED);
			} else {
				PTB->PSOR = MASK(RED_LED);
			}
			break;
		case GREEN:
			if (status) {
				PTB->PCOR |= MASK(GREEN_LED);
			} else {
				PTB->PSOR = MASK(GREEN_LED);
			}
			break;
		case BLUE:
			if (status) {
				PTB->PCOR |= MASK(BLUE_LED);
			} else {
				PTB->PSOR = MASK(BLUE_LED);
			}
			break;
		}
}

void led_red_thread (void *argument) {
 
  // ...
  for (;;) {
		led_control(RED, LED_ON);
		delay(0x80000);
		led_control(RED, LED_OFF);
		delay(0x80000);
		count++;
	}
}

void led_green_thread (void *argument) {
 
  // ...
  for (;;) {
		led_control(GREEN,LED_ON);
		delay(0x80000);
		led_control(GREEN,LED_OFF);
		delay(0x80000);
		count++;
	}
}

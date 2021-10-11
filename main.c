/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
 
/*----------------------------------------------------------------------------
* Application main thread
*---------------------------------------------------------------------------*/

#include "MKL25Z4.h"
#include "led.h"
#include "uart2.h"
#include "motor.h"
 
osThreadId_t red_led_on, red_led_off, green_led_on, green_led_off, blue_led_on, blue_led_off;

osThreadId_t motor_forward, motor_backward, motor_stop;

void tBrain(void *argument) {
	uint8_t rx_data;
	
	for(;;) {
		osMessageQueueGet(mqid_rx,&rx_data,NULL,osWaitForever);
		
		switch(rx_data) {
			case(0x31):
				osThreadFlagsSet(red_led_on,0x0001);
				break;
			case(0x30):
				osThreadFlagsSet(red_led_off,0x0001);
				break;
			case(0x33):
				osThreadFlagsSet(green_led_on,0x0001);
				break;
			case(0x32):
				osThreadFlagsSet(green_led_off,0x0001);
				break;
			case(0x35):
				osThreadFlagsSet(blue_led_on,0x0001);
				break;
			case(0x34):
				osThreadFlagsSet(blue_led_off,0x0001);
				break;
			case(0x36):
				osThreadFlagsSet(motor_forward,0x0001);
				break;
			case(0x37):
				osThreadFlagsSet(motor_backward,0x0001);
				break;
			default:
				motorStop();
				break;
				
		}
	}
}

void tRedLedOn(void *argument) {
	for (;;) {
		osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
		
		led_control(RED,LED_ON);
	}
}

void tRedLedOff(void *argument) {
	for (;;) {
		osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
		
		led_control(RED,LED_OFF);
	}
}

void tGreenLedOn(void *argument) {
	for (;;) {
		osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
		
		led_control(GREEN,LED_ON);
	}
}

void tGreenLedOff(void *argument) {
	for (;;) {
		osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
		
		led_control(GREEN,LED_OFF);
	}
}

void tBlueLedOn(void *argument) {
	for (;;) {
		osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
		
		led_control(BLUE,LED_ON);
	}
}

void tBlueLedOff(void *argument) {
	for (;;) {
		osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
		
		led_control(BLUE,LED_OFF);
	}
}

void tMotorForward(void *argument) {
	for(;;) {
		osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
		
		motorForward();
	}
}

void tMotorBackward(void *argument) {
	for(;;) {
		osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
		
		motorBackward();
	}
}

void tMotorStop(void *argument) {
	for(;;) {
		osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
		
		motorStop();
	}
}

 
int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
	InitUART2(BAUD_RATE);
	InitGPIO();
	InitMotor();
	stopAllRGB();
  // ...
 
  osKernelInitialize();                 // Initialize CMSIS-RTOS
	//--------------------//
	
	// Create Message Queue to store UART inputs
	mqid_rx = osMessageQueueNew(MQ_SIZE, 1, NULL);
	
	// Create tBrain thread to manage all inputs
	osThreadNew(tBrain, NULL, NULL);
	
	// Create threads to run LEDs
	red_led_on = osThreadNew(tRedLedOn,NULL,NULL);
	red_led_off = osThreadNew(tRedLedOff, NULL, NULL);
	green_led_on = osThreadNew(tGreenLedOn,NULL,NULL);
	green_led_off = osThreadNew(tGreenLedOff, NULL, NULL);
	blue_led_on = osThreadNew(tBlueLedOn,NULL,NULL);
	blue_led_off = osThreadNew(tBlueLedOff, NULL, NULL);
	
	// Create threads to run Motors
	motor_forward = osThreadNew(tMotorForward,NULL,NULL);
	motor_backward = osThreadNew(tMotorBackward,NULL,NULL);
	motor_stop = osThreadNew(tMotorStop, NULL, NULL);
	
	//-------------------//
	
  osKernelStart();                      // Start thread execution
  for (;;) {}
}

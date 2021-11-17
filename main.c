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
#include "runningLed.h"
#include "ledBlink.h"
#include "uart2.h"
#include "motor.h"
#include "music.h"
#include "ultrasonic.h"


// Enum to denote if BOT is MOVING/STOPPED.
typedef enum {
	MOVING,
	STOPPED
} robotState;

// Enum to denote the music BOT should play.
typedef enum {
	RUNNING_MUSIC,
	VICTORY_MUSIC,
	CHECK_WIFI_MUSIC,
	STOP_MUSIC
	} musicState;

// Initialise BOT as STOPPED.
volatile robotState MOVEMENT_STATE = STOPPED;
volatile musicState MUSIC_STATE = STOP_MUSIC;

// Initialise Thread IDs.
osThreadId_t motor_forward, motor_backward, motor_left, motor_right, motor_diag_right, motor_diag_left;
osThreadId_t check_wifi_led;
osThreadId_t running_led_green, still_led_green, led_red_500, led_red_250;
osThreadId_t play_victory_song, play_running_song, play_checkWifi_song;

// Initialise Semaphore IDs.
osSemaphoreId_t green_led_semaphore;
osSemaphoreId_t red_led_semaphore;
osSemaphoreId_t buzzer_semaphore;

// Create high priority for 10us pulse for Ultrasonic Sensor.
const osThreadAttr_t ultrasonic_trigger_attr = {
	.priority = osPriorityHigh
};

// Create high priority for tBrain
const osThreadAttr_t thread1_attr_high = {
	.priority = osPriorityHigh   
};

/*
	This IRQ Handler handles inputs from the HC-SR04
	Ultrasonic Sensor. 

	It only works in stage 2 or 4,
	because only stage 2 & 4 require the bot to stop
	based on input from the Ultrasonic Sensor.
*/
void PORTD_IRQHandler(void) {
	NVIC_ClearPendingIRQ(PORTD_IRQn);
	// Checks if BOT is in stage 2 or 4
	if (stage == 2 || stage == 4) {
		// If hasStarted == 0, the IRQHandler is detecting a rising edge.
		// Else, the IRQ Handler is detecting a falling edge.
		if (hasStarted == 0) {
			hasStarted = 1;
			timerStart = osKernelGetSysTimerCount();
		} else if (hasStarted == 1) {
			timerEnd = osKernelGetSysTimerCount();
			uint32_t timeDiff = timerEnd - timerStart;
			
			/* 
				If the time between rising edge and falling edge
				is below a threshold, the BOT is close to an
				obstacle.
			*/
			if (timeDiff <= threshold) {
				// If close to an obstacle, proceed to the next
				// stage of operation.
				stage++; 
			}
			hasStarted = 0;
		}
	}
	PORTD->ISFR = 0xffffffff;
}

void ultrasonic_trigger_thread (void *argument) {
	for (;;) {
		// Sends a 1ms pulse to the HC-SR04 to trigger it.
		// Unfortunately, 1us pulse doesn't seem to activate it.
		PTA->PDOR |= MASK(TRIGGER);
		osDelay(1);
		PTA->PDOR &= ~MASK(TRIGGER);
		
		// Wait 60ms between each pulse
		osDelay(60);
	}
}


void autonomous_thread (void *argument) {
	for (;;) {
		switch (stage) {
			
			// Stage 1 stops the BOT
			case 1:
				InitMotor();
				motorStopAll();

				stage = 2;
				break;
			
			// Stage 2 moves the BOT Forward.
			// PORTD_IRQHandler is activated to check for obstacles.
			case 2:
				MOVEMENT_STATE = MOVING;
				MUSIC_STATE = RUNNING_MUSIC;
				motorForward();
			break;
			
			// Stage 3 stops the BOT, and rotates it around the cone.
			case 3:	
				InitMotor();
				
				// Stops the BOT
				MOVEMENT_STATE = STOPPED;
				MUSIC_STATE = STOP_MUSIC;
				
			  moveSpecificWheel(LFFOR, 0x0001);
				moveSpecificWheel(LFBACK, 0x0001);
 
				moveSpecificWheel(RFFOR, 0x0001);
				moveSpecificWheel(RFBACK, 0x0001);

				moveSpecificWheel(LBFOR, 0x0001);
				moveSpecificWheel(LBBACK, 0x0001);

				moveSpecificWheel(RBFOR, 0x0001);
				moveSpecificWheel(RBBACK, 0x0001);
			
				
				// Enable the BOT to move around the cone.
				MOVEMENT_STATE = MOVING;
				MUSIC_STATE = RUNNING_MUSIC;
				motorRotateCone();
				
				// Move the BOT to the next stage. 
				stage = 4;
				break;
			
			// Move the BOT back to the stop block.
			case 4:
				// Enable the BOT to move forward.
				motorForward();
				break;
			
			// Stop the BOT once it reaches the stop block.
			// Make the BOT play victory music.
			case 5:
				InitMotor();
				motorStopAll();
				MOVEMENT_STATE = STOPPED;
				MUSIC_STATE = VICTORY_MUSIC;
			
			// Do nothing if self-driving has not been activated.
			default:
				break;
				
		}
	}
}


void tBrain(void *argument) {
	
	for(;;) {
		
		// Wait for update in rx_data from UART2_IRQHandler.
		osMessageQueueGet(mqid_rx,&rx_data,NULL,osWaitForever);
		switch(rx_data) {
			case(0x35): // Activate SELF-DRIVING.
				MOVEMENT_STATE = MOVING;
				MUSIC_STATE = RUNNING_MUSIC;
				stage = 1;
			break;
			case(0x36): // Move FORWARD.
				MOVEMENT_STATE = MOVING;
				MUSIC_STATE = RUNNING_MUSIC;
				osThreadFlagsSet(motor_forward,0x0001);
				break;
			case(0x37): // Move BACKWARDS.
				MOVEMENT_STATE = MOVING;
				MUSIC_STATE = RUNNING_MUSIC;
				osThreadFlagsSet(motor_backward,0x0001);
				break;
			case(0x39): // Move LEFT.
				MOVEMENT_STATE = MOVING;
				MUSIC_STATE = RUNNING_MUSIC;
				osThreadFlagsSet(motor_left,0x0001);
				break;
			case(0x40): // Move RIGHT.
				MOVEMENT_STATE = MOVING;
				MUSIC_STATE = RUNNING_MUSIC;
				osThreadFlagsSet(motor_right,0x0001);
				break;
			case(0x41): // Check WIFI.
				MUSIC_STATE = CHECK_WIFI_MUSIC;
				osThreadFlagsSet(check_wifi_led,0x0001);
				break;
			case(0x42): // Play VICTORY_MUSIC.
				MUSIC_STATE = VICTORY_MUSIC;
				break;
			case(0x43): // Move DIAGONAL LEFT.
				MOVEMENT_STATE = MOVING;
				MUSIC_STATE = RUNNING_MUSIC;	
				osThreadFlagsSet(motor_diag_left,0x0001);	
				break;
			case(0x44): // Move DIAGONAL RIGHT.
				MOVEMENT_STATE = MOVING;
				MUSIC_STATE = RUNNING_MUSIC;
				osThreadFlagsSet(motor_diag_right,0x0001);
				break;
			default: // Stop BOT.
				MOVEMENT_STATE = STOPPED;
				MUSIC_STATE = STOP_MUSIC;
				motorStopAll();
				break;				
		}
		osDelay(100); // Delay tBrain after each user command.
	}
}

// Moves BOT forward.
void tMotorForward(void *argument) {
	for(;;) {
		osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
		
		motorForward();
	}
}

// Moves BOT backward.
void tMotorBackward(void *argument) {
	for(;;) {
		osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
		
		motorBackward();
	}
}

// Moves BOT to the right.
void tMotorRight(void *argument) {
	for(;;) {
		osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
		
		motorRight();
	}
}

// Moves BOT to the left.
void tMotorLeft(void *argument) {
	for(;;) {
		osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
		
		motorLeft();
	}
}

// Moves BOT to diagonal left.
void tMotorDiagLeft(void *argument) {
	for(;;) {
		osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
		motorDiagonalLeft();
	}
}

// Moves BOT to diagonal right.
void tMotorDiagRight(void *argument) {
	for(;;) {
		osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
		motorDiagonalRight();
	}
}

// Handles user checking WIFI.
void tCheckWifiLed(void *argument) {
	for(;;) {
		osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
		osSemaphoreAcquire(green_led_semaphore, osWaitForever);
		greenLedControlAll(ON);
		osDelay(200);
		greenLedControlAll(OFF);
		osDelay(200);
		greenLedControlAll(ON);
		osDelay(200);
		greenLedControlAll(OFF);
		osDelay(200);
		osSemaphoreRelease(green_led_semaphore);
	}
}

// Handles the green LEDs when BOT is RUNNING.
// The Green LEDs will be turn on and off one by one.
void tRunningLedGreen (void *argument) {
  for (;;) {
			if (MOVEMENT_STATE == MOVING) {
				osSemaphoreAcquire(green_led_semaphore, osWaitForever);
				greenLedControlAll(OFF);
				switch(ledNum) {
					case 1:
						greenLedControlSingle(ON, 1);
						osDelay(100);
						greenLedControlSingle(OFF, 1);
						ledNum++;
						osSemaphoreRelease(green_led_semaphore);
					case 2:
						greenLedControlSingle(ON, 2);
						osDelay(100);
						greenLedControlSingle(OFF, 2);
						ledNum++;
						osSemaphoreRelease(green_led_semaphore);
					case 3:
						greenLedControlSingle(ON, 3);
						osDelay(100);
						greenLedControlSingle(OFF, 3);
						ledNum++;
						osSemaphoreRelease(green_led_semaphore);
					case 4:
						greenLedControlSingle(ON, 4);
						osDelay(100);
						greenLedControlSingle(OFF, 4);
						ledNum++;
						osSemaphoreRelease(green_led_semaphore);
					case 5:
						greenLedControlSingle(ON, 5);
						osDelay(100);
						greenLedControlSingle(OFF, 5);
						ledNum++;
						osSemaphoreRelease(green_led_semaphore);
					case 6:
						greenLedControlSingle(ON, 6);
						osDelay(100);
						greenLedControlSingle(OFF, 6);
						ledNum++;
						osSemaphoreRelease(green_led_semaphore);
					case 7:
						greenLedControlSingle(ON, 7);
						osDelay(100);
						greenLedControlSingle(OFF, 7);
						ledNum++;
						osSemaphoreRelease(green_led_semaphore);
					case 8:
						greenLedControlSingle(ON, 8);
						osDelay(100);
						greenLedControlSingle(OFF, 8);
						ledNum++;
						osSemaphoreRelease(green_led_semaphore);
					case 9:
						greenLedControlSingle(ON, 9);
						osDelay(100);
						greenLedControlSingle(OFF, 9);
						ledNum++;
						osSemaphoreRelease(green_led_semaphore);
					case 10:
						greenLedControlSingle(ON, 10);
						osDelay(100);
						greenLedControlSingle(OFF, 10);
						ledNum = 1;
						osSemaphoreRelease(green_led_semaphore);
				}
			osSemaphoreRelease(green_led_semaphore);
			}
	}
}

// Handles the green LEDs when BOT is STOPPED.
// The Green LEDs will all stay on.
void tGreenLedStill(void *argument) {
	for(;;) {
		if (MOVEMENT_STATE == STOPPED && MUSIC_STATE == STOP_MUSIC) {
			osSemaphoreAcquire(green_led_semaphore, osWaitForever);
			greenLedControlAll(ON);
			osSemaphoreRelease(green_led_semaphore);
		} else if (MOVEMENT_STATE == STOPPED && MUSIC_STATE == CHECK_WIFI_MUSIC) {
			osSemaphoreAcquire(green_led_semaphore, osWaitForever);
			greenLedControlAll(ON);
			osDelay(200);
			greenLedControlAll(OFF);
			osDelay(200);
			greenLedControlAll(ON);
			osDelay(200);
			greenLedControlAll(OFF);
			osDelay(200);
			osSemaphoreRelease(green_led_semaphore);
			MUSIC_STATE = STOP_MUSIC;
		}
	}
}

// Handles the red LEDs when BOT is MOVING.
// Flashes the red LEDs every 500ms.
void tRedLed500 (void *argument) {
  for (;;) {
		if (MOVEMENT_STATE == MOVING) {
			osSemaphoreAcquire(red_led_semaphore, osWaitForever);
			led_control(true);
			osDelay(500); 
			led_control(false);
			osDelay(500);
			osSemaphoreRelease(red_led_semaphore);
		}
	}
}

// Handles the red LEDs when BOT is STOPPED.
// Flashes the red LEDs every 250ms.
void tRedLed250 (void *argument) {
  for (;;) {
		if (MOVEMENT_STATE == STOPPED) {
			osSemaphoreAcquire(red_led_semaphore, osWaitForever);
			led_control(true);
			osDelay(250); 
			led_control(false);
			osDelay(250);
			osSemaphoreRelease(red_led_semaphore);
		}
	}
}

// Make the BOT play the correct song based on MUSIC_STATE.
void tPlaySong (void *argument) {
	for (;;) {
		switch (MUSIC_STATE) {
			case STOP_MUSIC:
				playNote(0,0);
				break;
			
			case RUNNING_MUSIC:
				for (int i = 0; i < canonInD_len; i++) {
					if (MUSIC_STATE != RUNNING_MUSIC) {
						break;
					}
					playNote(canonInD[i].noteFreq, canonInD[i].duration);
				}
				break;
				
			case CHECK_WIFI_MUSIC:
				for (int i = 0; i < checkwifi_len; i++) {
					if (MUSIC_STATE != CHECK_WIFI_MUSIC) {
						break;
					}
					playNote(checkWifi[i].noteFreq, checkWifi[i].duration);
				}
				MUSIC_STATE = STOP_MUSIC;
				break;
				
			case VICTORY_MUSIC:
				for (int i = 0; i < victory_len; i++) {
					if (MUSIC_STATE != VICTORY_MUSIC) {
						break;
					}
					playNote(Victory[i].noteFreq, Victory[i].duration);
				}
				MUSIC_STATE = STOP_MUSIC;
				break;
		}
	}
}


int main (void) {
 
  // System Initialization.
  SystemCoreClockUpdate();
	InitUART2(BAUD_RATE);
	InitPWM();
	InitSongs();
	InitMotor();
	InitGreenLED();
	InitGPIOLedBlink();
	initUltraSonic();
	
  osKernelInitialize();                 // Initialize CMSIS-RTOS.
	//--------------------//
	
	// Create Semaphores.
	green_led_semaphore = osSemaphoreNew(1,1,NULL);
	red_led_semaphore = osSemaphoreNew(1,1,NULL);
	buzzer_semaphore = osSemaphoreNew(1,1,NULL);

	// Create Message Queue to store UART inputs.
	mqid_rx = osMessageQueueNew(MQ_SIZE, 2, NULL);
	
	// Create thread to manage all UART inputs.
	osThreadNew(tBrain, NULL, &thread1_attr_high);
	
	// Create threads to control green and red LEDs.
	running_led_green = osThreadNew(tRunningLedGreen, NULL, NULL);
	still_led_green = osThreadNew(tGreenLedStill, NULL, NULL);
	led_red_500 = osThreadNew(tRedLed500, NULL, NULL);
	led_red_250 = osThreadNew(tRedLed250, NULL, NULL);
	
	// Create thread to control PWM to play music.
	osThreadNew(tPlaySong, NULL, NULL);
	
	// Create threads to control motors.
	motor_forward = osThreadNew(tMotorForward,NULL,NULL);
	motor_backward = osThreadNew(tMotorBackward,NULL,NULL);
	motor_left = osThreadNew(tMotorLeft,NULL,NULL);
	motor_right = osThreadNew(tMotorRight,NULL,NULL);
	motor_diag_left = osThreadNew(tMotorDiagLeft, NULL, NULL);
	motor_diag_right = osThreadNew(tMotorDiagRight, NULL, NULL);
	
	// Create threads for SELF DRIVING MODE.
	
	// This thread sends a trigger pulse to HC-SR04 every 60ms.
	osThreadNew(ultrasonic_trigger_thread, NULL, &ultrasonic_trigger_attr);
	
	// This thread handles the self-driving by controlling the BOT directly,
	// when self-driving is activated.
	osThreadNew(autonomous_thread, NULL, NULL); 
	
	//-------------------//
	
  osKernelStart();                      // Start thread execution.
  for (;;) {}
}

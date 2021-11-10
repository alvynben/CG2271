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

typedef enum {
	MOVING,
	STOPPED
} robotState;

typedef enum {
	RUNNING_MUSIC,
	VICTORY_MUSIC,
	CHECK_WIFI_MUSIC,
	STOP_MUSIC
	} musicState;

	
volatile robotState MOVEMENT_STATE = STOPPED;
volatile musicState MUSIC_STATE = STOP_MUSIC;
//volatile selfDrivingMode = 
 
	const osThreadAttr_t thread1_attr_high = {
	.priority = osPriorityHigh                    //Set initial thread priority to high   
};

osThreadId_t motor_forward, motor_backward, motor_left, motor_right, motor_diag_right, motor_diag_left;

osThreadId_t self_driving_mode;

osThreadId_t check_wifi_led;
	
osThreadId_t running_led_green, still_led_green, led_red_500, led_red_250;
	
osThreadId_t play_victory_song, play_running_song, play_checkWifi_song;
	
osSemaphoreId_t green_led_semaphore;

osSemaphoreId_t red_led_semaphore;

osSemaphoreId_t buzzer_semaphore;


void tBrain(void *argument) {
	//uint8_t rx_data;
	
	for(;;) {
				test++;
		osMessageQueueGet(mqid_rx,&rx_data,NULL,osWaitForever);
		switch(rx_data) {
			case(0x35):
				MOVEMENT_STATE = MOVING;
				MUSIC_STATE = RUNNING_MUSIC;
				osThreadFlagsSet(self_driving_mode, 0x0001);
			break;
			case(0x36):
				MOVEMENT_STATE = MOVING;
				MUSIC_STATE = RUNNING_MUSIC;
				osThreadFlagsSet(motor_forward,0x0001);
				break;
			case(0x37):
				MOVEMENT_STATE = MOVING;
				MUSIC_STATE = RUNNING_MUSIC;
				osThreadFlagsSet(motor_backward,0x0001);
				break;
//			case(0x38):
//				osThreadFlagsSet(motor_stop,0x0001);
//				break;
			case(0x39):
				MOVEMENT_STATE = MOVING;
				MUSIC_STATE = RUNNING_MUSIC;
				osThreadFlagsSet(motor_left,0x0001);
				break;
			case(0x40): 
				MOVEMENT_STATE = MOVING;
				MUSIC_STATE = RUNNING_MUSIC;
				osThreadFlagsSet(motor_right,0x0001);
				break;
			case(0x41):
				MUSIC_STATE = CHECK_WIFI_MUSIC;
				osThreadFlagsSet(check_wifi_led,0x0001);
//				osThreadFlagsSet(play_checkWifi_song,0x0001);
				break;
			case(0x42):
				MUSIC_STATE = VICTORY_MUSIC;

//				osThreadFlagsSet(play_victory_song,0x0001);
				break;
			case(0x43):
				MOVEMENT_STATE = MOVING;
				MUSIC_STATE = RUNNING_MUSIC;	
				osThreadFlagsSet(motor_diag_left,0x0001);	
				break;
			case(0x44):
				MOVEMENT_STATE = MOVING;
				MUSIC_STATE = RUNNING_MUSIC;
				osThreadFlagsSet(motor_diag_right,0x0001);
				break;
			default:
				MOVEMENT_STATE = STOPPED;
				MUSIC_STATE = STOP_MUSIC;
				motorStopAll();
				blocked = 1;
				break;				
		}
		osDelay(50);
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

void tMotorRight(void *argument) {
	for(;;) {
		osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
		
		motorRight();
	}
}

void tMotorLeft(void *argument) {
	for(;;) {
		osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
		
		motorLeft();
	}
}

void tMotorDiagLeft(void *argument) {
	for(;;) {
		osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
		motorDiagonalLeft();
	}
}

void tMotorDiagRight(void *argument) {
	for(;;) {
		osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
		motorDiagonalRight();
	}
}

void tSelfDrivingMode(void *argument) {
//	for (;;) {
//		osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
//		while (blocked == 0) {
//			motorForward();
//			osDelay(400);
//		}
//		//motorRotateCone();
//		while (blocked == 1) {
//			motorRotateCone();
//		}
//		//selfDrivingMode();
//		MUSIC_STATE = STOP_MUSIC;
//		MOVEMENT_STATE = STOPPED;
//	}
}

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

void tRunningLedGreen (void *argument) {
  // ...
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
 
  // System Initialization
  SystemCoreClockUpdate();
	InitUART2(BAUD_RATE);
	InitPWM();
	InitSongs();
	//InitGPIO();
	InitMotor();
//	stopAllRGB();
	InitGreenLED();
	InitGPIOLedBlink();
	
  // ...
//	while(1) {
//		//motorForward();
//	}
//	
	
 
  osKernelInitialize();                 // Initialize CMSIS-RTOS
	//--------------------//
	
	// Create a Semaphore
	green_led_semaphore = osSemaphoreNew(1,1,NULL);
	red_led_semaphore = osSemaphoreNew(1,1,NULL);
	buzzer_semaphore = osSemaphoreNew(1,1,NULL);

	// Create Message Queue to store UART inputs
	mqid_rx = osMessageQueueNew(MQ_SIZE, 2, NULL);
	
	// Create tBrain thread to manage all inputs
	osThreadNew(tBrain, NULL, &thread1_attr_high);
	
	
	// Create threads for LED strips
	running_led_green = osThreadNew(tRunningLedGreen, NULL, NULL);
	still_led_green = osThreadNew(tGreenLedStill, NULL, NULL);
	led_red_500 = osThreadNew(tRedLed500, NULL, NULL);
	led_red_250 = osThreadNew(tRedLed250, NULL, NULL);
	
	// Create threads to run Motors
	motor_forward = osThreadNew(tMotorForward,NULL,NULL);
	motor_backward = osThreadNew(tMotorBackward,NULL,NULL);
	motor_left = osThreadNew(tMotorLeft,NULL,NULL);
	motor_right = osThreadNew(tMotorRight,NULL,NULL);
	
	osThreadNew(tPlaySong, NULL, NULL);
	motor_diag_left = osThreadNew(tMotorDiagLeft, NULL, NULL);
	motor_diag_right = osThreadNew(tMotorDiagRight, NULL, NULL);
	
	// Create thread for self driving mode
	self_driving_mode = osThreadNew(tSelfDrivingMode, NULL, NULL);


	//-------------------//
	
  osKernelStart();                      // Start thread execution
  for (;;) {}
}

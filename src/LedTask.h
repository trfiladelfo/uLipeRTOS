/*
 * LedTask.h
 *
 *  Created on: Apr 14, 2014
 *      Author: felipeneves
 */

#ifndef LEDTASK_H_
#define LEDTASK_H_

/*************************************************************
 	 	 	 	 Defines
 *************************************************************/
#define LED_TASK_STACK_SIZE 64
#define LED_DELAY_SIZE      0x800

/*************************************************************
 	 	 	 	 Led Types
 *************************************************************/
typedef enum
{
	teLED1 = 12,
	teLED2,
	teLED3,
	teLED4,

	teMAX_LEDS = 16
}teLedNumber;

typedef enum
{
	teOFF = 0,
	teON,

	teNBR_STATES
}teLedState;
/*************************************************************
 	 	 	 	 Globals
 *************************************************************/
extern os_stack_t axLedTaskStack[LED_TASK_STACK_SIZE];

/*************************************************************
 	 	 	 	 Prototypes
 *************************************************************/
void vLedSetValue(teLedState eLedState, teLedNumber eLedNumber);
uint8_t bLedGetValue(void);
void vLedMainTask(void *pvTaskArgs);

/*************************************************************
 	 	 	 	 EndOfFile
 *************************************************************/

#endif /* LEDTASK_H_ */

/*
 * ButtonTask.h
 *
 *  Created on: Apr 17, 2014
 *      Author: felipeneves
 */

#ifndef BUTTONTASK_H_
#define BUTTONTASK_H_

/*************************************************************
 	 	 	 	 Defines
 *************************************************************/
//Defines a task stack size:
#define BUTTON_TASK_STACK_SIZE 64

/*************************************************************
 	 	 	 	 Button Types
 *************************************************************/
typedef enum
{
	teRELEASED = 0x00,
	tePRESSED,

	teINVALID = 0xFF
}teButtonState;

/*************************************************************
 	 	 	 	 Globals
 *************************************************************/
extern os_stack_t axButtonTaskStack[BUTTON_TASK_STACK_SIZE];

/*************************************************************
 	 	 	 	 Prototypes
 *************************************************************/
extern uint8_t bButtonGet(void);
extern void vButtonMainTask(void *pTaskArgs);

/*************************************************************
 	 	 	 	 EndOfFile
 *************************************************************/

#endif /* BUTTONTASK_H_ */

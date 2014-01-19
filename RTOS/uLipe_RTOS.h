/***********************************************************************
						ULIPE The RTOS

	file: uLipe_RTOS.h

	description: this file includes all the modules of uLipe RTOS,
				 types, and programming conventions.


	author:Felipe Neves

 ************************************************************************/
#ifndef ULIPE_RTOS_H_
#define ULIPE_RTOS_H_	//prevents recursive compilation


/************************************************************************
 	 	 	 	 	 LibC Include files
 ************************************************************************/
#include <stdint.h>

/************************************************************************
 	 	 	 	 	 Defines and Macros
 ************************************************************************/
#define OS_TICKS 			1000 				//Tick timer in us
#define OS_TIME_QUANTA		(OS_TICKS / 10) 	//Time quanta unity

/************************************************************************
 	 	 	 	 	 Enumerations
 ************************************************************************/
typedef enum
{
	OS_OK = 0,
	OS_ERROR,
	OS_TASKLIST_FULL,
	OS_PRIORITY_OUT_OF_RANGE,
	OS_TASK_IS_NOT_HERE,
	OS_NAME_TOO_LONG,
	OS_IS_NOT_STATE
}errors_t;				//OS possible errors

/***********************************************************************
 	 	 	 	 	 Personalized types
 ***********************************************************************/

typedef void  		taskptr_t;
typedef uint8_t  	os_taskname_t;
typedef uint8_t		os_taskID_t;
typedef uint8_t     os_error_t;
typedef uint32_t 	os_stack_t; //stack type

/**********************************************************************
 	 	 	 	 	 OS include files
 **********************************************************************/

#include "uLipe_Task.h" //tasks routines

//todo
//#include "uLipe_Queue.h" //queue management

//todo
//#include "uLipe_Core.h" //OS Core

//todo
#include "AsmStuff.h" //Assembly resources

//todo
//#include "uLipe_Semaphore.h" //Semaphore resources

//todo
//#include "uLipe_Mutex.h"		//Mutex resource

//todo
//#include "uLipe_Timers.h"		//software timers

//todo
//#include "uLipe_MailBox.h"	//Mailbox resources

/****************************************************************************
 	 	 	 	 	 End of file
 ****************************************************************************/
#endif /* ULIPE_RTOS_H_ */

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
//Tick timer in us
#define OS_TICKS 			1000

//Time quanta unity
#define OS_TIME_QUANTA		(OS_TICKS / 10)

/************************************************************************
 	 	 	 	 	 Enumerations
 ************************************************************************/

//OS possible errors enumerations
typedef enum
{
	OS_OK = 0,
	OS_ERROR,
	OS_TASKLIST_FULL,
	OS_PRIORITY_OUT_OF_RANGE,
	OS_TASK_IS_NOT_HERE,
	OS_NAME_TOO_LONG,
	OS_IS_NOT_STATE,
	OS_QUEUE_FULL,
	OS_QUEUE_EMPTY,
	OS_QUEUE_OUT_OF_RANGE,

	OS_QTY_OF_ERRORS
}errors_t;

/***********************************************************************
 	 	 	 	 	 Personalized types
 ***********************************************************************/

typedef void  		taskptr_t;
typedef uint8_t  	os_taskname_t;
typedef uint8_t		os_taskID_t;
typedef uint8_t     os_error_t;
typedef uint32_t 	os_stack_t;
typedef uint8_t		os_queue_t;

/**********************************************************************
 	 	 	 	 	 OS include files
 **********************************************************************/

#ifdef __TEST_CORE
//OS Kernel
#include "uLipe_Core.h"
#endif
//Assembly resources, cpu port
#include "AsmStuff.h"

//Task management methods
#include "uLipe_Task.h"

//Queue managemete methods
//#include "uLipe_Queue.h"


//todo
//Semaphore resources
//#include "uLipe_Semaphore.h"

//todo
//Mutex resources
//#include "uLipe_Mutex.h"

//todo
//software timers and delays
//#include "uLipe_Timers.h"

//todo
//Mailbox resources
//#include "uLipe_MailBox.h"

/****************************************************************************
 	 	 	 	 	 End of file
 ****************************************************************************/
#endif /* ULIPE_RTOS_H_ */

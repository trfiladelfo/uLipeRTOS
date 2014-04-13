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
#define OS_TICK_RATE			1000

//Defines the current clock rate:
#define SYSCLK					168000000

//Defines the current number of tasks:
#define NUMBER_OF_TASKS			8

/************************************************************************
 	 	 	 	 	 Enumerations
 ************************************************************************/

//OS possible errors enumerations
typedef enum
{
	OS_OK = 0,
	OS_ERROR,
	OS_TASKLIST_FULL,
	OS_PRIORITY_INVALID,
	OS_TASK_IS_NOT_HERE,
	OS_NAME_TOO_LONG,
	OS_IS_NOT_STATE,
	OS_QUEUE_FULL,
	OS_QUEUE_EMPTY,
	OS_QUEUE_OUT_OF_RANGE,
	OS_QUEUE_NOT_EMPTY,
	OS_INDEX_OUT_OF_RANGE,
	OS_INVALID_POINTER,
	OS_INVALID_PARAMETER,

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
typedef uint32_t    base_t;

//The task control block
typedef struct os_taskTCB_t
{
	//Is the current stackpointer of task
	os_stack_t 		*TaskStack;
	//The task function properly
	taskptr_t   	(*TaskAction)(void *TaskArgs);
	//the attachment for next TCB
	struct os_taskTCB_t	*NextTask;
	//the attachment of Previous TCB
	struct os_taskTCB_t	*PrevTask;
	//Name of task in ASCII
	uint32_t		TaskFlags;
	//Task periodic deadline
	uint32_t 		TaskTime;
	//Current time elapsed
	uint32_t 		TaskElapsedTime;
	//Task priority of execution
	uint8_t	 		TaskPriority;
	//Current task state of execution
	uint8_t			TaskState;
	//Vector that contains task name
	os_taskname_t	TaskName[16];
	//flag of occupied (or not) TCB
	uint8_t		EmptyTCB;
}taskTCB_t;


/**********************************************************************
 	 	 	 	 	 OS include files
 **********************************************************************/

//Assembly resources, cpu port
#include "AsmStuff.h"

//OS Kernel
#include "uLipe_Core.h"

//Task management methods
#include "uLipe_Task.h"

//todo
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

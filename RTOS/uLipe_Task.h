/***********************************************************************
						ULIPE The RTOS

	file: uLipe_Task.h

	description: this file includes all the routines and utilities for
				 task management in uLipeRTOS.

	author:Felipe Neves


 ************************************************************************/

#ifndef ULIPE_TASK_H_
#define ULIPE_TASK_H_

/************************************************************************
 	 	 	 	 module debug directive
 ************************************************************************/
#ifdef RTOS_DEBUG

//standard types
#include <stdint.h>

#endif




/************************************************************************
 							Defines
 ************************************************************************/

//Number of used tasks in RTOS
#define NUMBER_OF_TASK  NUMBER_OF_TASKS

//first time task flags!
#define TASK_FLAG_IS_FIRST_TIME       (1 << 0)

//Defines a list terminator:
#define END_LIST 		 0xFFAAFFAA

/************************************************************************
 	 	 	 	 	 	 	Typedefs
 ************************************************************************/

//Task possible states enumeration
typedef enum
{
	TASK_READY = 0,
	TASK_SUSPEND,
	TASK_BLOCKED,
	TASK_RUNNING,
	TASK_DELETED
}taskstates_t;

/***********************************************************************
 	 	 	 	 	 Externs
 ***********************************************************************/

//Table of task control blocks
extern taskTCB_t axTaskList[];

/************************************************************************
 	 	 	 	 	 Function Prototypes
 ************************************************************************/

extern  void Task_InitBlocks(void);

extern  os_error_t 	Task_Create	(taskptr_t (*TaskAction),
								 os_stack_t *TaskStack,
								 uint8_t StackSize,
								 uint8_t TaskPriority,
								 os_taskname_t *TaskName);

extern  os_error_t 	Task_Delete(uint8_t OsPrio);

taskTCB_t* 	Task_Query(uint8_t OsPrio, os_error_t *Err);

extern	os_error_t  Task_Stop(uint8_t OsPrio);

extern  os_error_t  Task_Resume(uint8_t OsPrio);

/************************************************************************
 	 	 	 	 	 End of File
 ************************************************************************/
#endif /* ULIPE_TASK_H_ */

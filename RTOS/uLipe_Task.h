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
#define NUMBER_OF_TASK  7

//undefined task ID
#define UNDEFINED_ID   (NUMBER_OF_TASK + 2)

//first time task flags!
#define TASK_FLAG_IS_FIRST_TIME       (1 << 0)

//Defines the end of linked list
#define END_LIST		NULL

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

//The task control block
typedef struct os_taskTCB_t
{
	//Task flags register
	uint32_t		TaskFlags;
	//Task periodic deadline
	uint32_t 		TaskTime;
	//Current time elapsed
	uint32_t 		TaskElapsedTime;
	//Task unique Identifier
	uint8_t  		TaskID;
	//Task priority of execution
	uint8_t	 		TaskPriority;
	//Will be used further
	uint8_t			TaskDelayed;
	//Current task state of execution
	uint8_t			TaskState;
	//flag of occupied (or not) TCB
	uint32_t		EmptyTCB;
	//Name of task in ASCII
	os_taskname_t	*TaskName;
	//Is the current stackpointer of task
	os_stack_t 		*TaskStack;
	//The task function properly
	taskptr_t   	(*TaskAction)(void *TaskArgs);
	//the attachment for next TCB
	struct os_taskTCB_t	*NextTask;
	//the attachment of Previous TCB
	struct os_taskTCB_t	*PrevTask;

}taskTCB_t;


/***********************************************************************
 	 	 	 	 	 Externs
 ***********************************************************************/

//Stack of IDLE Task
extern os_stack_t 			IdleTaskStack[64];

//Name of TCB used for idle task
extern os_taskname_t IdleName[8];


/************************************************************************
 	 	 	 	 	 Function Prototypes
 ************************************************************************/

extern  void Task_InitBlocks(void);

extern  os_error_t 	Task_Create
					(taskptr_t (*TaskAction), os_stack_t *TaskStack,
					uint8_t TaskPriority, os_taskname_t *TaskName,
					uint8_t NameSize,os_stack_t StackSize);

extern  os_error_t 	Task_Delete(os_taskID_t TaskID);

extern	os_taskID_t Task_GetID(os_taskname_t *TaskName, uint8_t NameSize);

extern	taskTCB_t* 	Task_Query(os_taskID_t TaskID);

extern	os_error_t 	Task_ChangeState(os_taskID_t TaskID, taskstates_t State );

extern  os_error_t  Task_Suspend(os_taskID_t TaskID);

extern	os_error_t  Task_Block(os_taskID_t TaskID);

taskTCB_t* Task_GetList(void);

extern  void Task_Idle(void *TaskArgs);

/************************************************************************
 	 	 	 	 	 End of File
 ************************************************************************/
#endif /* ULIPE_TASK_H_ */

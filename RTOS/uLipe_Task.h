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

#include <stdint.h> //standard types

#endif




/************************************************************************
 							Defines
 ************************************************************************/

#define NUMBER_OF_TASK  7 					//numero de tarefas usadas no RTOS
#define UNDEFINED_ID    NUMBER_OF_TASK + 2 	//undefined task ID


/************************************************************************
 	 	 	 	 	 	 	Typedefs
 ************************************************************************/
typedef enum
{
	TASK_READY = 0,
	TASK_SUSPEND,
	TASK_BLOCKED,
	TASK_RUNNING,
	TASK_DELETED
}taskstates_t;							//Task possible states


typedef struct os_taskTCB_t
{
	uint32_t 		TaskTime;			//task time slot
	uint32_t 		TaskElapsedTime;	//task time elapsed after last slot
	uint8_t  		TaskID;				//task unique ID
	uint8_t	 		TaskPriority;		//task priority level (from 0 to 32)
	uint8_t			TaskDelayed;		//task delay flag
	uint8_t			TaskState;			//task state
	uint32_t			EmptyTCB;			//TCBEmpty
	os_taskname_t	*TaskName;			//taskname
	os_stack_t 		*TaskStack;			//tasks stack pointer
	taskptr_t   	(*TaskAction)(void *TaskArgs); 		//task action to do
	struct os_taskTCB_t	*NextTask;	//pointer to the next task TCB
	struct os_taskTCB_t	*PrevTask; 	//pointer to the previous  TCB

}taskTCB_t;							//well, the task control block

/************************************************************************
 	 	 	 	 	 Function Prototypes
 ************************************************************************/
extern  void		Task_InitBlocks(void);

extern	os_error_t 	Task_Create(taskptr_t (*TaskAction), os_stack_t *TaskStack,
						uint8_t TaskPriority, os_taskname_t *TaskName, uint8_t NameSize);

extern  os_error_t 	Task_Delete(os_taskID_t TaskID);

extern	os_taskID_t Task_GetID(os_taskname_t *TaskName, uint8_t NameSize);

extern	taskTCB_t* 	Task_Query(os_taskID_t TaskID);

extern	os_error_t 	Task_ChangeState(os_taskID_t TaskID, taskstates_t State );

extern  void 		Task_Idle(void *TaskArgs);

/************************************************************************
 	 	 	 	 	 End of File
 ************************************************************************/
#endif /* ULIPE_TASK_H_ */

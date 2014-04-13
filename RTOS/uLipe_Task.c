/***********************************************************************
						ULIPE The RTOS

	file: uLipe_Task.c

	description: this file includes all the routines and utilities for
				 task management in uLipeRTOS.

	author:Felipe Neves


 ************************************************************************/

/************************************************************************
 	 	 	 	 	 Debug/Include Directive
 ************************************************************************/
#ifdef RTOS_DEBUG

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "uLipe_Task.h"

#else

#include <stdio.h>
#include <string.h>
#include "uLipe_RTOS.h"

#endif
/************************************************************************
  					Local Defines and Constants
 ************************************************************************/
//Defines a maximum number of tasks
#define MAX_TASK_NUMBER  64

//Defines the least priority number
#define LESS_PRIORITY	 MAX_TASK_NUMBER - 1

//Defines the most proirity number
#define MAX_PRIORITY	 0

//Defines that TCB is empty
#define EMPTY			 0

//Defines that TCB is full
#define FILLED			 0xFF

/************************************************************************
 	 	 	 	 	 Public variables
 ************************************************************************/

//Table of task control blocks
taskTCB_t axTaskList[MAX_TASK_NUMBER];

/************************************************************************
 	 	 	 	 	 Module local variables
 ************************************************************************/

/************************************************************************
 	 	 	 	 	 Module local Prototypes
 ************************************************************************/

/************************************************************************
 	 	 	 	 	 Functions
 ************************************************************************/
/************************************************************************
 	 function:	Task_InitBlocks()

 	 description: this function,fills zero on all memory block
 	 	 	 	  reserved for tasks TCB.

 	 parameters: N/A

 	 return:	N/A


 ************************************************************************/
void Task_InitBlocks(void)
{
	uint8_t bI;

	//clears all TCBs first:
	for(bI = 0; bI < MAX_TASK_NUMBER; bI++)
	{
		memset((char *)&axTaskList[bI], 0 , sizeof(taskTCB_t));

		//set the TCB as free
		axTaskList[bI].EmptyTCB = EMPTY;

		//set the TCB as task deleted:
		axTaskList[bI].TaskState = TASK_DELETED;


	}

	//The Idle task is the first end terminator:
	axTaskList[MAX_TASK_NUMBER - 1].NextTask = (taskTCB_t *)END_LIST;

}

/************************************************************************
 	 function:	Task_Create()

 	 description: this function,reserves, fill and attach a task
 	 	 	 	  TCB on the task linked list

 	 parameters: -TaskAction: The function pointer to task

 	 	 	 	 -TaskStack: A pointer to a array which will
 	 	 	 	 	 	     used as task stack.

 	 	 	 	 -StackSize: Size of stack in bytes, it
 	 	 	 	 	 	     MUST be specified in BYTES!

 	 	 	 	 -TaskPriority: The priority given to task,
 	 	 	 	 	 	 	 	how less the number higher the priority,
 	 	 	 	 	 	 	 	each task has a unique priority number.

 	 	 	 	 -TaskName: A name that can be given to task, it
 	 	 	 	 	 	    can be null, if it null a '?' is set as
 	 	 	 	 	 	    name.


 	 return:	 -OS_OK: System ok, or can be one of errors, please
 	  	  	      	     refer the uLipe_RTOS.h to view the possible
 	  	  	      	     errors


 ************************************************************************/
os_error_t 	Task_Create	(taskptr_t (*TaskAction),
							os_stack_t *TaskStack,
							uint32_t StackSize,
							uint8_t TaskPriority,
							os_taskname_t *TaskName)
{
	//auxiliary pointer to tcbs:
	taskTCB_t *pTCB;

	//allocates a temporary sREG:
	uint32_t Sreg;

	//Auxiliary counter:
	uint8_t bI = 0;

	//check parameters:

	//The task method:
	if((taskptr_t *)NULL == TaskAction)
	{
		//null pointer is not accepted
		return(OS_INVALID_PARAMETER);
	}

	//check the task stack:
	if((os_stack_t *)NULL == TaskStack)
	{
		//null stackpointer not, too
		return(OS_INVALID_PARAMETER);
	}


	//check stack size
	if(StackSize == 0x00)
	{
		//stack size null is not accepted.
		return(OS_INVALID_PARAMETER);
	}

	//check for priority:
	if(0x00 != axTaskList[TaskPriority].EmptyTCB)
	{
		//the priority already exists
		return(OS_PRIORITY_INVALID);
	}

	//taskname is not checked since it can be null

	//create a critical section:
	Sreg = Asm_CriticalIn();

	//fill the free tcb:

	//task stack first:
	axTaskList[TaskPriority].TaskStack =
			(os_stack_t *)(TaskStack + (StackSize>>2));

	//The task action:
	axTaskList[TaskPriority].TaskAction =
			(taskptr_t *)TaskAction;

	//Priority:
	axTaskList[TaskPriority].TaskPriority =
			TaskPriority;

	//TaskFlags:
	axTaskList[TaskPriority].TaskFlags  =
			TASK_FLAG_IS_FIRST_TIME ;

	//Task time:
	axTaskList[TaskPriority].TaskTime = TICKS;

	//Elapsed time between executions:
	axTaskList[TaskPriority].TaskElapsedTime = 0;

	//Unfree this TCB:
	axTaskList[TaskPriority].EmptyTCB = FILLED;

	//Put task in ready state:
	axTaskList[TaskPriority].TaskState = TASK_READY;

	//create it initial stack frame:
	Core_StackFrameCreate((taskTCB_t*) &axTaskList[TaskPriority]);

	//Put this TCB on ready list:
	Core_ReadyTask(TaskPriority);

	//check if task name is a null pointer:
	if((os_taskname_t *)NULL == TaskName)
	{
		//then put a '?' as a name:
		axTaskList[TaskPriority].TaskName[0] = '?';
	}
	else
	{
		//Copy the task name to the tcb vector
		for(bI = 0; bI < 20; bI++)
		{
			axTaskList[TaskPriority].TaskName[bI] =
					*TaskName++;

			//if the name is less than the tcb field
			//interrupt the loop
			if('\0' == *TaskName)
			{
				break;
			}
		}
	}

	//Link the TCB:

	//check if the task is the IdleTask:
	if((LESS_PRIORITY) == TaskPriority)
	{
		//then puts the list terminator on its next task:
		axTaskList[TaskPriority].NextTask =
				(taskTCB_t *)END_LIST;
	}
	//else, lets link on task list:
	else
	{
		pTCB = (taskTCB_t*)&axTaskList[LESS_PRIORITY];

		//search for task list terminator:
		while((taskTCB_t *)END_LIST != pTCB->NextTask)
		{
			pTCB = (taskTCB_t *)pTCB->NextTask;
		}

		//when find, attach the tcb on list:
		pTCB->NextTask = (taskTCB_t *)&axTaskList[TaskPriority];

		//attach previous task:
		axTaskList[TaskPriority].PrevTask = (taskTCB_t *)pTCB;

		//puts the terminator on the current position:
		axTaskList[TaskPriority].NextTask = (taskTCB_t *)END_LIST;
	}


	//finishes the critical section:
	Asm_CriticalOut(Sreg);

	//return a ok, if all gone well :)
	return(OS_OK);
}
/************************************************************************
 	 function:	Task_Delete()

 	 description: this function removes a taskTCB of tasklist
 	 	 	 	  and unfill it, freeing the TCB

 	 parameters: -OsPrio -The priority of the desired task, the priori
 	 	 	 	 	 	 ty is used as a unique Task ID.


 	 return: 	-OS_OK: System ok, or can be one of errors, please
 	  	  	      	     refer the uLipe_RTOS.h to view the possible
 	  	  	      	     errors


 ************************************************************************/
os_error_t 	Task_Delete(uint8_t OsPrio)
{
	//pointer to TCB.
	taskTCB_t *pTCB;

	//allocates a temporary status reg:
	uint32_t Sreg;

	//check parameter:

	//if the task is installed:
	if(FILLED != axTaskList[OsPrio].EmptyTCB)
	{
		//if this task is not installed, return
		return(OS_PRIORITY_INVALID);
	}

	//create a critical section:
	Sreg = Asm_CriticalIn();

	//Uninstall task:

	//free block:
	axTaskList[OsPrio].EmptyTCB = EMPTY;

	//set task as deleted:
	axTaskList[OsPrio].TaskState = TASK_DELETED;

	//initialize ptcb:
	pTCB = (taskTCB_t *) axTaskList[OsPrio].PrevTask;

	//attach the previous task on next task:
	if((taskTCB_t *)END_LIST != axTaskList[OsPrio].NextTask)
	{
		pTCB->NextTask = (taskTCB_t *)axTaskList[OsPrio].NextTask;

		axTaskList[OsPrio].NextTask->PrevTask =
				(taskTCB_t *)pTCB;
	}
	//if the next task is END_LIST
	else
	{
		//the current terminator is inserted on new next task
		pTCB->NextTask = (taskTCB_t *)END_LIST;
	}

	//end critical section:
	Asm_CriticalOut(Sreg);

	//return system ok:
	return(OS_OK);
}
/************************************************************************
 	 function:	Task_Query()

 	 description: this function returns a pointer to a TCB
 	 	 	 	  that corresponds to a passed ID

 	 parameters:  -OsPrio: The priority of the task desired.

 	 	 	 	  -Err:  Pointer to error variable to store a
 	 	 	 	  	  	 System ok, or can be one of errors, please
 	  	  	      	     refer the uLipe_RTOS.h to view the possible
 	  	  	      	     errors



 	 return:  - TCB pointer of the desired task, or a null pointer
 	 	 	    if the task is not installed.

 ************************************************************************/
taskTCB_t* 	Task_Query(uint8_t OsPrio, os_error_t *Err)
{
	//check parameters:

	//if the task is installed:
	if(FILLED != axTaskList[OsPrio].EmptyTCB)
	{
		//deposit error:
		*Err = OS_PRIORITY_INVALID;

		//if this task is not installed, return
		return(NULL);
	}

	//if installed proceed.

	//simply return the tcb of table:
	return((taskTCB_t*) &axTaskList[OsPrio]);

}

/************************************************************************
 	 function:	Task_Stop()

	 description: this function suspend a task execution changing it
 	 	 	 	  to block state.

 	 parameters: -OsPrio: The priority of the task desired.


 	 return: 	 -OS_OK: System ok, or can be one of errors, please
 	  	  	      	     refer the uLipe_RTOS.h to view the possible
 	  	  	      	     errors


************************************************************************/
os_error_t  Task_Stop(uint8_t OsPrio)
{
	//Needs of a temporary status register:
	uint32_t Sreg = 0;

	//check parameter:

	//if the task is installed:
	if(FILLED != axTaskList[OsPrio].EmptyTCB)
	{
		//if this task is not installed, return
		return(OS_PRIORITY_INVALID);
	}

	//Enable Scheduler:
	Core_EnableSchedule();

	//check if task running:
	if(TASK_RUNNING == axTaskList[OsPrio].TaskState)
	{
		//only set task as Blocked
		axTaskList[OsPrio].TaskState = TASK_BLOCKED;

		//Reeschedule system
		Core_Schedule();
	}
	else
	{
		//create a critical section:
		Sreg = Asm_CriticalIn();

		//remove task from list if in ready list
		Core_UnreadyTask(OsPrio);

		//only set task as Blocked
		axTaskList[OsPrio].TaskState = TASK_BLOCKED;

		//Removes the critical section:
		Asm_CriticalOut(Sreg);

		//Reeschedule system
		Core_Schedule();
	}

	//set system ok:
	return(OS_OK);

}
/************************************************************************
 	 function:	Task_Resume()

	 description: this function suspend a task execution changing it
 	 	 	 	  to block state.

 	 parameters: -OsPrio: The priority of the task desired.

 	 return:	 -OS_OK: System ok, or can be one of errors, please
 	  	  	      	     refer the uLipe_RTOS.h to view the possible
 	  	  	      	     errors


 ************************************************************************/
os_error_t  Task_Resume(uint8_t OsPrio)
{
	//check parameter:

	//if the task is installed:
	if(FILLED != axTaskList[OsPrio].EmptyTCB)
	{
		//if this task is not installed, return
		return(OS_PRIORITY_INVALID);
	}

	//only set task as Suspend
	axTaskList[OsPrio].TaskState = TASK_SUSPEND;

	//reeschedule is not needed here.

	//set system ok:
	return(OS_OK);

}
/************************************************************************
 	 	 	 	 	 End of File
 ************************************************************************/

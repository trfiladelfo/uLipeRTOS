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
#include "uLipe_Task.h"


#endif
/************************************************************************
  					Local Defines and Constants
 ************************************************************************/
#define MAX_TASK_NUMBER  32 	//This determines the maximum lenght of taskList
#define LESS_PRIORITY	 31 	//Least priority level
#define MAX_PRIORITY	 0  	//Most priority level
#define EMPTY			 0		//
#define FILLED			 0xFF	//


/************************************************************************
 	 	 	 	 	 Module local variables
 ************************************************************************/
taskTCB_t TaskBlockList[NUMBER_OF_TASK + 1]; //Task list memory stattcally allocated

//taskTCB_t *CurrentTaskBlock = NULL	, //auxiliar task pointers
//  		  *OtherTaskBlock   = NULL	;
os_stack_t 			IdleTaskStack[64];      		//reserves some words for idle task stack
const os_taskname_t IdleName[8] = {"TaskIdle"};	//reserves a name for idle task

/************************************************************************
 	 	 	 	 	 Module local Prototypes
 ************************************************************************/
#define END_LIST		NULL //endlist


/************************************************************************
 	 	 	 	 	 Functions
 ************************************************************************/
/************************************************************************
 	 function:	Task_InitBlocks()

 	 description: this function,fills zero on all memory block
 	 	 	 	  reserved for tasks TCB.

 	 parameters: TODO

 	 return:	TODO


 ************************************************************************/
void Task_InitBlocks(void)
{
	//fill with zeros all TCBS!
	memset(&TaskBlockList, NULL, sizeof(TaskBlockList));
}

/************************************************************************
 	 function:	Task_Create()

 	 description: this function,reserves, fill and attach a task
 	 	 	 	  TCB on the task linked list

 	 parameters: TODO

 	 return:	TODO


 ************************************************************************/
os_error_t 	Task_Create(taskptr_t (*TaskAction), os_stack_t *TaskStack,
						uint8_t TaskPriority, os_taskname_t *TaskName, uint8_t NameSize)
{
	taskTCB_t *TaskList 		= &TaskBlockList,
			  *TaskToBeLinked	= &TaskBlockList; //points to tasklist

	uint8_t    LoopCntr = 0; //simple loop coounter

	//check for size of taskname:
	if(NameSize > 32)
	{
		return(OS_NAME_TOO_LONG);
	}

	//first check for a empty task block
	for(LoopCntr = 0; LoopCntr <= NUMBER_OF_TASK ; LoopCntr++)
	{
		//check if current TCB is empty
		if(EMPTY == TaskList->EmptyTCB)
		{
			//fill te task TCB
			TaskList->TaskAction = TaskAction; //the task method
			TaskList->TaskStack  = TaskStack; //the task stack

			//check for priority given
			if(TaskPriority < MAX_PRIORITY || TaskPriority > LESS_PRIORITY)
			{
				//return the system error
				return(OS_PRIORITY_OUT_OF_RANGE);
			}
			TaskList->TaskPriority = TaskPriority & 0x1F; //the task priority
			TaskList->TaskName = TaskName; 				  //gives name for task
			TaskList->TaskID   = LoopCntr;				  //gives an ID for Task
			TaskList->TaskElapsedTime = 0;                //zero the elapsed time
			TaskList->TaskDelayed = OS_TICKS;			  //byDefault the task is executed
														  //after 1 OS_TICKS
			TaskList->TaskState   = TASK_READY;			  //task initial state

			TaskList->EmptyTCB    =  FILLED;				  //occupies the TCB



			break; //break the loop

		}
		else
		{
			TaskList = &TaskBlockList[LoopCntr]; //if not check for next TCB
			if(NUMBER_OF_TASK - 1  <  LoopCntr)
			{
				return(OS_TASKLIST_FULL);
			}
		}

	}
	//after fill the taskblock is time to insert it on the
	//tasklist linked list
	TaskList = &TaskBlockList[NUMBER_OF_TASK]; //first points to the TCB Head

	//check if Idle TCB was already initialized:
	if(EMPTY == TaskList->EmptyTCB)
	{
		//if not, initialize it.
		TaskList->TaskAction = (taskptr_t *)& Task_Idle; //assign the idle task
		TaskList->TaskStack  = &IdleTaskStack;			//assign a stack
		TaskList->TaskPriority = LESS_PRIORITY + 1; 	// Idletask has the least priority
		TaskList->TaskName = (os_taskname_t *) &IdleName;//gives name for task
		TaskList->TaskID   = NUMBER_OF_TASK + 1;		 //gives an ID for Task
		TaskList->TaskElapsedTime = 0;                //zero the elapsed time
		TaskList->TaskTime 		  = OS_TICKS;		  //byDefault the task time slot offset
													  // is 1 Tick
		TaskList->TaskState   = TASK_READY;			  //task initial state

		TaskList->EmptyTCB    =  FILLED;				  //occupies the TCB

		TaskList->NextTask    = &TaskBlockList[LoopCntr]; //aditionally attach the desired task on the linked list

		TaskList = TaskList->NextTask;

		TaskList->PrevTask = &TaskBlockList[NUMBER_OF_TASK];

		TaskList->NextTask = END_LIST; //marks the end of list

	}
	else	//else only uses it as a start of the linked list
	{
		while(END_LIST != TaskList->NextTask)
		{
			//goes trhough the linked list:
			TaskList = TaskList->NextTask;
		}
		//when the end of list is found
		TaskList->NextTask = &TaskBlockList[LoopCntr - 1]; //attach the new task in the linked list
		TaskToBeLinked     = TaskList->NextTask;

		//attach the previous task
		TaskToBeLinked->PrevTask = TaskList;

		//marks the new end of list
		TaskList = TaskList->NextTask;

		TaskList->NextTask = END_LIST;

	}

	return(OS_OK);
}
/************************************************************************
 	 function:	Task_Delete()

 	 description: this function,removes a taskTCB of tasklist
 	 	 	 	  and unfill it freeing the TCB

 	 parameters: TODO

 	 return:	TODO


 ************************************************************************/
os_error_t 	Task_Delete(os_taskID_t TaskID)
{
	taskTCB_t *TaskList = &TaskBlockList[NUMBER_OF_TASK],
			  *TaskToBeDeleted = NULL					  ;  //a pointer to tasklist, another to task to be deleted
															 //starts at IdleTCB

	do														//walk through linked list
	{
		if(TaskID == TaskList->TaskID)					   //if the task found
		{

			if(END_LIST != TaskList->NextTask)
			{
				//first remove it from tasklist
				TaskToBeDeleted = TaskList;				  //save the TCB

				//points to previous TasK
				TaskList = TaskList->PrevTask;

				//the previous task now points to te task after the task will be deteled
				TaskList->NextTask = TaskToBeDeleted->NextTask;

				//Now, points the next task previous task to current task
				TaskList = TaskList->NextTask;

				TaskList->PrevTask = TaskToBeDeleted->PrevTask;
			}
			else 											//if this task is the end of list, delete it is quite simple:
			{
				//first remove it from tasklist
				TaskToBeDeleted = TaskList;				  //save the TCB

				//points to previous TasK
				TaskList = TaskList->PrevTask;

				//marks the new end list
				TaskList->NextTask = END_LIST;

			}

			//Since the task is detached from list its time to unfill de Block:
			TaskToBeDeleted->NextTask = NULL;
			TaskToBeDeleted->PrevTask = NULL;
			TaskToBeDeleted->TaskAction = NULL;
			TaskToBeDeleted->TaskStack = NULL;
			TaskToBeDeleted->TaskID    = NULL;
			TaskToBeDeleted->TaskName  = NULL;
			TaskToBeDeleted->TaskState = TASK_DELETED;

			//Sinalizes as a empty block:
			TaskToBeDeleted->EmptyTCB = EMPTY;

			//return succefull action:
			return(OS_OK);
		}
		else
		{
			TaskList = TaskList->NextTask;
		}

	}while(END_LIST != TaskList);

	return(OS_TASK_IS_NOT_HERE);							//the task is not in the linked list
}
/************************************************************************
 	 function:	Task_GetID()

 	 description: this function returns the ID that corresponds
 	 	 	 	  a task name

 	 parameters: TODO

 	 return:	TODO


 ************************************************************************/
os_taskID_t Task_GetID(os_taskname_t *TaskName, uint8_t NameSize)
{
	taskTCB_t *TaskList = &TaskBlockList[NUMBER_OF_TASK]; //a pointer to the head of tasklist

	uint8_t LoopCntr = 0;									  //a loop counter

	os_taskname_t *ReferenceName = NULL; 				 //Reference Name

	//check name size:
	if(NameSize > 32)
	{
		return(OS_NAME_TOO_LONG);
	}

	//The current TCB is the idle block, well points to next:
	TaskList = TaskList->NextTask;

	//start the loopcounter:
	LoopCntr = 0;

	//receive the poiter to name:
	ReferenceName = TaskList->TaskName;

	//go through the tasklist:
	do
	{

		//compare their names
		while(NameSize >= LoopCntr)
		{
			if(TaskName == ReferenceName)
			{
				//test the next character
				TaskName++;
				ReferenceName++;

				//increments the loop counter:
				LoopCntr++;

			}
			else
			{
				//well, try the next task
				TaskList = TaskList->NextTask;
				//restarts the counter:
				LoopCntr = 0;
				//re-initialize the name pointer:
				ReferenceName = TaskList->TaskName;

				//breaks the loop
				break;
			}

		}

		//check if the compare waas succefull:
		if(0 != LoopCntr)
		{
			//yep, then read the task ID of this TCB and return it:
			return(TaskList->TaskID);
		}

		//else restarts a new loop.

	}while(END_LIST != TaskList);

	//if the task doesnt exist return a undefined ID
	return(UNDEFINED_ID);

}
/************************************************************************
 	 function:	Task_Query()

 	 description: this function returns a pointer to a TCB
 	 	 	 	  that corresponds to a passed ID

 	 parameters: TODO

 	 return:	TODO


 ************************************************************************/
taskTCB_t* 	Task_Query(os_taskID_t TaskID)
{
	taskTCB_t *TaskList = &TaskBlockList[NUMBER_OF_TASK]; 	//a TCB that points to a registered tasklist

	//check for a undefined ID:
	if(UNDEFINED_ID == TaskID)
	{
		return(NULL); //returns a null pointer
	}

	//the alghoritm i very simple:
	do
	{
		//go trhough tasklist and compares by ID:
		if(TaskID == TaskList->TaskID)
		{
			//if the IDs was matched, return this TCB
			return (TaskList);
		}

		//if not, go to next task
		TaskList = TaskList->NextTask;
	}while(END_LIST != TaskList);

	//if the ID is not match to any registered task, returns a null pointer
	return(NULL);
}
/************************************************************************
 	 function:	Task_ChangeState()

 	 description: this function change task for a different state

 	 parameters: TODO

 	 return:	TODO


 ************************************************************************/
os_error_t 	Task_ChangeState(os_taskID_t TaskID, taskstates_t State )
{
	taskTCB_t *TaskList = &TaskBlockList[NUMBER_OF_TASK]; //a pointer for tasklist

	//check if desired state exists:
	if((State < TASK_READY) || (State > TASK_DELETED))
	{
		return(OS_IS_NOT_STATE);
	}

	//else, search for a task:
	TaskList = TaskQuery(TaskID);

	//change it state:
	TaskList->TaskState = State;

	//return the succefull action:
	return(OS_OK);
}
/************************************************************************
 	 	 	 	 	 End of File
 ************************************************************************/

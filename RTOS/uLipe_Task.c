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
//Defines a maximum number of tasks
#define MAX_TASK_NUMBER  32

//Defies the least priority number
#define LESS_PRIORITY	 31
//Defines the most proirity number
#define MAX_PRIORITY	 0

//Defines that TCB is empty
#define EMPTY			 0

//Defines that TCB is full
#define FILLED			 0xFF

//Defines the end of linked list
#define END_LIST		NULL

/************************************************************************
 	 	 	 	 	 Public variables
 ************************************************************************/

//This is the current TCB in execution
taskTCB_t *CurrentTaskBlock 	= NULL	,

//This is the first ready TCB on queue
  		  *HighReadyTaskBlock   = NULL	;

/************************************************************************
 	 	 	 	 	 Module local variables
 ************************************************************************/

//This is the table of TCBs, its size is limited by the current
//Number of tasks
taskTCB_t TaskBlockList[NUMBER_OF_TASK + 1];

//Stack of IDLE Task
os_stack_t 			IdleTaskStack[64];

//Name of TCB used for idle task
const os_taskname_t IdleName[8] = {"TaskIdle"};

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
os_error_t 	Task_Create
			(taskptr_t (*TaskAction), os_stack_t *TaskStack,
			uint8_t TaskPriority, os_taskname_t *TaskName,
			uint8_t NameSize,os_stack_t StackSize)
{
	taskTCB_t *TaskList 		= &TaskBlockList,
			  *TaskToBeLinked	= &TaskBlockList;

	uint8_t    LoopCntr = 0;

	//check for size of taskname:
	if(NameSize > 32)
	{
		//return error if too long
		return(OS_NAME_TOO_LONG);
	}

	//first check for a empty task block
	for(LoopCntr = 0; LoopCntr <= NUMBER_OF_TASK ; LoopCntr++)
	{
		//check if current TCB is empty
		if(EMPTY == TaskList->EmptyTCB)
		{
			//if yes,fill the task TCB

			//put the task function
			TaskList->TaskAction =(taskptr_t *)TaskAction;

			//put the task stack, and points to its top.
			TaskList->TaskStack  =(os_stack_t* )(TaskStack +
								  (StackSize>>2) - 1);

			//check for priority given
			if(TaskPriority < MAX_PRIORITY || TaskPriority > LESS_PRIORITY)
			{
				//return if the priority is invalid
				return(OS_PRIORITY_OUT_OF_RANGE);
			}

			//if priority is ok, continues to fill
			//the TCB

			//mask, and put the priority
			TaskList->TaskPriority = TaskPriority & 0x1F;

			//Points to the task name
			TaskList->TaskName = TaskName;

			//puts the calculated ID
			TaskList->TaskID   = LoopCntr;

			//reset the time elapsed counter
			TaskList->TaskElapsedTime = 0;

			//calculate its deadline, based on
			//its priority
			TaskList->TaskTime = OS_TICKS +
								TaskPriority;

			//Put the initial TCB state
			TaskList->TaskState   = TASK_READY;

			//if all gone well, occupies the TCB
			TaskList->EmptyTCB    =  FILLED;

			//Set the first time execution flag.
			TaskList->TaskFlags  |= TASK_FLAG_IS_FIRST_TIME;

			//filled TCB, break this loop
			break;

		}
		else
		{
			//else, try next TCB
			TaskList = &TaskBlockList[LoopCntr];

			//check for tasklist full
			if(NUMBER_OF_TASK - 1  <  LoopCntr)
			{
				//if yes, return error
				return(OS_TASKLIST_FULL);
			}
		}

	}
	//after fill the taskblock is time to insert it on the
	//tasklist linked list

	//Points first to the last TCB in Table(is head)
	TaskList = &TaskBlockList[NUMBER_OF_TASK];

	//check if Idle TCB was already initialized:
	if(EMPTY == TaskList->EmptyTCB)
	{
		//if not, initialize it.

		//Put the task function (Task Idle)
		TaskList->TaskAction = (taskptr_t *)& Task_Idle;

		//Points to task stack
		TaskList->TaskStack  = (os_stack_t *)(IdleTaskStack +
							   (sizeof(IdleTaskStack) >> 2) - 1);

		//Assign its priority(is the least)
		TaskList->TaskPriority = LESS_PRIORITY + 1;

		//Assign its name pointing to it
		TaskList->TaskName = (os_taskname_t *) &IdleName;

		//Its ID is always the greater on tasklist
		TaskList->TaskID = NUMBER_OF_TASK + 1;

		//Reset its time counter
		TaskList->TaskElapsedTime = 0;

		//Assign its Deadline based on its priority
		TaskList->TaskTime = OS_TICKS +
							TaskList->TaskPriority;

		//Set the initial state
		TaskList->TaskState   = TASK_READY;

		//Occupie its TCB
		TaskList->EmptyTCB    =  FILLED;

		//Additionally, attach its as head of tasklist
		TaskList->NextTask    = &TaskBlockList[LoopCntr];

		//attach next task...
		TaskList = TaskList->NextTask;

		//...and the previous task
		TaskList->PrevTask = &TaskBlockList[NUMBER_OF_TASK];

		//marks the end of list
		TaskList->NextTask = END_LIST;

		//Declare that task has never executed
		TaskList->TaskFlags |= TASK_FLAG_IS_FIRST_TIME;

	}

	//if Head of list exists, then attach the
	//TCB desired on it.
	else
	{

		while(END_LIST != TaskList->NextTask)
		{
			//goes trhough the linked list:
			TaskList = TaskList->NextTask;
		}

		//when the end of list is found

		//The last item of list points to our new TCB
		TaskList->NextTask = &TaskBlockList[LoopCntr - 1];

		//and our new TCB...
		TaskToBeLinked     = TaskList->NextTask;

		//...points to the ex-last item as previous task
		TaskToBeLinked->PrevTask = TaskList;

		//marks the new end of list
		TaskList = TaskList->NextTask;


		TaskList->NextTask = END_LIST;

	}

	//return a ok, if all gone well :)
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
			  *TaskToBeDeleted = NULL;

	//Go trhough the linked list
	do
	{
		//if ID matches
		if(TaskID == TaskList->TaskID)
		{
			//check for end of list
			if(END_LIST != TaskList->NextTask)
			{
				//Task found, lets remove it

				//first save the TCB to be deleted
				TaskToBeDeleted = TaskList;

				//Detach its from previous task
				TaskList = TaskList->PrevTask;

				//the previous task now points to the task after the TCB
				//will be deteled
				TaskList->NextTask = TaskToBeDeleted->NextTask;

				//Now, points the next task previous task to current task
				TaskList = TaskList->NextTask;

				TaskList->PrevTask = TaskToBeDeleted->PrevTask;
			}

			//if this task is the end of list, delete it is quite simple:
			else
			{
				//Lets remove the TCB

				//Save the TCB will be deleted
				TaskToBeDeleted = TaskList;

				//points to previous TasK
				TaskList = TaskList->PrevTask;

				//marks the new end list
				TaskList->NextTask = END_LIST;

			}

			//Since the task is detached from list,
			//reset all the fields

			//Reset its position on linked list
			TaskToBeDeleted->NextTask = NULL;
			TaskToBeDeleted->PrevTask = NULL;

			//Delete its task fuction
			TaskToBeDeleted->TaskAction = NULL;

			//Delete its task stack
			TaskToBeDeleted->TaskStack = NULL;

			//Delete the task ID
			TaskToBeDeleted->TaskID    = NULL;

			//Delete the task Name
			TaskToBeDeleted->TaskName  = NULL;

			//Reset its execution mode...
			TaskToBeDeleted->TaskState = TASK_DELETED;

			//Sinalizes as a empty block:
			TaskToBeDeleted->EmptyTCB = EMPTY;

			//return succefull action:
			return(OS_OK);
		}

		//if not this TCB...
		else
		{
			//...try next TCB
			TaskList = TaskList->NextTask;
		}

	//until the list ends...
	}while(END_LIST != TaskList);

	//if got here, the TCB doesnot exist,
	//return error
	return(OS_TASK_IS_NOT_HERE);
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
	taskTCB_t *TaskList = &TaskBlockList[NUMBER_OF_TASK];

	uint8_t LoopCntr = 0;

	os_taskname_t *ReferenceName = NULL;

	//check name size:
	if(NameSize > 32)
	{

		//if too long, return the error
		return(OS_NAME_TOO_LONG);

	}

	//The current TCB is the idle block, well points to next:
	TaskList = TaskList->NextTask;

	//start the loopcounter:
	LoopCntr = 0;

	//receive the pointer to name:
	ReferenceName = TaskList->TaskName;

	//go through the tasklist:
	do
	{

		//compare their names
		while(NameSize >= LoopCntr)
		{
			//then, compare with reference
			//char by char
			if(TaskName == ReferenceName)
			{
				//if matches, goes to next char
				TaskName++;

				//Reference advances too
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

		//check if the compare waas successfull:
		if(0 != LoopCntr)
		{

			//yep, then read the task ID
			//of this TCB and return it:
			return(TaskList->TaskID);

		}


	//if not, restarts the loop...
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
	taskTCB_t *TaskList = &TaskBlockList[NUMBER_OF_TASK];

	//check for a undefined ID:
	if(UNDEFINED_ID == TaskID)
	{
		//if yes, return a Null pointer
		return(NULL);
	}

	//the alghoritm is very simple:
	do
	{
		//go trhough tasklist and compares by ID:
		if(TaskID == TaskList->TaskID)
		{
			//if the IDs was matched, return this TCB
			return ((taskTCB_t *)TaskList);
		}

		//if not, go to next task
		TaskList = TaskList->NextTask;

	//until the list ends
	}while(END_LIST != TaskList);

	//if the ID is not match to any registered task,
	//returns a null pointer
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
	taskTCB_t *TaskList = &TaskBlockList[NUMBER_OF_TASK];

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
 	 function:	Task_Suspend()

 	 description: this function suspend a task execution

 	 parameters: TODO

 	 return:	TODO


 ************************************************************************/
os_error_t 	Task_Suspend(os_taskID_t TaskID)
{
	taskTCB_t *TaskList = &TaskBlockList[NUMBER_OF_TASK];


	//search for a task:
	TaskList = TaskQuery(TaskID);

	//check for null pointer:
	if(TaskList == NULL)
	{
		//if yes, return a system error
		return(OS_TASK_IS_NOT_HERE);
	}

	//check for running task
	if(TaskList->TaskState == TASK_RUNNING)
	{

		//change it state
		TaskList->TaskState = TASK_SUSPEND;

		//return the succefull action:
		return(OS_OK);
	}

	//if task in another state the return error:
	return(OS_ERROR);
}

/************************************************************************
 	 function:	Task_Suspend()

 	 description: this function Blocks a task execution

 	 parameters: TODO

 	 return:	TODO


 ************************************************************************/
os_error_t 	Task_Block(os_taskID_t TaskID)
{
	taskTCB_t *TaskList = &TaskBlockList[NUMBER_OF_TASK];

	//search for a task:
	TaskList = TaskQuery(TaskID);

	//check for null pointer:
	if(TaskList == NULL)
	{
		//if yes, return a system error
		return(OS_TASK_IS_NOT_HERE);
	}

	//check for running task
	if(TaskList->TaskState == TASK_RUNNING)
	{

		//change it state
		TaskList->TaskState = TASK_BLOCKED;

		//return the succefull action:
		return(OS_OK);
	}

	//if task in another state the return error:
	return(OS_ERROR);
}
/************************************************************************
 	 function:	Task_Idle()

 	 description: The task idle function, will be used for debug
 	 	 	 	  or statisticall

 	 parameters: TODO

 	 return:	TODO


 ************************************************************************/
void Task_Idle(void *TaskArgs)
{
	while(1);

}
/************************************************************************
 	 	 	 	 	 End of File
 ************************************************************************/

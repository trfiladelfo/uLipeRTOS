/***********************************************************************
						ULIPE The RTOS

	file: uLipe_Core.c

	description: This file contains all the core of OS
				 including initialization routines,
				 scheduler and core services

	author:Felipe Neves


 ************************************************************************/

/************************************************************************
 	 	 	 	 	 Debug/Include Directive
 ************************************************************************/
#ifdef RTOS_DEBUG

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "uLipe_Core.h"

#else

#include <stdio.h>
#include <string.h>
#include "uLipe_RTOS.h"
#include "uLipe_Core.h"

#endif
/************************************************************************
  					Local Defines and Constants
 ************************************************************************/

/************************************************************************
  					Local structs
 ************************************************************************/

//This struct is aboute tasklist data
typedef struct TaskQueue
{
	//this is the data of taskqueue
	taskTCB_t 		 *TaskQueueData;

	//points to next node
	struct TaskQueue *PrevNode;

	//points to the next node
	struct TaskQueue *NextNode;

	//The index of this cell on queue:
	uint8_t QueueIndex;

}TaskQueue_t;

//this is the global queue control
typedef struct
{
	//Points to head of queue:
	TaskQueue_t *Head;

	//Points to tail of queue:
	TaskQueue_t *Tail;

	//Size of queue:
	uint8_t  QueueSize;

}TaskQueueCB_t;

/************************************************************************
  					Local Variables
 ************************************************************************/
//This is the task queue table:
TaskQueue_t TaskMainQueue[NUMBER_OF_TASK + 1];

//Instatiates a queue control block:
TaskQueueCB_t TaskMainCtl;

/************************************************************************
  					Local Prototypes
 ************************************************************************/

void uLipe_StackFrameCreate(taskTCB_t *CurrTask);
os_error_t uLipe_TaskQueueInit(void);
os_error_t uLipe_TaskEnqueue(taskTCB_t *CurrTask);
os_error_t uLipe_TaskDequeue(taskTCB_t *CurrTask);
taskTCB_t *uLipe_TaskQueryByIndex(uint8_t Index);
os_error_t uLipe_TaskDropByIndex(uint8_t Index);

/************************************************************************
 	 function:	uLipe_StackFrameCreate()

 	 description: this method create a initial stack Frame.

 	 parameters: TODO

 	 return:	TODO


 ************************************************************************/
void uLipe_StackFrameCreate(taskTCB_t *CurrTask)
{
	//create a stack frame for this TCB:

	//first the APSR register
	*--CurrTask->TaskStack = 0x01000000;
	//Now the current PC (first address of task method)
	*--CurrTask->TaskStack = (taskptr_t *)HighReadyTaskBlock->TaskAction;
	//The LR register with exc_return with PSP
	*--CurrTask->TaskStack = 0xFFFFFFFD;
	//R12
	*--CurrTask->TaskStack = 0x00;
	//R3
	*--CurrTask->TaskStack = 0x00;
	//R2
	*--CurrTask->TaskStack = 0x00;
	//R1
	*--CurrTask->TaskStack = 0x00;
	//R0
	*--CurrTask->TaskStack = 0x00;
}
/************************************************************************
 	 function:	uLipe_TaskQueueInit()

 	 description: Initializes the task queue list

 	 parameters: TODO

 	 return:	TODO


 ************************************************************************/
os_error_t uLipe_TaskQueueInit(void)
{
	//create a temporary TCB
	taskTCB_t *CurrTask = NULL;

	uint8_t LoopCntr = 0;

	//initializes the QueueControl block:

	//first the head
	TaskMainCtl.Head = (TaskQueue_t *) &TaskMainQueue[0];

	//Now the tail:
	TaskMainCtl.Tail = (TaskQueue_t *) &TaskMainQueue[0];

	//The queue size:
	TaskMainCtl.QueueSize = 0;

	//mount a circular queue:
	for(LoopCntr = 0; LoopCntr <= NUMBER_OF_TASK; LoopCntr++)
	{
		//for last iteration we should to wrap next fillment:
		if(NUMBER_OF_TASK == LoopCntr)
		{
			//attach a new queue cell on the list:
			TaskMainQueue[LoopCntr].NextNode =
					&TaskMainQueue[0];
		}
		else
		{
			//attach a new queue cell on the list:
			TaskMainQueue[LoopCntr].NextNode =
					&TaskMainQueue[LoopCntr + 1];
		}

		//Fill the data with null pointer:
		TaskMainQueue[LoopCntr].TaskQueueData = NULL;

		//Put the index on queue cell:
		TaskMainQueue[LoopCntr].QueueIndex = LoopCntr;

		//For first iteration we should wrap previos fillment
		if(LoopCntr == 0)
		{
			//and attach the previous elements:
			TaskMainQueue[LoopCntr].PrevNode =
					&TaskMainQueue[NUMBER_OF_TASK];
		}
		else
		{
			//and attach the previous elements:
			TaskMainQueue[LoopCntr].PrevNode =
					&TaskMainQueue[LoopCntr - 1];

		}
		//fill the queue size every iteration:
		TaskMainCtl.QueueSize++;

		//if the queue overflows:
		if((NUMBER_OF_TASK + 1) < TaskMainCtl.QueueSize)
		{
			//return error
			return(OS_QUEUE_FULL);
		}
	}
	//since queue is mounted, let take the tasklist:

	//get the idle task address:
	CurrTask = Task_Query(Task_GetID(&IdleName, sizeof(IdleName)));

	//searches the queue for ready tasks:
	while(END_LIST != CurrTask)
	{
		//check for ready task:
		if(TASK_READY == CurrTask->TaskState)
		{
			//if it ready the put into queue:
			TaskMainCtl.Tail->TaskQueueData = (taskTCB_t *)CurrTask;

			//advances the queue for next free list:
			TaskMainCtl.Tail = (TaskQueue_t *)TaskMainCtl.Tail->NextNode;

			/**THE QUEUE WILL NEVER OVERFLOWS**/
		}
		//go to next task:
		CurrTask = (taskTCB_t *)CurrTask->NextTask;
	}

	/* At this point we get:
	   TaskMainCtl.Head = TaskMainQueue[0] and
	   TaskMainCtl.Tail = TaskMainQueue[LoopCntr]
	 */
	return(OS_OK);

}
/************************************************************************
 	 function:	uLipe_TaskEnqueue()

 	 description: Put a ready TCB on Bottom of queue

 	 parameters: TODO

 	 return:	TODO


 ************************************************************************/
os_error_t uLipe_TaskEnqueue(taskTCB_t *CurrTask)
{
	//check for queue full:
	if((NUMBER_OF_TASK + 1) <= TaskMainCtl.QueueSize)
	{
		//return queue full error:
		return(OS_QUEUE_FULL);
	}
	//Since the tail is pointing for a freelist put
	//TCB on its position:
	TaskMainCtl.Tail->TaskQueueData =
			(taskTCB_t *)CurrTask;

	//advances the queue tail for next freelist:
	TaskMainCtl.Tail =
			(TaskQueue_t *)TaskMainCtl.Tail->NextNode;

	//advances the counter:
	TaskMainCtl.QueueSize++;

	//return ok:
	return(OS_OK);

}
/************************************************************************
 	 function:	uLipe_TaskDequeue()

 	 description: Removes a task TCB from top of queue

 	 parameters: TODO

 	 return:	TODO


 ************************************************************************/
os_error_t uLipe_TaskDequeue(taskTCB_t *CurrTask)
{
	//check if queue is empty first:
	if(NULL >= TaskMainCtl.QueueSize)
	{
		//return the error:
		return(OS_QUEUE_EMPTY);
	}

	//if not lets remove the TCB is in
	//top of queue:
	CurrTask = (taskTCB_t *)TaskMainCtl.Head->TaskQueueData;

	//Move Head for new top of queue:
	TaskMainCtl.Head = (TaskQueue_t *)TaskMainCtl.Head->NextNode;

	//Decrement the size,since a element was removed from
	//queue:
	TaskMainCtl.QueueSize--;

	//return a ok:
	return(OS_OK);
}
/************************************************************************
 	 function:	uLipe_TaskQueryByIndex()

 	 description: Searches for a task on the queue without
 	 	 	 	  remove it from.

 	 parameters: TODO

 	 return:	TODO


 ************************************************************************/
taskTCB_t *uLipe_TaskQueryByIndex(uint8_t Index)
{
	//auxiliar task querie
	TaskQueueCB_t *Query = &TaskMainCtl;

	//auxiliar counter:
	uint8_t Counter = 0;

	//check if index is in current queue range:
	if((Index <TaskMainCtl.QueueSize)||(Index > TaskMainCtl.QueueSize))
	{
		//return a Null pointer
		return ((taskTCB_t *)NULL);
	}
	//start to search for task Index:

	//shifts the temporary queue to desired index:
	for(Counter = 0; Counter < Index; Counter++)
	{
		//go on queue trhough the temporary one:
		Query->Tail = (TaskQueue_t *)Query->Head->NextNode;

	}

	//the task was found but not removed:
	return((taskTCB_t*)Query->Tail->TaskQueueData);

}
/************************************************************************
 	 function:	uLipe_TaskDropByIndex()

 	 description: Searches for a task on the queue and remove it
 	 	 	 	  from

 	 parameters: TODO

 	 return:	TODO


 ************************************************************************/
os_error_t uLipe_TaskDropByIndex(uint8_t Index)
{
	//creates a temporaty queue control block
	TaskQueueCB_t *Query = &TaskMainCtl;

	//creates temporary Queue
	TaskQueue_t *CellToBeRemoved = NULL;

	//Simple loop counter:
	uint8_t Counter = 0;

	//check if Index is in range
	if((Index <TaskMainCtl.QueueSize)||(Index > TaskMainCtl.QueueSize))
	{
		//return error
		return (OS_QUEUE_OUT_OF_RANGE);
	}

	//If is in, proceed with search:

	//shifts the temporary queue on desired index:
	for(Counter = 0; Counter<Index ; Counter++)
	{
		Query->Tail = (TaskQueue_t *)TaskMainCtl.Head->NextNode;
	}


	//Get the cell
	CellToBeRemoved = (TaskQueue_t*)Query->Tail;

	//use the Head as a temporary:

	//find previous element
	Query->Head = (TaskQueue_t*)Query->Tail->PrevNode;

	//make this element to skip its current next:
	Query->Head->NextNode = (TaskQueue_t*)CellToBeRemoved->NextNode;

	//Now the next element:
	Query->Tail = (TaskQueue_t*)Query->Head->NextNode;

	//Points this to the new previous element:
	Query->Tail =(TaskQueue_t*) CellToBeRemoved->PrevNode;

	//and move the removed cell for back to Head:
	TaskMainQueue[0].PrevNode =
			(TaskQueue_t*)CellToBeRemoved;
	//and move the last queue element for front of tail:
	TaskMainQueue[NUMBER_OF_TASK+1].NextNode =
			(TaskQueue_t*)CellToBeRemoved;

	//And reconfigure the element removed:
	CellToBeRemoved->NextNode =
			&TaskMainQueue[0];

	CellToBeRemoved->PrevNode =
			&TaskMainQueue[NUMBER_OF_TASK+1];

	//Return ok!
	return(OS_OK);
}
/************************************************************************
 	 function:	uLipe_Init()

 	 description: Initializes the OS and all kernel objects

 	 parameters: TODO

 	 return:	TODO


 ************************************************************************/
os_error_t uLipe_Init(void)
{
	//return ok
	return(OS_OK);
}
/************************************************************************
 	 function:	uLipe_Schedule()

 	 description: This rountines executes the scheduling alghoritm

 	 parameters: TODO

 	 return:	TODO


 ************************************************************************/
void  uLipe_Schedule(void)
{

}
/************************************************************************
 	 function:	uLipe_Start()

 	 description: This function starts the OS performing the first
 	 	 	 	  task execution.

 	 parameters: TODO

 	 return:	TODO


 ************************************************************************/
void uLipe_Start(void)
{

}
/************************************************************************
  					End of File
 ************************************************************************/

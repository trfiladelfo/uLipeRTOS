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


}TaskQueue_t;

//this is the global queue control
typedef struct
{
	//Points to head of queue:
	TaskQueue_t *Head;

	//Points to tail of queue:
	TaskQueue_t *Tail;

	//current Head Index:
	uint8_t HeadCurrIndex;

	//current Tail Index:
	uint8_t TailCurrIndex;

	//Size of queue:
	uint8_t  QueueSize;

}TaskQueueCB_t;
/************************************************************************
 	 	 	 	 	Public Variables
 ************************************************************************/
//This is the current TCB in execution
taskTCB_t *CurrentTaskBlock 	= NULL	,

//This is the first ready TCB on queue
  		  *HighReadyTaskBlock   = NULL	;

/************************************************************************
  					Local Variables
 ************************************************************************/
//Ready tasks queue
TaskQueue_t TaskMainQueue[NUMBER_OF_TASK + 1];

//Ready tasks queue control block
TaskQueueCB_t TaskMainCtl;

//Global tick counter:
volatile uint32_t  TickCounter = 0;

/************************************************************************
  					Local Prototypes
 ************************************************************************/

void uLipe_StackFrameCreate(taskTCB_t *CurrTask);
os_error_t uLipe_TaskQueueInit(void);
os_error_t uLipe_TaskEnqueue(taskTCB_t *CurrTask);
os_error_t uLipe_TaskDequeue(taskTCB_t *CurrTask);
os_error_t uLipe_EmptyQueue(void);
os_error_t uLipe_TaskQueryByIndex(uint8_t Index, taskTCB_t *DesiredTask);
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
	*--CurrTask->TaskStack = (uint32_t)CurrTask->TaskAction;
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

	//Initialize indexes:
	TaskMainCtl.HeadCurrIndex = TaskMainCtl.TailCurrIndex = 0;

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
	}
	//since queue is mounted, let take the tasklist:

	//get the idle task address:
	CurrTask = Task_Query(Task_GetID((os_taskname_t *)&IdleName, sizeof(IdleName)));

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

			//Increments the bottom queue current index:
			TaskMainCtl.TailCurrIndex++;

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

	//advances tail index:
	TaskMainCtl.TailCurrIndex = (TaskMainCtl.TailCurrIndex + 1)
			% (NUMBER_OF_TASK + 1);

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

	//Advances de the current index:
	TaskMainCtl.HeadCurrIndex = (TaskMainCtl.HeadCurrIndex + 1)
			% (NUMBER_OF_TASK + 1);

	//return a ok:
	return(OS_OK);
}
/************************************************************************
 	 function:	uLipe_EmptyQueue()

 	 description: Check if ready task queue is empty

 	 parameters: TODO

 	 return:	TODO


 ************************************************************************/
os_error_t  uLipe_EmptyQueue(void)
{
	//check the current queue size
	if(NULL != TaskMainCtl.QueueSize)
	{
		//return is not empty
		return(OS_QUEUE_NOT_EMPTY);
	}
	else
	{
		return(OS_QUEUE_EMPTY);
	}
}

/************************************************************************
 	 function:	uLipe_TaskQueryByIndex()

 	 description: Searches for a task on the queue without
 	 	 	 	  remove it from.

 	 parameters: TODO

 	 return:	TODO


 ************************************************************************/
os_error_t uLipe_TaskQueryByIndex(uint8_t Index, taskTCB_t *DesiredTask)
{


	//check if the current index comports a
	//ready task:
	if((Index < TaskMainCtl.HeadCurrIndex ) &&
			(Index > TaskMainCtl.TailCurrIndex ))
	{
		//put a null pointer on TCB:
		DesiredTask = (taskTCB_t*)NULL;

		//and return error
		return(OS_INDEX_OUT_OF_RANGE);
	}

	//since the index is in range and corresponds directly
	//to ready list table only indexes it with the passed
	//index.

	DesiredTask = (taskTCB_t*)&TaskMainQueue[Index].TaskQueueData;

	//return succefull action..
	return(OS_OK);
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
	//Auxiliaries:
	uint8_t LoopCntr = 0;

	//check if index corresponds to a ready task:
	if((Index < TaskMainCtl.HeadCurrIndex ) &&
			(Index > TaskMainCtl.TailCurrIndex ))
	{
		//return a error of index
		return(OS_INDEX_OUT_OF_RANGE);
	}

	//since the ready task exists, let remove it:
	for(LoopCntr = Index; LoopCntr <= TaskMainCtl.TailCurrIndex;
			LoopCntr = (LoopCntr + 1)% (NUMBER_OF_TASK + 1))
	{
		//we simply overwrite the current tcbs in one previous position:
		TaskMainQueue[LoopCntr].TaskQueueData =
				TaskMainQueue[(LoopCntr + 1)%(NUMBER_OF_TASK + 1)].TaskQueueData;

	}
	//updates the Tail
	TaskMainCtl.Tail = TaskMainCtl.Tail->PrevNode;

	//updates the tail index:
	TaskMainCtl.TailCurrIndex--;

	//wraps to maximum queue table length:
	if(TaskMainCtl.TailCurrIndex > NUMBER_OF_TASK)
	{
		TaskMainCtl.TailCurrIndex = NUMBER_OF_TASK;
	}

	//decrements the queue size, since a "middle data"
	//was dequeued
	TaskMainCtl.QueueSize--;

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
	//Initialize the tasks control blocks:
	Task_InitBlocks();

	//Queue Init

	//Mutex Init

	//Mailbox Init

	//Timers

	//Semaphores

	//reset the tick counter:
	TickCounter = 0;

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
	//auxiliar tcb pointer:
	taskTCB_t *AuxiliarTcb = NULL;

	//create a temporary status register:
	uint32_t StatusReg = 0;

	//a auxiliar index for search tasks:
	uint8_t  Index = 0, AuxiliarIndex = 0;


    //only schedule if queue is
	//not empty
    if(NULL != TaskMainCtl.QueueSize)
    {
    	//if not empty search for ready tasks:

    	//make head queue as a start point
    	//to queury a queue:
    	Index = TaskMainCtl.HeadCurrIndex;

    	//perform a search:
    	while(Index < TaskMainCtl.TailCurrIndex)
    	{
    		//create a critical code section:
    		StatusReg = Asm_CriticalIn();

    		//query for current index task:
    		uLipe_TaskQueryByIndex(Index, AuxiliarTcb);

    		//check for invalid value in HighPriorityReady:
    		if((taskTCB_t *)NULL == HighReadyTaskBlock)
    		{
    			//assign a valid value on it
    			HighReadyTaskBlock = (taskTCB_t*)AuxiliarTcb;
    		}
    		else
    		{
    			//check which priority is higher, remember
    			//that less value of priority most high is it:

    			if(HighReadyTaskBlock->TaskPriority > AuxiliarTcb->TaskPriority)
    			{
    				//if a new high priority is appeared
    				//get it
    				HighReadyTaskBlock = (taskTCB_t *)AuxiliarTcb;

    				//Save the candidate of most priority task on queue index:
    				AuxiliarIndex = Index;

    			}
    		}

    		//go to next task in queue
    		Index++;

    		//end of critical section.
    		Asm_CriticalOut(StatusReg);

    	}


    	//check if the most priority task is in top of queue:
    	if(AuxiliarIndex == TaskMainCtl.TailCurrIndex)
    	{
    		//The only dequeues it:
    		uLipe_TaskDequeue(AuxiliarTcb);
    	}
    	else
    	{
    		//if task is other position inside of queue
    		//drop it
    		uLipe_TaskDropByIndex(AuxiliarIndex);
    	}

    	//create a critical code section:
    	StatusReg = Asm_CriticalIn();

    	//Set task as running:
    	HighReadyTaskBlock->TaskState = TASK_RUNNING;

    	//Perform a context switching:
    	Asm_IntLevelContextChange();

    	//ends the critical section:
    	Asm_CriticalOut(StatusReg);

    }
    else
    {

    	//if no tasks ready, execute again the current task
    	CurrentTaskBlock->TaskState = TASK_RUNNING;

       	//make HighPriorityReady = CurrentTaskBlock:
        HighReadyTaskBlock = (taskTCB_t*)CurrentTaskBlock;


    	/*
    	//enable a critical section
    	StatusReg = Asm_CriticalIn();


    	//Set this task is in exectution:
    	HighReadyTaskBlock->TaskState = TASK_RUNNING;

    	//disable a critical section
    	Asm_CriticalOut(StatusReg);

    	//perform a context switch:
    	Asm_TaskLevelContextChange();
        */
    }

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

	//allocates a StatusRegister:
//	uint32_t StatusReg = 0x100;

	//pointer to task list:
	taskTCB_t *TaskList = (taskTCB_t *)NULL;

	//first get the current task list:
	TaskList = (taskTCB_t*)Task_GetList();

	//Go trhough task list and creates its initial frames:
	while((taskTCB_t*)NULL != TaskList)
	{
		//build initial stack frame:
		uLipe_StackFrameCreate((taskTCB_t*) TaskList);

		//go to next task:
		TaskList = TaskList->NextTask;
	}

	//Create critical code section:
   //StatusReg = Asm_CriticalIn();

	//Since the stack frames are created,
	//initialize the task ready list:
	uLipe_TaskQueueInit();

	//the system is ready to execute, get the first task in current task:
	uLipe_TaskDequeue((taskTCB_t*)CurrentTaskBlock);

	//and finally schedule at first time:
	uLipe_Schedule();

	//should not get here:
	while(1)
	{

	}

}
/************************************************************************
 	 function:	Systick_Handler()

 	 description: This function managers the time of OS and
 	 	 	 	  interrupts tasks execution if needed

 	 parameters: TODO

 	 return:	TODO


 ************************************************************************/
void SysTick_Handler(void)
{
	//points to tasklist:
	taskTCB_t *TaskList = (taskTCB_t *) NULL;

	//allocates a status register:
	uint32_t StatusReg = 0;

	//Enable critical section:
	StatusReg = Asm_CriticalIn();

	//First of all, update the current Tick:
	TickCounter++;

	//Disable critical section:
	Asm_CriticalOut(StatusReg);

	//Get the head of tasklist:
	TaskList = (taskTCB_t*)Task_GetList;

	//Suspend the current task:
	CurrentTaskBlock->TaskState = TASK_SUSPEND;

	//Store the current tick counter:
	CurrentTaskBlock->TaskElapsedTime = TickCounter;

	//go trhough task list and executes
	//the admission control:
	while((taskTCB_t*)NULL != TaskList)
	{

		//check if task is not in ready state:
		if(TASK_READY != TaskList->TaskState)
		{
			//search if a tick already occurred:
			if(OS_RATE <= (TickCounter - TaskList->TaskElapsedTime))
			{
				//then set this task as a ready:
				TaskList->TaskState = TASK_READY;

				//Read current tick counter:
				TaskList->TaskElapsedTime = TickCounter;

				//Put this task control block on
				//ready list:
				uLipe_TaskEnqueue((taskTCB_t*) TaskList);
			}

		}
		//in other cases, go to next task:
		TaskList = TaskList->NextTask;
	}

	//after proccess tick counter,
	//its time to schedule the next ready task:
	uLipe_Schedule();

}
/************************************************************************
 	 function:	uLipe_GetCurrentTick()

 	 description: Gets the current tick counter value

 	 parameters: TODO

 	 return:	TODO


 ************************************************************************/
uint32_t uLipe_GetCurrentTick(void)
{
	//return the current count:
	return(TickCounter);
}
/************************************************************************
  					End of File
 ************************************************************************/

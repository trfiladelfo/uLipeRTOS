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

#endif
/************************************************************************
  					Local Defines and Constants
 ************************************************************************/
//Defines a quantity of time quanta that corresponds a tick
#define TICK  64

//Defines a time quanta unity:
#define TIME_QUANTA      (OS_TICK_RATE/ TICK)

//Define the reload time value:
#define TIMER_LOAD       (SYSCLK / TIME_QUANTA)

//Defines for scheduling on or off:
#define SCHED_EN		0

#define SCHED_DIS       0xAA

//Defines schedule disabled or not:
#define OS_EN 			0xFF

#define OS_DIS			0x00

//Defines a maximum number of tasks
#define MAX_TASK_NUMBER  64

/************************************************************************
 	 	 	 	 	Public Variables
 ************************************************************************/
//This is the current TCB in execution
taskTCB_t *pxCurrentTask     = NULL	,

//This is the first ready TCB on queue
  		  *pxHighReadyTask   = NULL	;

/************************************************************************
  					Local Variables
 ************************************************************************/

//Global tick counter:
volatile uint32_t  dTickCounter = 0;

//Schedule flag:
uint8_t  bScheduleFlag = SCHED_DIS;

//OS Enabled flag:
uint8_t bOsEnable     = OS_DIS;

//OS ready group task:
uint8_t bReadyGroup  = 0;

//Map table:
uint8_t abReadyListGrp[8];

/************************************************************************
  					Constants
 ************************************************************************/
//Bit mask table to set or clear ready group
uint8_t const abBitPrioMask[8] =
{
	0x01,
	0x02,
	0x04,
	0x08,
	0x10,
	0x20,
	0x40,
	0x80
};

//This table maps the least bit position significant for a number
//between 0 and 0xFF:
uint8_t const abLeastBitMap[256] =
{
	0u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u,
	4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u,
	5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u,
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u,
    6u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u,
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u,
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u,
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u,
    7u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u,
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u,
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u,
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u,
    6u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u,
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u,
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u,
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u
};


/************************************************************************
  					Local Prototypes
 ************************************************************************/


/************************************************************************
 	 function:	Core_ReadyTask()

 	 description: This function makes a task to ready to
 	 	 	 	  run

 	 parameters:  -bOsPrio: The priority of the task desired.

 	 return:	  N/A


 ************************************************************************/
void Core_ReadyTask(uint8_t bOsPrio)
{
	//high bits of priority
	uint8_t bPrioH = 0;

	//low bits of priority
	uint8_t bPrioL  = 0;

	//breaks the given priority
	//in two parts:

	//High part first:
	bPrioH = (uint8_t)((bOsPrio >> 3) & 0x07);

	//Low part now:
	bPrioL = (uint8_t)(bOsPrio & 0x07);

	//set the bit thats match of prio in
	//ready group:
	bReadyGroup |= abBitPrioMask[bPrioH];

	//Now, maps the table of ready group
	//which contains this task:
	abReadyListGrp[bPrioH] |= abBitPrioMask[bPrioL];

}
/************************************************************************
 	 function:	Core_UnreadyTask()

 	 description: This function removes a task from ready list

 	 parameters: -bOsPrio: The priority of the task desired.

 	 return:	 N/A


************************************************************************/
void Core_UnreadyTask(uint8_t bOsPrio)
{
	//high bits of priority
	uint8_t bPrioH = 0;

	//low bits of priority
	uint8_t bPrioL  = 0;

	//breaks the given priority
	//in two parts:

	//High part first:
	bPrioH = (uint8_t)((bOsPrio >> 3) & 0x07);

	//Low part now:
	bPrioL = (uint8_t)(bOsPrio & 0x07);

	//clar the bit thats match of prio in
	//ready group:
	bReadyGroup &= ~(abBitPrioMask[bPrioH]);

	//Now, maps the table of ready group
	//which contains this task:
	abReadyListGrp[bPrioH] &= ~(abBitPrioMask[bPrioL]);

}

/************************************************************************
 	 function:	Core_Schedule()

 	 description: This function schedules the next high priority
 	 	 	 	  ready task for execution, and requests a context
 	 	 	 	  switching.

 	 parameters: N/A

 	 return:	 N/A


 ************************************************************************/
void  Core_Schedule(void)
{
	//high bits of priority
	uint8_t bPrioH = 0;

	//low bits of priority
	uint8_t bPrioL  = 0;

	//Alocates a temporary priority byte:
	uint8_t bOsPrio = 0;

	//Allocates a temporary status register:
	uint32_t Sreg = 0;

	//only schedule if:
	if(SCHED_EN != bScheduleFlag)
	{
		//create a critical section:
		Sreg = Asm_CriticalIn();

		//reads the ready group and checks for the
		//least bit set (highest priority)
		bPrioH = (abLeastBitMap[bReadyGroup] & 0x07);

		//reads now the least significant bit of
		// the selected group (highest priority)
		bPrioL = (abLeastBitMap[abReadyListGrp[bPrioH]] & 0x07);

		//combines prio high and low to form the HPT:
		bOsPrio =(uint8_t) (((bPrioH << 3))  + (bPrioL & 0x07));

		//whew...gives the address of highest priority task to the
		//highreadytaskblock:
		pxHighReadyTask = (taskTCB_t *) &axTaskList[bOsPrio];

		//Set this task as running:
		pxHighReadyTask->TaskState = TASK_RUNNING;

		//check if a task context switch is needed:
		if(pxHighReadyTask != pxCurrentTask)
		{

			//Removes this task from ready list:
			Core_UnreadyTask(bOsPrio);

			//Disable the scheduler:
			bScheduleFlag = SCHED_DIS;

			//removes the critical section:
			Asm_CriticalOut(Sreg);

			//Perform a context switch:
			Asm_TaskLevelContextChange();
		}
		else
		{
			//removes the critical section
			Asm_CriticalOut(Sreg);
		}
	}

}
/************************************************************************
 	 function:	Core_StackFrameCreate()

 	 description: this method create a initial stack Frame.

 	 parameters:  pxCurrTask - A task control block pointer, from it
 	 	 	 	 	 	 	 the taskStack field is acessed and filled
 	 	 	 	 	 	 	 with initial values for task stack

 	 return:	N/A


 ************************************************************************/
void Core_StackFrameCreate(taskTCB_t *pxCurrTask)
{
	//create a stack frame for this TCB:

	//first the APSR register
	*--pxCurrTask->TaskStack = 0x01000000;
	//Now the current PC (first address of task method)
	*--pxCurrTask->TaskStack = (uint32_t)pxCurrTask->TaskAction;
	//The LR register with exc_return with PSP
	*--pxCurrTask->TaskStack = 0xFFFFFFFD;
	//R12
	*--pxCurrTask->TaskStack = 0x00;
	//R3
	*--pxCurrTask->TaskStack = 0x00;
	//R2
	*--pxCurrTask->TaskStack = 0x00;
	//R1
	*--pxCurrTask->TaskStack = 0x00;
	//R0
	*--pxCurrTask->TaskStack = 0x00;
}
/************************************************************************
 	 function:	Core_Init()

 	 description: Initializes the OS and all kernel objects

 	 parameters: N/A

 	 return:	 OS_OK - System ok, all objects were initialized.


 ************************************************************************/
os_error_t Core_Init(void)
{
	//Initialize the tasks control blocks:
	Task_InitBlocks();

	//Queue Init

	//Mutex Init

	//Mailbox Init

	//Timers

	//Semaphores

	//Disable the OS:
	bOsEnable = OS_DIS;

	//reset the tick counter:
	dTickCounter = 0;

	//return ok
	return(OS_OK);
}
/************************************************************************
 	 function:	Core_Start()

 	 description: This function starts the OS performing the first
 	 	 	 	  task execution, this routines gives the system control
 	 	 	 	  to OS, so when executed and if OS was correctly configured
 	 	 	 	  it will never returns.

 	 parameters: N/A

 	 return:	 N/A

	 OBS.: This routine must be called for user application in order
	 	   to give its control to OS, but also it MUST be called after
	 	   all tasks were instaled and after a Core_Init().

 ************************************************************************/
void Core_Start(void)
{
	//Since the all system is set up,
	//enables the tick counter:
	Asm_LowLevelTickInit(TIMER_LOAD);

	//Enable the scheduler:
	Core_EnableSchedule();

	//Schedule the highest priority task:
	Core_Schedule();

	//Since all gone well, gives the control to
	//kernel:
	bOsEnable = OS_EN;

	//should not return from here:
	while(1);
}
/************************************************************************
 	 function:	Core_GetCurrentTick()

 	 description: Gets the current tick counter value

 	 parameters: N/A

 	 return:	 - Current tick counter value.


 ************************************************************************/
uint32_t Core_GetCurrentTick(void)
{
	//return the current count:
	return(dTickCounter);
}
/************************************************************************
 	 function:	Core_EnableSchedule()

 	 description: Enable schedule allowing next task execution
 	 	 	 	  selection

 	 parameters: N/A

 	 return:	 N/A


 ************************************************************************/
void Core_EnableSchedule(void)
{
	//check if scheduler is already enabled:
	if(SCHED_DIS != bScheduleFlag)
	{
		bScheduleFlag = SCHED_EN;
	}
}
/************************************************************************
 	 function:	Core_DisableSchedule()

 	 description: Disable schedule, not allowing next task execution
 	 	 	 	  selection

 	 parameters:  N/A

 	 return:	  N/A


 ************************************************************************/
void Core_DisableSchedule(void)
{
	//check if scheduler is already disabled:
	if(SCHED_EN != bScheduleFlag)
	{
		bScheduleFlag = SCHED_DIS;
	}
}

/************************************************************************
 	 function:	Core_OsEnable()

 	 description: Gives the application control to the RTOS

 	 parameters:  N/A

 	 return:	  N/A


 ************************************************************************/
void Core_OsEnable(void)
{
	bOsEnable = OS_EN;
}
/************************************************************************
 	 function:	Core_OsDisable()

 	 description: Disable the RTOS execution

 	 parameters:  N/A

 	 return:	  N/A


 ************************************************************************/
void Core_OsDisable(void)
{
	bOsEnable = OS_DIS;
}
/************************************************************************
 	 function:	Core_IdleTask()

 	 description: This task runs in less priority, is used
 	 	 	 	  for statistical purposes.

 	 parameters:  N/A

 	 return:	  N/A


 ************************************************************************/
void Core_IdleTask(void *pxTaskArgs)
{
	//allocates a counter
	uint32_t dExecCounter = 0;

	//status register:
	uint32_t dSreg = 0;

	//every task is a infinite loop:
	while(1)
	{
		//create a critical section
		dSreg = Asm_CriticalIn();

		//increments the execution counter:
		dExecCounter++;

		//ends the critical section:
		Asm_CriticalOut(dSreg);
	}
}
/************************************************************************
 	 function:	Core_TimeTick()

 	 description: This handler is called after a timeout generated
 	 	 	      by hardware timer of target processor. Its primary
 	 	 	      function is to increment the global tick counter
 	 	 	      the go trhough installed tasks and check for a TICK
 	 	 	      timer event, if yes set task as ready for execution
 	 	 	      and put its on ready tasks queue.

 	 parameters:  N/A

 	 return:	  N/A


 ************************************************************************/
void Core_TimeTick(void)
{

	//TCB to check the task list:
	taskTCB_t * pTCB = NULL;

	//allocates a status register:
	uint32_t  Sreg = 0;

	//create a critical section:
	Sreg = Asm_CriticalIn();

	//increments the tick counter:
	dTickCounter++;

	//ends the critical section:
	Asm_CriticalOut(Sreg);


	//check first if the OS is enabled:
	if(OS_EN == bOsEnable)
	{

		//check if the tick period of current task is expired
		if((dTickCounter - pxCurrentTask->TaskElapsedTime) >= TICKS)
		{
			//Suspend the current task:
			pxCurrentTask->TaskState = TASK_SUSPEND;

			//reads the current tick counter:
			pxCurrentTask->TaskElapsedTime = dTickCounter;

			//Enable Scheduler:
			bScheduleFlag = SCHED_EN;

		}


		//points to head of tasklist
		//always will be the lowest priority task:
		pTCB = (taskTCB_t *) &axTaskList[MAX_TASK_NUMBER];

		//go trhough task list:
		while((taskTCB_t *)END_LIST != pTCB)
		{
			//check if task is blocked:
			if(TASK_BLOCKED != pTCB->TaskState)
			{

				//if not blocked, check if state is different
				//of running:
				if(TASK_SUSPEND == pTCB->TaskState)
				{
					//check the current pTCB is ready:
					if(dTickCounter - pTCB->TaskElapsedTime >= pTCB->TaskTime)
					{
						//if is ready put on ready list:
						pTCB->TaskState = TASK_READY;

						//put this task on ready group list:
						Core_ReadyTask((uint8_t)pTCB->TaskPriority);

						//check if this task has more priority
						//than the current task:
						if(pTCB->TaskPriority < pxCurrentTask->TaskPriority)
						{
							//So, enable the scheduler:
							bScheduleFlag = SCHED_EN;
						}

					}

				}

			}
			//go to next list position:
			pTCB = (taskTCB_t *)pTCB->NextTask;
		}

		//after to pass all the task list
		//find (if necessary) the new high priority task ready to run:
		Core_Schedule();

	}
}
/************************************************************************
 	 function:	Systick_Handler()

 	 description: This handler is called after a timeout generated
 	 	 	      by hardware timer of target processor. Its primary
 	 	 	      function is to increment the global tick counter
 	 	 	      the go trhough installed tasks and check for a TICK
 	 	 	      timer event, if yes set task as ready for execution
 	 	 	      and put its on ready tasks queue.

 	 parameters:  N/A

 	 return:	  N/A


 ************************************************************************/
void Systick_Handler(void)
{
	//calls the tick processing routine...
	Core_TimeTick();
}
/************************************************************************
  					End of File
 ************************************************************************/

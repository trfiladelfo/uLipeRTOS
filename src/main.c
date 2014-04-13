#include "stm32f4xx.h"
#include <stdio.h>
#include "uLipe_RTOS.h"
#include "Tasks.h"



int main(void)
{
	//Error variable:
	uint8_t bErr = OS_OK;


	//Initialize the kernel objects:
	bErr = Core_Init();

	//check if all gone well:
	if(OS_OK != bErr)
	{
		//traps
		while(1);
	}

	//Install tasks:
	bErr = Task_Create	((taskptr_t *) &TaskTry,
						 (os_stack_t *)&TaskTryStack,
						 sizeof(TaskTryStack),
						 0,
						 (os_taskname_t *)&Try);

	//check if all gone well:
	if(OS_OK != bErr)
	{
		//traps
		while(1);
	}


	//Install tasks:
	bErr = Task_Create	((taskptr_t *) &TaskTry2,
						 (os_stack_t *)&TaskTryStack2,
						 sizeof(TaskTryStack2),
						 47,
						 (os_taskname_t *)&Try2);

	//check if all gone well:
	if(OS_OK != bErr)
	{
		//traps
		while(1);
	}


	//Then, start the kernel:
	Core_Start();


	while (1);

}



// ----------------------------------------------------------------------------


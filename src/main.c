#include "stm32f4xx.h"
#include <stdio.h>
#include "uLipe_RTOS.h"

//Led tasks includes:
#include "LedTask.h"

//ButtonTask includes:
#include "ButtonTask.h"


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

	//Install led tasks:
	bErr = Task_Create	((taskptr_t *) &vLedMainTask,
						 (os_stack_t *)&axLedTaskStack,
						 sizeof(axLedTaskStack),
						 10,
						 (os_taskname_t *)"LedTask");

	//check if all gone well:
	if(OS_OK != bErr)
	{
		//traps
		while(1);
	}

	//Install button task:
	bErr = Task_Create	((taskptr_t *) &vButtonMainTask,
						 (os_stack_t *)&axButtonTaskStack,
						 sizeof(axButtonTaskStack),
						 9,
						 (os_taskname_t *)"ButtonTask");

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


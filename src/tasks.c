/*
 * tasks.c
 *
 *  Created on: Dec 30, 2013
 *      Author: felipeneves
 */


#include "uLipe_RTOS.h"
#include "tasks.h"


os_stack_t TaskTryStack[64];
os_taskname_t Try[7] = {"TaskTry"};


os_stack_t TaskTryStack2[64];
os_taskname_t Try2[8] = {"TaskTry2"};

os_stack_t TaskTryStack3[64];
os_taskname_t Try3[8] = {"TaskTry3"};

os_stack_t TaskTryStack4[64];
os_taskname_t Try4[8] = {"TaskTry4"};

os_stack_t TaskTryStack5[64];
os_taskname_t Try5[8] = {"TaskTry5"};

os_stack_t TaskTryStack6[64];
os_taskname_t Try6[8] = {"TaskTry6"};

os_stack_t TaskTryStack7[64];
os_taskname_t Try7[8] = {"TaskTry7"};

os_stack_t TaskTryStack8[64];
os_taskname_t Try8[8] = {"TaskTry8"};


taskptr_t TaskTry(void *TaskArgs)
{

	static uint32_t counter1 = 0,
					counter2 = 0,
					counter3 = 0;

	while(1)
	{
		counter1 = 250;
		counter2 = 225;
		counter3 = 350;
	}

}

taskptr_t TaskTry2(void *TaskArgs)
{
	static uint32_t counter1 = 0,
					counter2 = 0,
					counter3 = 0;

	while(1)
	{
		counter1 = 100;
		counter2 = 150;
		counter3 = 175;
	}
}


taskptr_t TaskTry3(void *TaskArgs)
{

	static uint32_t counter1 = 0,
					counter2 = 0,
					counter3 = 0;

	while(1)
	{
		counter1 = 100;
		counter2 = 150;
		counter3 = 175;
	}
}

taskptr_t TaskTry4(void *TaskArgs)
{

	static uint32_t counter1 = 0,
					counter2 = 0,
					counter3 = 0;

	while(1)
	{
		counter1 = 100;
		counter2 = 150;
		counter3 = 175;
	}
}


taskptr_t TaskTry5(void *TaskArgs)
{

	static uint32_t counter1 = 0,
					counter2 = 0,
					counter3 = 0;

	while(1)
	{
		counter1 = 100;
		counter2 = 150;
		counter3 = 175;
	}
}

taskptr_t TaskTry6(void *TaskArgs)
{

	static uint32_t counter1 = 0,
					counter2 = 0,
					counter3 = 0;

	while(1)
	{
		counter1 = 100;
		counter2 = 150;
		counter3 = 175;
	}
}


taskptr_t TaskTry7(void *TaskArgs)
{

	static uint32_t counter1 = 0,
					counter2 = 0,
					counter3 = 0;

	while(1)
	{
		counter1 = 100;
		counter2 = 150;
		counter3 = 175;
	}
}


taskptr_t TaskTry8(void *TaskArgs)
{

	static uint32_t counter1 = 0,
					counter2 = 0,
					counter3 = 0;

	while(1)
	{
		counter1 = 100;
		counter2 = 150;
		counter3 = 175;
	}
}

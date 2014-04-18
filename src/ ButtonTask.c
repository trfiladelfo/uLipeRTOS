/*
 *  ButtonTask.c
 *
 *  Created on: Apr 17, 2014
 *      Author: felipeneves
 */

#include "stm32f4xx.h"
#include "uLipe_RTOS.h"
#include "ButtonTask.h"

/*************************************************************
 	 	 	 	 Global variables
 *************************************************************/
//allocates a stack for a task
os_stack_t axButtonTaskStack[BUTTON_TASK_STACK_SIZE];

/*************************************************************
 	 	 	 	 local variables
 *************************************************************/
uint8_t bButtonState = teRELEASED;

/*************************************************************
 	 	 	 	 Local Prototypes
 *************************************************************/
void vButtonTaskInit(void);

/*************************************************************
 	 	 	 	 Functions
 *************************************************************/

/*************************************************************
		vButtonTaskInit()

		Initializes Button of the discovery board

		Param: N/A
		Ret  : N/A


 *************************************************************/
void vButtonTaskInit(void)
{

	//Configure the GPIOS:

	//Enable clock for GPIOA:
	RCC->AHB1ENR |= 0x01;

	//Configure data direction: (PD12 - 15, Output)
	GPIOA->MODER = 0x00000000;

	//Configures Pulls(PD12 - 15, PullDown):
	GPIOA->PUPDR = 0x00000000;

	//Reset all leds:
	//GPIOA->ODR = 0x00000000;
}

/*************************************************************
		bButtonGet()

		Get the current state of buttons

		Param: N/A
		Ret  : ButtonState - 0x00 - Released
							 0xFF - Pressed


 *************************************************************/
uint8_t bButtonGet(void)
{
	//return current button state
	return(bButtonState);
}

/*************************************************************
		vButtonMainTask()

		Main button task, scan button state

		Param: N/A
		Ret  : N/A


 *************************************************************/
void vButtonMainTask(void *pTaskArgs)
{
	//Allocate a debounce variable:
	uint16_t wDebounce = 0;

	//Allocate a temporary variable to buttons:
	uint8_t bCurButton = 0, bPrevButton = 0;

	//Initialize the task:
	vButtonTaskInit();

	//Every task is a infinite loop
	while(1)
	{
		//scan the button:
		bPrevButton = (uint8_t) (GPIOA->IDR & 0x01);

		//Wait a delay:
		wDebounce = 0xFFFF;

		//Loop it!
		while(wDebounce)
		{
			wDebounce--;
		}

		//Rescan the button:
		bCurButton = (uint8_t) (GPIOA->IDR & 0x01);

		//check if not a noise
		if(bCurButton == bPrevButton)
		{
			//if not, check state:
			if(bCurButton)
			{
				bButtonState = tePRESSED;
			}
			else
			{
				bButtonState = teRELEASED;
			}

		}
		//if a noise, do nothing
		else
		{
			//Reset the variables:
			bPrevButton = 0;
			bCurButton  = 0;
		}
	}
}

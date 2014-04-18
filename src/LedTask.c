/*
 * LedTask.c
 *
 * Simple Led task Demo
 *
 *  Created on: Apr 14, 2014
 *      Author: felipeneves
 */

#include "stm32f4xx.h"
#include "uLipe_RTOS.h"
#include "LedTask.h"
#include "ButtonTask.h"


/*************************************************************
 	 	 	 	 Global variables
 *************************************************************/
//stack of this task
os_stack_t axLedTaskStack[LED_TASK_STACK_SIZE];

/*************************************************************
 	 	 	 	 local variables
 *************************************************************/


/*************************************************************
 	 	 	 	 Local Prototypes
 *************************************************************/
void vLedTaskInit(void);

/*************************************************************
 	 	 	 	 Functions
 *************************************************************/

/*************************************************************
		vLedTaskInit()

		Initializes leds of the discovery board

		Param: N/A
		Ret  : N/A


 *************************************************************/
void vLedTaskInit(void)
{

	//Configure the GPIOS:

	//Enable clock for GPIOD:
	RCC->AHB1ENR |= 0x08;

	//Configure data direction: (PD12 - 15, Output)
	GPIOD->MODER = 0x55000000;

	//Configures Pulls(PD12 - 15, PullDown):
	GPIOD->PUPDR = 0xAA000000;

	//Reset all leds:
	GPIOD->ODR = 0x00000000;

}
/*************************************************************
		vLedSetValue()

		Turn a selected on or off

		Param: eLedState = Desired led state, can be
						   ON or OFF;

			   eLedNumber = Desired led number, check
			   			   	teLedNumber for available
			   			   	options;
		Ret  : N/A


 *************************************************************/
void vLedSetValue(teLedState eLedState, teLedNumber eLedNumber)
{
	//Check parameters:

	//check led states
	if((eLedState > teON) && (eLedState < teOFF))
	{
		//do nothing and return:
		return;
	}

	//check if leds exists:
	if(eLedNumber >= teMAX_LEDS)
	{
		//do nothing and return
		return;
	}

	//select led function:
	switch(eLedState)
	{
	case teON:
		//Turn On desired led:
		GPIOD->ODR |= (1 << eLedNumber);
	break;

	case teOFF:
		//Turn Off desired led:
		GPIOD->ODR &= ~(1 << eLedNumber);
	break;

	default:
	break;
	}

}

/*************************************************************
		bLedGetValue()

		Get the current value of leds

		Param: N/A

		Ret  : bLedStatus - Contains the leds status
							packed in a byte


 *************************************************************/
uint8_t bLedGetValue(void)
{
	//allocates a Led register:
	uint8_t bLeds = 0;

	//Read GPIOD leds:
	bLeds = (uint8_t)((GPIOD->ODR >> 12) & 0x0F);

	//return Result
	return(bLeds);
}
/*************************************************************
		vLedMainTask()

		Main task of leds management

		Param: pTaskArgs - Not used here

		Ret  : N/A

 *************************************************************/
void vLedMainTask(void *pvTaskArgs)
{
	//Allocates a delay register:
	uint16_t wDelay = 0;

	//Initialize the leds task
	vLedTaskInit();

	//Every task is a infinite loop
	while(1)
	{

	  //check if the button is pressed:
	  if(tePRESSED == bButtonGet())
	  {
		  //Set all leds and wait a while
		  vLedSetValue(teON, teLED1);
		  for(wDelay = 0; wDelay < LED_DELAY_SIZE; wDelay++);

		  vLedSetValue(teON, teLED2);
		  for(wDelay = 0; wDelay < LED_DELAY_SIZE; wDelay++);

		  vLedSetValue(teON, teLED3);
		  for(wDelay = 0; wDelay < LED_DELAY_SIZE; wDelay++);

		  vLedSetValue(teON, teLED4);
		  for(wDelay = 0; wDelay < LED_DELAY_SIZE; wDelay++);


		  //clear all leds and wait
		  vLedSetValue(teOFF, teLED1);
		  for(wDelay = 0; wDelay < LED_DELAY_SIZE; wDelay++);

		  vLedSetValue(teOFF, teLED2);
		  for(wDelay = 0; wDelay < LED_DELAY_SIZE; wDelay++);

		  vLedSetValue(teOFF, teLED3);
		  for(wDelay = 0; wDelay < LED_DELAY_SIZE; wDelay++);

		  vLedSetValue(teOFF, teLED4);
		  for(wDelay = 0; wDelay < LED_DELAY_SIZE; wDelay++);

	  }
	}
}

#include "stm32f4xx.h"
#include <stdio.h>
#include "uLipe_RTOS.h"
#include "Tasks.h"

/*
 * STM32F4 led blink sample.
 *
 * In debug configurations, demonstrate how to print a greeting message
 * on the standard output. In release configurations the message is
 * simply discarded. By default the trace messages are forwarded to the SWO,
 * but can be rerouted to semihosting or completely suppressed by changing
 * the definitions in misc/include/trace_impl.h.
 *
 * Then enter a continuous loop and blink a led with 1Hz.
 *
 * The external clock frequency is specified as HSE_VALUE=8000000,
 * adjust it for your own board. Also adjust the PLL constants to
 * reach the maximum frequency, or special clock configurations.
 *
 * The build does not use startup files, and on Release it does not even use
 * any standard library function (on Debug the printf() brigs lots of
 * functions; removing it should also use no other standard lib functions).
 *
 * If the application requires to use a special initialisation code present
 * in some other libraries (for example librdimon.a, for semihosting),
 * define USE_STARTUP_FILES and uncheck the corresponding option in the
 * linker configuration.
 */

// ----------------------------------------------------------------------------

static void
Delay(__IO uint32_t nTime);

static void
TimingDelay_Decrement(void);

void
SysTick_Handler(void);

/* ----- SysTick definitions ----------------------------------------------- */

#define SYSTICK_FREQUENCY_HZ       1000

/* ----- LED definitions --------------------------------------------------- */

/* STM32F4DISCOVERY definitions (the GREEN LED) */
/* Adjust them for your own board. */

#define BLINK_PORT      GPIOD
#define BLINK_PIN       12
#define BLINK_RCC_BIT   RCC_AHB1Periph_GPIOD

#define BLINK_TICKS     SYSTICK_FREQUENCY_HZ/2

// ----------------------------------------------------------------------------
volatile int seconds = 0;
volatile os_taskID_t ID;

volatile taskTCB_t *TestTCB = NULL, *AnotherTCB = NULL;

volatile os_error_t  Error = OS_OK; //cria um handles de erro

int
main(void)
{

	Asm_LowLevelTickInit(16800); //vamos testar!

	Task_InitBlocks();

	Task_Create(&TaskTry, &TaskTryStack, 0 , &Try, sizeof(Try));
	Task_Create(&TaskTry2, &TaskTryStack2, 1 , &Try2, sizeof(Try2));
	Task_Create(&TaskTry3, &TaskTryStack3, 3 , &Try3, sizeof(Try2));
	Task_Create(&TaskTry4, &TaskTryStack4, 9 , &Try4, sizeof(Try2));
	Task_Create(&TaskTry5, &TaskTryStack5, 7 , &Try5, sizeof(Try2));
	Task_Create(&TaskTry6, &TaskTryStack6, 2 , &Try6, sizeof(Try2));
	Task_Create(&TaskTry7, &TaskTryStack7, 12 , &Try7, sizeof(Try2));

	TestTCB	   = Task_Query(Task_GetID(&Try, sizeof(Try)));
	AnotherTCB = Task_Query(Task_GetID(&Try2, sizeof(Try2)));




	TestTCB->TaskAction(NULL); //executa a task

	//aqui vai trocar o contexto
	asm volatile
	(
			"LDR R4, =TestTCB 	 \n	" //carrega o TCB Desejado
			"LDR R5, =AnotherTCB \n "
			"LDR R0, [R4] \n"
			"LDR R1, [R5] \n"
			"BL  Asm_ContextChange"
	);




	while (1);

}

// ----------------------------------------------------------------------------

static __IO uint32_t uwTimingDelay;

/**
 * @brief  Inserts a delay time.
 * @param  nTime: specifies the delay time length, in SysTick ticks.
 * @retval None
 */
void
Delay(__IO uint32_t nTime)
{
  uwTimingDelay = nTime;

  while (uwTimingDelay != 0)
    ;
}

/**
 * @brief  Decrements the TimingDelay variable.
 * @param  None
 * @retval None
 */
void
TimingDelay_Decrement(void)
{
  if (uwTimingDelay != 0x00)
    {
      uwTimingDelay--;
    }
}

// ----------------------------------------------------------------------------

/**
 * @brief  This function is the SysTick Handler.
 * @param  None
 * @retval None
 */
void
SysTick_Handler(void)
{
  TimingDelay_Decrement();
}


void Task_Idle(void *TaskArgs)
{

}
// ----------------------------------------------------------------------------


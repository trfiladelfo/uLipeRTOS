/***********************************************************************
						ULIPE The RTOS

	file: uLipe_Core.h

	description: This file contains the OS Core interface

	author:Felipe Neves


 ************************************************************************/

//prevents recursive compilation:
#ifndef __ULIPE_CORE_H
#define __ULIPE_CORE_H 1

/************************************************************************
 	 	 	 	 module debug directive
 ************************************************************************/
#ifdef RTOS_DEBUG

//standard types
#include <stdint.h>

#endif

/************************************************************************
 							Defines
 ************************************************************************/
//defines the tick current value
#define TICKS  64

/************************************************************************
 							Macros
 ************************************************************************/


/************************************************************************
 	 	 	 	 	 	 	Typedefs
 ************************************************************************/


/************************************************************************
 	 	 	 	 	 Function Prototypes
 ************************************************************************/
extern void Core_StackFrameCreate(taskTCB_t *pxCurrTask);

extern os_error_t Core_Init(void);

extern void	Core_Start(void);

extern void	Core_Schedule(void);

extern void Core_ReadyTask(uint8_t OsPrio);

extern void Core_UnreadyTask(uint8_t OsPrio);

extern void Core_OSEnable(void);

extern void Core_OSDisable(void);

uint32_t Core_GetCurrentTick(void);

extern void Core_EnableSchedule(void);

extern void Core_DisableSchedule(void);

extern void Core_IdleTask(void *pxTaskArgs);

extern void Core_TimeTick(void);

//extern void Systick_Handler(void);
/****************************************************************************
 	 	 	 	 	 End of file
 ****************************************************************************/
#endif	/*ULIPE_CORE_H*/

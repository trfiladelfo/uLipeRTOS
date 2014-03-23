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
#define TICKS (NUMBER_OF_TASK + 1)
/************************************************************************
 							Macros
 ************************************************************************/



/************************************************************************
 	 	 	 	 	 	 	Typedefs
 ************************************************************************/


/************************************************************************
 	 	 	 	 	 Function Prototypes
 ************************************************************************/
extern os_error_t uLipe_Init(void);
extern void 	  uLipe_Schedule(void);
extern void		  uLipe_Start(void);
uint32_t uLipe_GetCurrentTick(void);
void 	   uLipe_EnableSchedule(void);
void       uLipe_DisableSchedule(void);
extern void uLipe_TimeTick(void);
extern void       Systick_Handler(void);



/****************************************************************************
 	 	 	 	 	 End of file
 ****************************************************************************/
#endif	/*ULIPE_CORE_H*/

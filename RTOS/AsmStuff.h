/***********************************************************************
						ULIPE The RTOS

	file: AsmStuff.S

	description: This file contains all the low level routines
				 such interrupts management and switch contex


	author:Felipe Neves

************************************************************************/

#ifndef ASMSTUFF_H_
#define ASMSTUFF_H_ //prevents recursive inclusion

/************************************************************************
 	 	 	 	 	 Include files
 ************************************************************************/
#include "uLipe_RTOS.h"

/************************************************************************
 	 	 	 	 	 Function Prototypes
 ************************************************************************/
extern void 	  Asm_LowLevelTickInit(uint32_t TickCount);
extern uint32_t   Asm_CriticalIn(void);
extern void       Asm_CriticalOut(uint32_t StatusRegister);
extern void       Asm_IntLevelContextChange(void);
extern void       Asm_TaskLevelContextChange(void);
void 	  		  PendSV_Handler(void);

/************************************************************************
 	 	 	 	 	 End of File
 ************************************************************************/
#endif /* ASMSTUFF_H_ */

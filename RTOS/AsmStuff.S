@***********************************************************************
@						ULIPE The RTOS
@
@	file: AsmStuff.S
@
@	description: This file contains all the low level routines
@				 such interrupts management and switch contex
@
@
@	author:Felipe Neves
@
@************************************************************************


.text 				@execution stuff begins here
.syntax unified		@ual assembly standart


.extern CurrentTaskBlock   @the current task TCB in execution
.extern HighReadyTaskBlock @the high priority ready task

.global Asm_LowLevelTickInit
.global Asm_CriticalIn
.global Asm_CriticalOut
.global Asm_IntLevelContextChange
.global Asm_TaskLevelContextChange
.global PendSV_Handler



@************************************************************************
@ 	 function:	Asm_LowLevelTickInit()
@
@	 description: this Initializes the tick counter and its
@				  interrupts
@
@ 	 parameters: TODO
@
@ 	 return:	N/A
@
@
@************************************************************************
Asm_LowLevelTickInit:

		PUSH {R4,R5 - R11} @;save context


		MOV  R1,R0 	 			@;Copies the desired tick
		PUSH {R1}
		MRS	 R2,MSP				@;save current sp

		MOV  R0, #0x00000002 	@;Control, use PSP
		MSR  CONTROL, R0		@;Write the new value
		ISB						@;recommended

		MSR  PSP,R2				@load PSP with current MSP

		LDR  R0, =0xE000E010    @;Load the systick control register contents
		MOV  R1, #0x00000000	@;
		STR	 R1, [R0], #0x04	@;
		POP  {R1}				@;retrieve Tick Value
		STR  R1, [R0]			@;Write the new value for load count
		SUBS R0, #0x04			@;Pints again to systick control gerister
								@;
								@;
		MOV  R1, #0x00000003    @;Enable systick timer and its interrupt
		STR  R1, [R0],#0x04 	@;Write back on the systick register, and points to
							    @;Load register
								@;
		POP	{R4,R5 - R11}		@;
		BX	LR					@;Return from sub

@************************************************************************
@ 	 function:	Asm_CriticalIn()
@
@	 description: Disables all interrupts
@
@ 	 parameters: TODO
@
@ 	 return:	N/A
@
@
@************************************************************************
Asm_CriticalIn:

		CPSID	I					@disables all interrupts
		BX		LR					@Return from sub

@************************************************************************
@ 	 function:	Asm_CriticalIn()
@
@	 description: Enables all interrupts
@
@ 	 parameters: TODO
@
@ 	 return:	N/A
@
@
@************************************************************************
Asm_CriticalOut:

		CPSIE	I					@enables all interrupts
		BX		LR					@Return from sub

@************************************************************************
@ 	 function:	Asm_IntLevelContextChange()
@
@	 description: This function performs a request to PEND SV
@				  in order tochange a context
@
@ 	 parameters: TODO
@
@ 	 return:	N/A
@
@
@************************************************************************
Asm_IntLevelContextChange:


		BX LR 					@Return from sub

@************************************************************************
@ 	 function:	Asm_TaskLevelContextChange()
@
@	 description: This function performs a request to PEND SV
@				  in order tochange a context
@
@ 	 parameters: TODO
@
@ 	 return:	N/A
@
@
@************************************************************************
Asm_TaskLevelContextChange:
		PUSH {R0 - R3}			@save using registers

		LDR  R0, = 0xE000ED22  @PENDSV Priority
		LDR  R1, [R0]
		ORR  R1,0x0FF			@lowest priority
		STR  R1,[R0]

		LDR  R0, = 0xE000ED04   @SCB->ICSR register
		LDR  R1, [R0]			@load current value
		ORR  R1, #0x10000000	@set SV handler
		STR  R1, [R0]			@write it back


		POP  {R0 - R3}

		BX LR 					@Return from sub

@;************************************************************************
@; 	 function:	PendSV_Handler()
@;
@;	 description: performs a context switch
@;
@;
@; 	 parameters: TODO
@;
@; 	 return:	N/A
@;
@;
@;************************************************************************
PendSV_Handler:
		LDR 	R0, =CurrentTaskBlock	@Get the current TCB
		MRS		R1, PSP					@;Get the current SP
		STMDB	R1!,{R4 - R11}			@;save R4 - R11 on the current stack
										@;
		STR		R1,[R0,#0x00000014] 	@;Save the current stack pointer

		LDR		R0,=HighReadyTaskBlock  @Get the first task from de queue
		LDR		R0,[R0,#0x00000014]		@;Get the new stack pointer
		LDR     R1,[R0]					@
									    @
		LDMIA	R1!,{R4 - R11}			@;Change the context
		MSR		PSP , R1				@;write the new stackpointer

		BX		LR						@;Return from sub

.end @;execution stuff ends here


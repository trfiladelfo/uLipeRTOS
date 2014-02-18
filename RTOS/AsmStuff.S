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
.thumb_func
Asm_LowLevelTickInit:


		CPSID 	I				@Disable interrupts
								@
		LDR  R1, =0xE000ED14	@SCB->CCR
		LDR  R2, [R1]			@
		ORR  R2,R2,#0x200		@Enable double word aling
		STR	 R2,[R1]			@
								@
		LDR  R1, =0xE000E010    @Load the systick control register contents
		MOVS R2, #0x00000000	@
		STR	 R2, [R1], #0x04	@
		STR  R0, [R1]			@Write the new value for load count
		SUBS R1, #0x04			@Points again to systick control gerister
								@
								@
		MOVS R2, #0x00000003    @Enable systick timer and its interrupt
		STR  R2, [R1]		 	@Write back on the systick register, and points to
							    @Load register
								@
		LDR   R1, =0xE000ED22  	@PENDSV Priority
		MOVS  R0, #0xFF			@
		STRB  R0,[R1]			@set pend sv priority
								@
		CPSIE  I				@Enable global interrupts
		BX	LR					@Return from sub

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
.thumb_func
Asm_CriticalIn:
		MRS     R0, PRIMASK			@save status register
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
.thumb_func
Asm_CriticalOut:

		MSR	    PRIMASK,R0				@retrieve saved sr register
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
.thumb_func
Asm_IntLevelContextChange:

		CPSID 	I					@Disables interrrupts
		LDR  	R0, =0xE000ED04   	@SCB->ICSR register
		MOVS	R1,#0x10000000	  	@Requests a PENDSV
									@exception
									@
		STR		R1,[R0]				@
		CPSIE	I					@Enable interrupts
									@
		BX LR 					  	@Return from sub

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
.thumb_func
Asm_TaskLevelContextChange:

		CPSID 	I					@Disables interrrupts
		LDR  	R0, =0xE000ED04   	@SCB->ICSR register
		MOVS	R1,#0x10000000	  	@Requests a PENDSV
									@exception
									@
		STR		R1,[R0]				@
		CPSIE	I					@Enable interrupts
									@
									@
		BX 		LR 					@Return from sub

@************************************************************************
@ 	 function:	PendSV_Handler()
@
@	 description: performs a context switch
@
@
@ 	 parameters: TODO
@
@ 	 return:	N/A
@
@
@************************************************************************
.thumb_func
PendSV_Handler:
		TST		LR,#4					@check the origin of interrupt
		IT		EQ						@if, it from a MSP usage
		BEQ		FIRST_CHANGE			@
										@
		LDR		R0, =HighReadyTaskBlock @if not,
		LDR		R0,[R0]					@
		LDR		R1,[R0]					@Check the current taskFLag
		TST		R1,#1					@Check if first time flag is set
		IT		NE						@
		BNE 	SECOND_CHANGE			@Jump for first context switch
										@
										@if, not, runs context switch normally
		LDR 	R0, =CurrentTaskBlock	@Get the current TCB
		LDR		R0,[R0]					@
		MRS		R1, PSP					@Get the current SP
		STMDB	R1!,{R4 - R11}			@save R4 - R11 on the current stack
										@
		STR		R1,[R0,#0x00000018] 	@Save the current stack pointer
										@
		LDR		R0,=HighReadyTaskBlock  @Get the first task from de queue
		LDR		R0,[R0]					@Get the new stack pointer
		LDR		R1, [R0,#0x00000018]	@
									    @
		LDMIA	R1!,{R4 - R11}			@Reload R4 -- R11
		MSR		PSP , R1				@write the new stackpointer
NO_SAVE:								@
		ORR     LR,LR,#0x00000004		@ensures that return in PSP
		BX		LR						@Return from sub
										@
FIRST_CHANGE:							@
		MOVS    R4, #0x02				@if the first run
		MSR	    CONTROL, R4				@uses PSP always
		ISB								@recommended after write control
		LDR		R0,=HighReadyTaskBlock  @Get the newTCB
		LDR		R0,[R0]					@Get the new stack pointer
		LDR		R1,[R0]					@Get task flags
		BIC		R1,R1,#1				@clear first time flag
		STR		R1,[R0]					@
		LDR		R1, [R0,#0x00000018]	@
		MSR     PSP,R1					@Load the first stack pointer
		B		NO_SAVE					@

SECOND_CHANGE:
		LDR 	R0, =CurrentTaskBlock	@Get the current TCB
		LDR		R0,[R0]					@
		MRS		R1, PSP					@Get the current SP
		STMDB	R1!,{R4 - R11}			@save R4 - R11 on the current stack
		STR		R1,[R0,#0x00000018] 	@Save the current stack pointer
										@
		LDR		R0,=HighReadyTaskBlock  @Get the newTCB
		LDR		R0,[R0]					@Get the new stack pointer
		LDR		R1,[R0]					@Get task flags
		BIC		R1,R1,#1				@clear first time flag
		STR		R1,[R0]					@
		LDR		R1, [R0,#0x00000018]	@
		MSR     PSP,R1					@Load the first stack pointer
		B		NO_SAVE					@


.end @;execution stuff ends here


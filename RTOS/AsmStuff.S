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

.global Asm_LowLevelTickInit
.global Asm_ContextChange



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



@;************************************************************************
@; 	 function:	Asm_ContextChange()
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
Asm_ContextChange:

		MRS		R2, PSP				@;Get the current SP
		STMDB	R2!,{R4 - R11}		@;save R4 - R11 on the current stack
									@;
		STR		R2,[R0,#0x00000014] @;Save the current stack pointer
									@;
		LDR		R2,[R1,#0x00000014]	@;Get the new stack pointer
									@;
		LDMIA	R2!,{R4 - R11}		@;Change the context
		MSR		PSP , R2			@;write the new stackpointer

		BX		LR					@;Return from sub






.end @;execution stuff ends here


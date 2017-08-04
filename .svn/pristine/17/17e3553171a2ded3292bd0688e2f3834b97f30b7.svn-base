
.globl keyboard_link,rtc_link,sched_link,mouse_link


.align 4

keyboard_link:
	#Clear interrupt flags
	cli
	#push all the registers
	push %es
	push %ds
	pushal
	pushfl
	#Call the interrupt handler
	call keyboard_int_handler
	#Restore all the registers
	popfl
	popal
	popl %ds
	popl %es
	#Restore int flags
	sti
	#Return
	iret


rtc_link:
	#Clear interrupt flags
	cli
	#push all the registers
	push %es
	push %ds
	pushal
	pushfl
	#Call the interrupt handler
	call rtc_int_handle
	#Restore all the registers
	popfl
	popal
	popl %ds
	popl %es
	#Restore int flags
	sti
	#Return
	iret


mouse_link:
	#Clear interrupt flags
	cli
	#push all the registers
	push %es
	push %ds
	pushal
	pushfl
	#Call the interrupt handler
	call mouse_int_handle
	#Restore all the registers
	popfl
	popal
	popl %ds
	popl %es
	#Restore int flags
	sti
	#Return
	iret

sched_link:
	#Clear interrupt flags
	cli
	#push all the registers
	push %es
	push %ds
	pushal
	pushfl
	#Call the interrupt handler
	call scheduling
	#Restore all the registers
	popfl
	popal
	popl %ds
	popl %es
	#Restore int flags
	sti
	#Return
	iret



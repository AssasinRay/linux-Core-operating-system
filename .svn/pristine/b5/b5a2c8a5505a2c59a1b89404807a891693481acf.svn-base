.global syscall_wrapper




syscall_wrapper:
	
	cli



	pushal
	pushfl

	cmpl $1, %eax
	jl invalid
	cmpl $11, %eax
	jg invalid

	pushl %edx
	pushl %ecx
	pushl %ebx



	decl %eax

	call *jumptable(, %eax, 4)
	movl %eax, temp_eax

	popl %ebx
	popl %ecx
	popl %edx

	popfl
	popal

	movl temp_eax, %eax

	sti
	iret


invalid:

	popfl
	popal

	sti
	movl $-1, %eax
	iret


jumptable:
	.long halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn, ktime

temp_eax:
	.long 0
